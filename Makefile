# Makefile
# Ondřej Míchal <xmicha80>, Marek Filip <xfilip46>
# FIT BUT
# 24/09/2020

export SHELL := /usr/bin/env sh
export NAME = ifj20compiler
export ZIPNAME = xfilip46.zip

export PROJECT_DIR := $(shell pwd)
export BUILD_DIR := $(PROJECT_DIR)/build

export BIN_DIR := $(BUILD_DIR)/bin
export OBJECTS_DIR := $(BUILD_DIR)/objects
export TARGET:= $(BIN_DIR)/$(NAME)

.PHONY: all run testunit test2e2 docs zip clean help

all:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR) $(OBJECTS_DIR)
	$(MAKE) -C src

run:
	@$(TARGET)

testunit: all
	$(MAKE) -C tests/unit
	@./build/bin/ifj20compiler-tests

teste2e: all
	@bats ./tests/e2e

docs:
	@cd docs; doxygen ./Doxyfile

zip:
	@mkdir -p $(BUILD_DIR)
	@cd src; zip $(ZIPNAME) ./*.c ./*.h ./Makefile; mv $(ZIPNAME) ../

clean:
	@rm -f $(ZIPNAME)
	@rm -f -r $(BUILD_DIR)
	@rm -f src/$(NAME) src/*.o
	@rm -f docs/html docs/latex

help:
	@echo "make"
	@echo "    Execute 'make all' command."
	@echo "make all"
	@echo "    Build the project."
	@echo "make run"
	@echo "    Run the program."
	@echo "make test"
	@echo "    Build and test the project."
	@echo "make docs"
	@echo "    Generate documentation using doxygen"
	@echo "make zip"
	@echo "    Create a zip file with the project."
	@echo "make clean"
	@echo "    Remove all unneeded files."
	@echo "make help"
	@echo "    Show this screen."
