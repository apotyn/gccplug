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

#ifndef GCC_LOG_UTILS_H
#define GCC_LOG_UTILS_H

#ifdef __cpluplus
extern "C" {
#endif

void vlogf(const char * fmt, va_list alist);
void logf(const char * fmt, ...);
void logver(const char * prefix, const struct plugin_gcc_version * version);

size_t escaped(char * buf, size_t n, const char * s, size_t len);

#ifdef __cpluplus
}
#endif

#endif /* #ifndef GCC_LOG_UTILS_H */

/* vim: set ts=4 tw=78 noet: */

