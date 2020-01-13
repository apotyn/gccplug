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
#include <stdarg.h>

#include "gcc-common-header.h"
#include "gcc-log-utils.h"


void vlogf(const char * fmt, va_list alist) {
	vfprintf(stderr, fmt, alist);
}

void logf(const char * fmt, ...) {
	va_list alist;
	va_start(alist, fmt);
	vlogf(fmt, alist);
	va_end(alist);
}

void logver(const char * prefix, const struct plugin_gcc_version * version) {
	logf("%s: version=%s datestamp=%s devphase=%s revision=%s\n",
		prefix, version->basever, version->datestamp,
		version->devphase, version->revision);
}

size_t escaped(char * buf, size_t n, const char * s, size_t len) {
	size_t p, i;

	for (p = 0, i = 0; p < n && i < len; ++i) {
		if (s[i] >= 32 && s[i] <= 127) {
			buf[p++] = s[i];
		}
		else {
			p += snprintf(buf + p, n - p, "\\x%02x", s[i]);
		}
		buf[p] = 0;
	}

	return p;
}

/* vim: set ts=4 tw=78 noet: */

