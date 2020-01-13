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

#ifndef GCC_COMMON_HEADER_H
#define GCC_COMMON_HEADER_H

/* include BUILDING_GCC_{MAJOR,MINOR,PATCHLEVEL,VERSION} */
#include <bversion.h>

/* include plugin definitions */
#if BUILDING_GCC_VERSION >= 6000
#	include <gcc-plugin.h>
#else
#	include <plugin.h>
#endif

/* include struct plugin_gcc_version gcc_version */
#include <plugin-version.h>

#include <config.h>
#include <system.h>
#include <coretypes.h>


/* utility macros */
#define __GCC_STRINGIFY(x) #x
#define GCC_STRINGIFY(x) __GCC_STRINGIFY(x)
#define GCC_CONCAT2(x, y) x ## y
#define GCC_CONCAT3(x, y, z) x ## y ## z
#define GCC_COUNTOF(x) (sizeof(x) / sizeof(x[0]))


/* plugin API */
#ifdef _WIN32
#	define GCC_API __declspec(dllexport)
#else
#	define GCC_API
#endif

#endif /* #ifndef GCC_COMMON_HEADER_H */

/* vim: set ts=4 tw=78 noet: */

