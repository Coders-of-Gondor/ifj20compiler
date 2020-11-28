# Makefile
# Ondřej Míchal <xmicha80>
# Implementace překladače imperativního jazyka IFJ20.
# 26/11/2020

export CC = /usr/bin/env gcc
export CFLAGS = -std=c99 -g -Wall -Wextra

ifeq (0, ${MAKELEVEL})
export SHELL := /usr/bin/env sh
export NAME = ifj20compiler

export TARGET := $(NAME)
export OBJECTS_DIR := $(shell pwd)
CFLAGS += -DNDEBUG
endif

public_sources = \
	debug.c \
	error.c \
	global.c \
	main.c \
	parser.c \
	scanner.c \
	stack_charptr.c \
	stack_int.c \
	stack_stack_charptr_tptr.c \
	str.c \
	symtable.c \
	token.c

public_headers = \
	debug.h \
	error.h \
	global.h \
	paser.h \
	scanner.h \
	stack.h \
	stack_extern_template.h \
	stack_charptr.h \
	stack_int.h \
	stack_stack_charptr_tptr.h \
	str.h \
	symtable.h \
	token.h

private_sources = \
	scanner-private.c

private_headers = \
	scanner-private.h \
	symtable-private.h

compiler_sources = \
	$(public_sources) \
	$(private_sources)

compiler_headers = \
	$(public_headers) \
	$(private_headers)

compiler_objects = $(compiler_sources:%.c=$(OBJECTS_DIR)/%.o)

.PHONY: all

all: $(TARGET)
$(TARGET): $(compiler_objects)
	$(CC) -o $(TARGET) $^ $(LDFLAGS)
$(compiler_objects): $(OBJECTS_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	# remove the bin and object files
	rm -f $(NAME) *.o
