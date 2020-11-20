# Makefile
# Ondřej Míchal <xmicha80>
# Implementace překladače imperativního jazyka IFJ20.
# 24/09/2020

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
	main.c \
	global.c \
	error.c \
	token.c \
	symtable.c \
	str.c \
	scanner.c \
	parser.c

public_headers = \
	debug.h \
	global.h \
	error.h \
	token.h \
	symtable.h \
	str.h \
	scanner.h \
	paser.h

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
