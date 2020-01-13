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
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "hashfns.h"


/****************************************************************************
 * hash function wrappers will never be replaced by integer constant
 ***************************************************************************/

static unsigned int noop_hash(const char * s) {
	return 666;
}

static unsigned int runtime_noop_hash(const char * s) {
	return noop_hash(s);
}

static unsigned int runtime_pjw_hash(const char * s) {
	return pjw_hash(s);
}


/****************************************************************************
 * tests
 ***************************************************************************/

#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY(x)
#define CONCAT(x, y) x ## y
#define RUNTIME_HASH(hashfn, x) CONCAT(runtime_, hashfn)(x)
#define STATIC_HASH(hashfn, x) hashfn(x)

#define expect(expr) \
	do { \
		const char * msg = STRINGIFY(expr); \
		bool success = !!(expr); \
		printf("%s : %s\n", msg, (success) ? "ok" : "failed"); \
	} while (0)


int main(int argc, char **argv) {

	/* check gcc plugin is working */
	expect(RUNTIME_HASH(noop_hash, "hello") == 666);
	expect(STATIC_HASH(noop_hash, "hello") == 0xdeadbeef);

	/* static hash should be replaced during compile time */
	expect(STATIC_HASH(pjw_hash, "qwerty") == RUNTIME_HASH(pjw_hash, "qwerty"));

	/* could not do compile time replacement for nonliteral */
	const char * s = argv[0];
	expect(STATIC_HASH(pjw_hash, s) == RUNTIME_HASH(pjw_hash, s));

	return EXIT_SUCCESS;
}

/* vim: set ts=4 tw=78 noet: */

