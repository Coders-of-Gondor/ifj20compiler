# Makefile
# Ondřej Míchal <xmicha80>, Marek Filip <xfilip46>
# FIT BUT
# 24/09/2020

export SHELL := /usr/bin/env sh
export CC = /usr/bin/env gcc
export CFLAGS = -std=c99 -g -pedantic -Wall -Wextra
export NAME = ifj20compiler

export SOURCE_DIR := $(shell pwd)/src
export BUILD_DIR := $(shell pwd)/build

.PHONY: all install run zip clean help

all:
	@mkdir -p $(BUILD_DIR)
	$(MAKE) -C $(SOURCE_DIR)

run:
	src/$(NAME)

zip:
	@zip xcoders69.zip Makefile src/*.c src/*.h src/Makefile

clean:
	@rm -f *.zip
	@rm -f -r $(BUILD_DIR)

help:
	@echo "make"
	@echo "    Executes 'make all' command"
	@echo "make all"
	@echo "    Builds the project"
	@echo "make run"
	@echo "    Runs the program"
	@echo "make zip"
	@echo "    Create a zip file with the project"
	@echo "make clean"
	@echo "    Remove all unneeded files"
	@echo "make help"
	@echo "    Show this screen"
