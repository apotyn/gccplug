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

#ifndef HASHFNS_H
#define HASHFNS_H

#ifdef __cpluplus
extern "C" {
#endif

unsigned int djb2_hash(const char * s);
unsigned int sdbm_hash(const char * s);
unsigned int lose_hash(const char * s);
unsigned int rs_hash(const char * s);
unsigned int js_hash(const char * s);
unsigned int pjw_hash(const char * s);
unsigned int elf_hash(const char * s);
unsigned int bkdr_hash(const char * s);
unsigned int mabkdr_hash(const char * s);
unsigned int dek_hash(const char * s);
unsigned int ap_hash(const char * s);
unsigned int ly_hash(const char * s);
unsigned int rot13_hash(const char * s);
unsigned int faq6_hash(const char * s);
unsigned int fnv1_hash(const char * s);
unsigned int fnv1a_hash(const char * s);
unsigned int q3cvars_hash(const char * s);
unsigned int my1_hash(const char * s);

#ifdef __cpluplus
}
#endif

#endif /* #ifndef HASHFNS_H */

/* vim: set ts=4 tw=78 noet: */

