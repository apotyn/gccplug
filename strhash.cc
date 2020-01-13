/*****************************************************************************
 * Copyright (C) 2020 Alexander Potylitsin <apotyn@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 *
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "gcc-common-header.h"
#include "gcc-gimple-pass.h"
#include "gcc-log-utils.h"

#include <function.h>
#include <tree.h>

#include <gimple.h>
#include <gimple-iterator.h>
#include <gimple-expr.h>
#include <gimple-walk.h>

#include <diagnostic.h>

#include "hashfns.h"


/*****************************************************************************
 * plugin global state
 ****************************************************************************/

static const char * const version_string = "0.1";
static const char * const help_string = "Compile-time string hashing support.";
static const char * plugin_name = "";
static bool enable_mismatch_args_warning = false;
static bool enable_non_literal_arg_warning = false;
static bool enable_call_replacement_warning = false;


/*****************************************************************************
 * known hash functions
 ****************************************************************************/

static unsigned int noop_hash(const char * s) {
	return 0xdeadbeef;
}

static unsigned int (* lookup_hashfn(const char * name))(const char *) {
#	pragma push_macro("HASHFN_ENTRY")
#	define HASHFN_ENTRY(fn) \
		{ .name = GCC_STRINGIFY(fn), .hashfn = fn }

	static const struct {
		const char * name;
		unsigned int (* hashfn)(const char *);
	} ftab[] = {
		HASHFN_ENTRY(noop_hash),
		HASHFN_ENTRY(djb2_hash),
		HASHFN_ENTRY(sdbm_hash),
		HASHFN_ENTRY(lose_hash),
		HASHFN_ENTRY(rs_hash),
		HASHFN_ENTRY(js_hash),
		HASHFN_ENTRY(pjw_hash),
		HASHFN_ENTRY(elf_hash),
		HASHFN_ENTRY(bkdr_hash),
		HASHFN_ENTRY(mabkdr_hash),
		HASHFN_ENTRY(dek_hash),
		HASHFN_ENTRY(ap_hash),
		HASHFN_ENTRY(ly_hash),
		HASHFN_ENTRY(rot13_hash),
		HASHFN_ENTRY(faq6_hash),
		HASHFN_ENTRY(fnv1_hash),
		HASHFN_ENTRY(fnv1a_hash)
	};
#pragma pop_macro("HASHFN_ENTRY")

	for (unsigned int i = 0; i < GCC_COUNTOF(ftab); ++i) {
		if (strcmp(name, ftab[i].name) == 0) {
			return ftab[i].hashfn;
		}
	}

	return NULL;
}


/*****************************************************************************
 * gimple hashing calls replacement pass
 ****************************************************************************/

static const char * addr_string_cst(gimple * stmt, int i) {
	tree expr = gimple_call_arg(stmt, i);
	if (ADDR_EXPR == TREE_CODE(expr)) {
		tree arg = TREE_OPERAND(expr, 0);
		if (STRING_CST == TREE_CODE(arg)) {
			return TREE_STRING_POINTER(arg);
		}
	}
	return NULL;
}

static struct gimple * build_unsigned_assign(tree lhs, unsigned int x) {
	tree rhs = build_int_cst(unsigned_type_node, x);
	gassign * assign = gimple_build_assign(lhs, rhs);
	return (assign);
}

static bool strhash_pass_gate(void *, function * fn) {
	return true;
}

static unsigned int strhash_pass_execute(void *, function * fn) {
	gimple_seq gimple_body = fn->gimple_body;
	//print_gimple_seq(stderr, gimple_body, 0, 0);

	gimple_stmt_iterator gsi;
	for (gsi = gsi_start(gimple_body); !gsi_end_p(gsi); gsi_next(&gsi)) {
		gimple * stmt = gsi_stmt(gsi);
		location_t locus = gimple_location(stmt);

		/* check for function call. */
		if (!is_gimple_call(stmt)) continue;

		/* retrive function name and lookup its implementation. */
		tree fndecl = gimple_call_fndecl(stmt);
		const char * fname = get_name(fndecl);
		unsigned int (* hashfn)(const char *) = lookup_hashfn(fname);
		if (!hashfn) continue;

		/* check the function has exactly one argument. */
		if (1 != gimple_call_num_args(stmt)) {
			if (enable_mismatch_args_warning) {
				warning_at(locus, 0, "Hash function %qs called with multiple arguments.", fname);
				inform(locus, "Folding to integer constant will NOT be performed.");
			}
			continue;
		}

		/* retrive argument expression. */
		const char * str = addr_string_cst(stmt, 0);
		if (!str) {
			if (enable_non_literal_arg_warning) {
				warning_at(locus, 0, "Hash function %qs called with non literal string argument.", fname);
				inform(locus, "Folding to integer constant will NOT be performed.");
			}
			continue;
		}

		/* here we are replacing the function call with constant assignment. */
		unsigned int hval = hashfn(str);
		if (enable_call_replacement_warning) {
			warning_at(locus, 0, "Replacing %<%s(\"%s\")%> with %qu", fname, str, hval);
		}
		tree lhs = gimple_call_lhs(stmt);
		gimple * newstmt = build_unsigned_assign(lhs, hval);
		gsi_replace(&gsi, newstmt, false);
	}

	return 0;
}

static struct pass_data strhash_pass_data = {
	.type = GIMPLE_PASS,
	.name = "strhash_pass",
	.optinfo_flags = OPTGROUP_NONE,
	.tv_id = TV_NONE,
	.properties_required = PROP_gimple_any,
	.properties_provided = 0,
	.properties_destroyed = 0,
	.todo_flags_start = 0,
	.todo_flags_finish = 0,
};

DECLARE_GIMPLE_PASS(strhash_pass, strhash_pass_data, strhash_pass_gate, strhash_pass_execute);


/*****************************************************************************
 * gcc plugin main
 ****************************************************************************/

static bool parse_args(int argc, struct plugin_argument * argv) {
	for (int i = 0; i < argc; ++i) {
		const char * key = argv[i].key;

		if (strcmp(key, "version") == 0) {
			logf("%s\n", version_string);
		}
		else
		if (strcmp(key, "help") == 0) {
			logf("%s\n", help_string);
		}
		else
		if (strcmp(key, "mismatch-args-warning") == 0) {
			enable_mismatch_args_warning = true;
		}
		else
		if (strcmp(key, "no-mismatch-args-warning") == 0) {
			enable_mismatch_args_warning = false;
		}
		else
		if (strcmp(key, "non-literal-arg-warning") == 0) {
			enable_non_literal_arg_warning = true;
		}
		else
		if (strcmp(key, "no-non-literal-arg-warning") == 0) {
			enable_non_literal_arg_warning = false;
		}
		else
		if (strcmp(key, "call-replacement-warning") == 0) {
			enable_call_replacement_warning = true;
		}
		else
		if (strcmp(key, "no-call-replacement-warning") == 0) {
			enable_call_replacement_warning = false;
		}
		else {
			error("unknown option %<-fplugin-arg-%s-%s%>", plugin_name, key);
			return false;
		}
	}

	return true;
}

/* see http://gcc.gnu.org/onlinedocs/gccint/Plugin-API.html#Plugin-API */
GCC_API int plugin_is_GPL_compatible = 1;

/* see http://gcc.gnu.org/onlinedocs/gccint/Plugin-API.html#Plugin-API */
GCC_API int plugin_init(struct plugin_name_args * plugin_info, struct plugin_gcc_version * version) {
	plugin_name = plugin_info->base_name;

	/* check gcc version. */
	if (!plugin_default_version_check(version, &gcc_version)) {
		error("incompatible gcc/plugin versions");
		logver("unsupported gcc", version);
		logver("required gcc", &gcc_version);
		return 1;
	}

	/* parse command-line arguments */
	if (!parse_args(plugin_info->argc, plugin_info->argv)) {
		return 1;
	}

	/* register plugin info. */
	static struct plugin_info strhash_info = {
		.version = version_string,
		.help = help_string
	};
	register_callback(plugin_name, PLUGIN_INFO, NULL, &strhash_info);

	/* register my pass */
	static struct register_pass_info pass_info = {
		.pass = create_gimple_pass(strhash_pass, g, NULL),
		.reference_pass_name = "cfg",
		.ref_pass_instance_number = 1,
		.pos_op = PASS_POS_INSERT_BEFORE,
	};
	register_callback(plugin_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &pass_info);

	return 0;
}

/* vim: set ts=4 tw=78 noet: */
