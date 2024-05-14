CROSS_COMPILE ?= #aarch64-linux-gnu-

AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld
CC = $(CROSS_COMPILE)gcc
CX = $(CROSS_COMPILE)g++
CPP = $(CC) -E
AR = $(CROSS_COMPILE)ar
NM = $(CROSS_COMPILE)nm
STRIP = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

export AS LD CC CX CPP AR NM
export STRIP OBJCOPY OBJDUMP

topdir := $(shell pwd)
release_dir := $(topdir)/release
bin_dir=$(release_dir)/bin
lib_dir=$(release_dir)/lib
inc_dir=$(release_dir)/include
#doc_dir=$(release_dir)/doc
thirdparty_dir := $(topdir)/thirdparty

export topdir bin_dir lib_dir inc_dir #doc_dir

DIRS = src

all: create_dirs
	@for dir in $(DIRS) ; do \
		if test -d $$dir ; then \
			echo "$$dir: $(MAKE) $@" ; \
			if (cd $$dir; $(MAKE) $@; $(MAKE) install) ; then \
				true; \
			else \
				exit 1; \
			fi; \
		fi \
	done

prebuild: create_dirs
	@if test -d $(thirdparty_dir) ; then \
		pushd $(thirdparty_dir) ; \
		./prebuild.sh $(release_dir) ; \
		popd ; \
	fi

create_dirs:
	mkdir -p $(bin_dir) $(inc_dir) $(lib_dir)

clean:
	@for dir in $(DIRS) ; do \
		if test -d $$dir ; then \
			echo "$$dir: $(MAKE) $@" ; \
			if (cd $$dir; $(MAKE) $@) ; then \
				true; \
			else \
				exit 1; \
			fi; \
		fi \
	done

.PHONY: all clean
