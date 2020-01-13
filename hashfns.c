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

#include "hashfns.h"


/*
 * http://www.cse.yorku.ca/~oz/hash.html
 * this algorithm (k=33) was first reported by dan bernstein many years ago
 * in comp.lang.c. another version of this algorithm (now favored by
 * bernstein) uses xor: hash(i) = hash(i - 1) * 33 ^ str[i];
 * the magic of number 33 (why it works better than many other constants,
 * prime or not) has never been adequately explained.
 */
unsigned int djb2_hash(const char * s) {
	unsigned int hash = 5381;
	unsigned int c;

	while (c = (unsigned char)*s++) {
		/* hash = hash * 33 + c */
		hash = ((hash << 5) + hash) + c;
	}

	return hash;
}

/*
 * http://www.cse.yorku.ca/~oz/hash.html
 * this algorithm was created for sdbm (a public-domain reimplementation of
 * ndbm) database library. it was found to do well in scrambling bits,
 * causing better distribution of the keys and fewer splits. it also happens
 * to be a good general hashing function with good distribution.
 * the actual function is hash(i) = hash(i - 1) * 65599 + str[i];
 * what is included below is the faster version used in gawk.
 * [there is even a faster, duff-device version] the magic constant 65599 was
 * picked out of thin air while experimenting with different constants, and
 * turns out to be a prime. this is one of the algorithms used in berkeley db
 * (see sleepycat) and elsewhere.
 */
unsigned int sdbm_hash(const char * s) {
	unsigned int hash = 0;
	unsigned int c;

	while (c = (unsigned char)*s++) {
		hash = c + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}

/*
 * http://www.cse.yorku.ca/~oz/hash.html
 * This hash function appeared in K&R (1st ed) but at least the reader was
 * warned: "This is not the best possible algorithm, but it has the merit of
 * extreme simplicity." This is an understatement; It is a terrible hashing
 * algorithm, and it could have been much better without sacrificing its
 * "extreme simplicity." [see the second edition!]
 * Many C programmers use this function without actually testing it, or
 * checking something like Knuth's Sorting and Searching, so it stuck.
 * It is now found mixed with otherwise respectable code, eg. cnews. sigh.
 * [see also: http://cm.bell-labs.com/cm/cs/tpop/index.html]
 */
unsigned int lose_hash(const char * s) {
	unsigned int hash = 0;
	unsigned int c;

	while (c = (unsigned char)*s++) {
		hash += c;
	}

	return hash;
}

/*
 * http://vak.ru/doku.php/proj/hash/sources
 * http://www.partow.net/programming/hashfunctions/
 */
unsigned int rs_hash(const char * s) {
	unsigned int b = 378551;
	unsigned int a = 63689;
	unsigned int hash = 0;
	unsigned int c;

	while (c = (unsigned char)*s++) {
		hash = hash * a + c;
		a = a * b;
	}

	return hash;
}

/*
 * http://vak.ru/doku.php/proj/hash/sources
 * http://www.partow.net/programming/hashfunctions/
 */
unsigned int js_hash(const char * s) {
	unsigned int hash = 1315423911;
	unsigned int c;

	while (c = (unsigned char)*s++) {
		hash ^= ((hash << 5) + c + (hash >> 2));
	}

	return hash;
}

/*
 * http://vak.ru/doku.php/proj/hash/sources
 * http://www.partow.net/programming/hashfunctions/
 * http://ru.wikipedia.org/wiki/PJW-32
 */
unsigned int pjw_hash(const char * s) {
	unsigned int hash = 0;
	unsigned int test = 0;
	unsigned int i = 0;
	unsigned int c;

	while (c = (unsigned char)*s++) {
		hash = (hash << 4) + c;
		if ((test = hash & 0xF0000000U) != 0) {
			hash = ((hash ^ (test >> 24)) & 0x0FFFFFFFU);
		}
	}

	return hash;
}

/*
 * http://vak.ru/doku.php/proj/hash/sources
 */
unsigned int elf_hash(const char * s) {
	unsigned int hash = 0;
	unsigned int x = 0;
	unsigned int c;

	while (c = (unsigned char)*s++) {
		hash = (hash << 4) + c;
		if ((x = hash & 0xF0000000L) != 0) {
			hash ^= (x >> 24);
			hash &= ~x;
		}
	}

	return hash;
}

/*
 * http://vak.ru/doku.php/proj/hash/sources
 * http://www.partow.net/programming/hashfunctions/
 * http://ru.wikipedia.org/wiki/BKDR
 */
unsigned int bkdr_hash(const char * s) {
	unsigned int seed = 131313;   /* 31 131 1313 13131 131313 etc.. */
	unsigned int hash = 0;
	unsigned int c;

	while (c = (unsigned char)*s++) {
		hash = (hash * seed) + c;
	}

	return hash;
}

/*
 * http://ru.wikipedia.org/wiki/BKDR
 */
unsigned int mabkdr_hash(const char * s) {
	unsigned int seed = 131313;   /* 31 131 1313 13131 131313 etc.. */
	unsigned int hash = 0;
	unsigned int i;

	for (i = 0; s[i] != 0; ++i) {
		hash = (hash * seed) + (unsigned char)(s[i]) + /* maBKDR modification here*/ i;
	}

	return hash;
}

/*
 * http://vak.ru/doku.php/proj/hash/sources
 */
unsigned int dek_hash(const char * s) {
	unsigned int hash = 0;
	unsigned int c;

	for (hash = 0; s[hash] != 0; ++hash);

	while (c = (unsigned char)*s++) {
		hash = ((hash << 5) ^ (hash >> 27)) ^ c;
	}

	return hash;
}

/*
 * http://vak.ru/doku.php/proj/hash/sources
 * http://ru.wikipedia.org/wiki/APHash
 */
unsigned int ap_hash(const char * s) {
	unsigned int hash = 0;
	unsigned int i;

	for (i = 0; s[i] != 0; ++i) {
		hash ^= ((i & 1) == 0) ?
			((hash << 7) ^ ((unsigned char) s[i]) ^ (hash >> 3)) :
			(~((hash << 11) ^ ((unsigned char) s[i]) ^ (hash >> 5)));
	}

	return hash;
}

/*
 * http://vak.ru/doku.php/proj/hash/sources
 * Congruential generator proposed by  Leonid Yuriev.
 * Multiplier constant suggested by M.Lavaux & F.Janssens. 
 */
unsigned int ly_hash(const char * s) {
	unsigned int hash = 0;
	unsigned int c;

	while (c = (unsigned char)*s++) {
		hash = (hash * 1664525) + c + 1013904223;
	}

	return hash;
}

/*
 * http://vak.ru/doku.php/proj/hash/sources
 * http://ru.wikipedia.org/wiki/Rot13
 * No multiplication, by Serge Vakulenko.
 * Two shifts are converted by GCC 4 to a single rotation instruction.
 */
unsigned int rot13_hash(const char * s) {
	unsigned int hash = 0;
	unsigned int c;

	while (c = (unsigned int)*s++) {
		hash = hash + c - (hash << 13) | (hash >> 19);
	}

	return hash;
}

/*
 * http://vak.ru/doku.php/proj/hash/sources
 * http://ru.wikipedia.org/wiki/FAQ6
 * From Bob Jenkins Hash Function FAQ: http://burtleburtle.net/bob/hash/hashfaq.html
 */
unsigned int faq6_hash(const char * s) {
	unsigned int hash = 0;
	unsigned int c;

	while (c = (unsigned char)*s++) {
		hash += c;
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

/*
 * http://ru.wikipedia.org/wiki/FNV
 */
unsigned int fnv1_hash(const char * s) {
	unsigned int hash = 0x811c9dc5;
	unsigned int c;

	while (c = (unsigned char)*s++) {
		hash *= 0x01000193;
		hash ^= c;
	}

	return hash;
}

/*
 * http://ru.wikipedia.org/wiki/FNV
 */
unsigned int fnv1a_hash(const char * s) {
	unsigned int hash = 0x811c9dc5;
	unsigned int c;

	while (c = (unsigned char)*s++) {
		hash ^= c;
		hash *= 0x01000193;
	}

	return hash;
}

/*
 * karmak, quake3
 */
unsigned int q3cvars_hash(const char * s) {
	unsigned int hash = 0;
	unsigned int i;
	char c;

	for (i = 0; s[i] != 0; ++i) {
		c = (s[i] >= 'A' && s[i] <= 'Z') ? (s[i] - 'A' + 'a') : s[i];
		hash += (unsigned int)(unsigned char)(c) * (i + 119);
	}

	return hash;
}

/*
 *  faq6 only for [A-Za-z]
 */
unsigned int my1_hash(const char * s) {
	unsigned int hash = 0;
	unsigned int i;

	for (i = 0; s[i] != 0; ++i) {
		if (s[i] >'A' && s[i] < 'Z') {
			hash += (unsigned int)(s[i] - 'A');
			hash += (hash << 10);
			hash ^= (hash >> 6);
		}
		else
		if (s[i] >'a' && s[i] < 'z') {
			hash += (unsigned int)(s[i] - 'a');
			hash += (hash << 10);
			hash ^= (hash >> 6);
		}
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

/* vim: set ts=4 tw=78 noet: */

