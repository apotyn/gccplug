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

#ifndef GCC_GIMPLE_PASS_H
#define GCC_GIMPLE_PASS_H

#include "gcc-common-header.h"
#include <tree-pass.h>
#include <context.h>


#define DECLARE_GIMPLE_PASS(name, pdata, gatefn, execfn) \
struct GCC_CONCAT2(name, _pass): gimple_opt_pass { \
	GCC_CONCAT2(name, _pass)(gcc::context * ctx_, void * udata_): \
		gimple_opt_pass((pdata), ctx_), udata(udata_) {}; \
	virtual bool gate(function * fn) override { \
		return gatefn(udata, fn); \
	} \
	virtual unsigned int execute(function * fn) override { \
		return execfn(udata, fn); \
	}\
	virtual GCC_CONCAT2(name, _pass) * clone() override { \
		return this; \
	} \
	void * udata; \
};

#define create_gimple_pass(name, ctx, udata) \
	new GCC_CONCAT2(name, _pass)((ctx), (udata))


#endif /* #ifndef GCC_GIMPLE_PASS_H */

/* vim: set ts=4 tw=78 noet: */

