# $Id $

# Makefile for Rom24. Works fine on my Debian system.
# You may need to use 'gmake' on BSD systems.

MAKEFLAGS += -j2 --no-print-directory
ifneq ($(REAL_MAKE),1)
# run_make will be called once (it's .PHONY target), 
# even if make is called with several targets
%: run_make
	@:

.PHONY: run_make
run_make:
	@$(MAKE) $(MAKECMDGOALS) REAL_MAKE=1; if [ $$? -ne 0 ]; then exit 1; fi

else  # REAL_MAKE defined (actual makefile)
CC = gcc
RM = rm
EXE = rom

PROF = -O0 -g -w -Wall -lcrypt
# add -Wall back if you want warnings. I do not.
C_FLAGS = $(PROF)
# -Wno-char-subscripts
L_FLAGS = $(PROF)

#IMC2 - Comment out to disable IMC2 support
#IMC = 1

#ifdef IMC
#   C_FLAGS := $(C_FLAGS) -DIMC -DIMCROM
#endif

# Source Files
SRC_FILES := $(wildcard *.c)

# Object Files
OBJ_DIR = obj
OBJ_FILES := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

.ONESHELL:
rom: $(OBJ_FILES)
	$(RM) -f $(EXE)
	$(CC) -o $(EXE) $(OBJ_FILES) $(L_FLAGS)

$(OBJ_DIR)/%.o: %.c
	$(CC) $(C_FLAGS) -c -o $@ $<

clean:
	$(RM) -f $(OBJ_FILES) $(EXE) *~ *.bak *.orig *.rej

new: clean
	@$(MAKE)

distro:
	make clean
	cd ../..
	$(RM) -f log/*.log
	tar zvcf quickmud-`date -I`.tar.gz MUD
endif  # # REAL_MAKE defined (actual makefile)
