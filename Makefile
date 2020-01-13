##############################################################################
# Copyright (C) 2020 Alexander Potylitsin <apotyn@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see http://www.gnu.org/licenses/.
#
##############################################################################


HOST_GCC=g++
TARGET_GCC=gcc
RM=rm -f

PLUGDIR:=$(shell $(TARGET_GCC) -print-file-name=plugin)

CXXFLAGS+= -I$(PLUGDIR)/include -fPIC -fno-rtti -O2

# extension is depended of target OS
STRHASH=strhash.so
TEST=test


$(STRHASH): strhash.cc hashfns.c gcc-log-utils.c
	$(HOST_GCC) $(CXXFLAGS) -shared $^ -o $@


$(TEST): test.c hashfns.c $(STRHASH)
	$(TARGET_GCC) -fplugin=$(shell pwd)/$(STRHASH) $(filter-out $(STRHASH),$^) -o $@


all: strhash.so test


clean:
	$(RM) $(STRHASH)
	$(RM) $(TEST)


dumpinfo:
	$(info PLUGDIR:  $(PLUGDIR))
	$(info CXXFLAGS: $(CXXFLAGS))


.PHONY: all clean dumpinfo

.DEFAULT_GOAL:= all

# vim: ts=4:noet
