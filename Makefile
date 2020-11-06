# Makefile
# Ondřej Míchal <xmicha80>, Marek Filip <xfilip46>
# FIT BUT
# 24/09/2020

export SHELL := /usr/bin/env sh
export CC = /usr/bin/env gcc
export CFLAGS = -std=c99 -g -Wall -Wextra
export NAME = ifj20compiler

export PROJECT_DIR := $(shell pwd)
export BUILD_DIR := $(PROJECT_DIR)/build

export BIN_DIR := $(BUILD_DIR)/bin
export OBJECTS_DIR := $(BUILD_DIR)/objects
export TARGET:= $(BIN_DIR)/$(NAME)

.PHONY: all run test zip clean help

all:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR) $(OBJECTS_DIR)
	$(MAKE) -C src

run:
	@$(TARGET)

test: all
	$(MAKE) -C tests
	@./build/bin/ifj20compiler-tests

zip:
	@zip xcoders69.zip Makefile src/*.c src/*.h src/Makefile

clean:
	@rm -f *.zip
	@rm -f -r $(BUILD_DIR)

help:
	@echo "make"
	@echo "    Execute 'make all' command."
	@echo "make all"
	@echo "    Build the project."
	@echo "make run"
	@echo "    Run the program."
	@echo "make test"
	@echo "    Build and test the project."
	@echo "make zip"
	@echo "    Create a zip file with the project."
	@echo "make clean"
	@echo "    Remove all unneeded files."
	@echo "make help"
	@echo "    Show this screen."
