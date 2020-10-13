# Makefile
# Ondřej Míchal <xmicha80>, Marek Filip <xfilip46>
# FIT BUT
# 24/09/2020

export SHELL := /usr/bin/env sh
export CC = /usr/bin/env gcc
export CFLAGS = -std=c99 -g -pedantic -Wall -Wextra
export LDFLAGS = -I $(INCLUDE_DIR) -L $(LIB_DIR)
export LDLIBS = -lhtab
export NAME = ifj20compiler

export PROJECT_DIR := $(shell pwd)
export BUILD_DIR := $(PROJECT_DIR)/build

export BIN_DIR := $(BUILD_DIR)/bin
export INCLUDE_DIR := $(BUILD_DIR)/include
export LIB_DIR := $(BUILD_DIR)/lib

.PHONY: all run zip clean help

all:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR) $(INCLUDE_DIR) $(LIB_DIR)
	$(MAKE) -C extras/htab
	$(MAKE) -C src

run:
	src/$(NAME)

zip:
	@zip xcoders69.zip Makefile src/*.c src/*.h src/Makefile

clean:
	@rm -f *.zip
	@rm -f -r $(BUILD_DIR)
	@$(MAKE) -s -C extras/htab clean
	@$(MAKE) -s -C src clean

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
