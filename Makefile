# $Id $

# Makefile for Rom24. Works fine on my Debian system.
# You may need to use 'gmake' on BSD systems.

CC = gcc
RM = rm
EXE = rom



PROF = -O0 -g -w -lcrypt
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
	sudo -u matthew $(RM) -f $(EXE)
	sudo -u matthew $(CC) -o $(EXE) $(OBJ_FILES) $(L_FLAGS)
	sudo -u matthew chmod u+s rom

$(OBJ_DIR)/%.o: %.c
	sudo -u matthew $(CC) $(C_FLAGS) -c -o $@ $<

clean:
	sudo -u matthew $(RM) -f $(OBJ_FILES) $(EXE) *~ *.bak *.orig *.rej

new:
	make clean
	make

distro:
	make clean
	cd ../..
	$(RM) -f log/*.log
	tar zvcf quickmud-`date -I`.tar.gz MUD
