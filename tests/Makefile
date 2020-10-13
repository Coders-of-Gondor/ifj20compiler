# Makefile
# Vojtěch Fiala <xfiala61>, Ondřej Míchal <xmicha80>
# FIT BUT
# 15/10/2020

ifeq (0, ${MAKELEVEL})
$(error invoke "make tests" from the root build directory)
endif

TARGET := $(TARGET)-tests
CXX = g++
CXXFLAGS = -g -std=c++17 -fpermissive
LDFLAGS = -lgtest -pthread

tests_sources = \
	tests.cpp \
	parser_tests.cpp \
	scanner_tests.cpp \
	symtable_tests.cpp \
	token_tests.cpp

tested_sources = \
	scanner.c \
	scanner-private.c

tests_objects = $(tests_sources:%.cpp=$(OBJECTS_DIR)/%.o)
tested_objects = $(tested_sources:%.c=$(OBJECTS_DIR)/%.o)

.PHONY: all

all: $(TARGET)
$(TARGET): $(tests_objects) $(tested_objects)
	$(CXX) -o $(TARGET) $^ $(LDFLAGS)
$(tests_objects): $(OBJECTS_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
