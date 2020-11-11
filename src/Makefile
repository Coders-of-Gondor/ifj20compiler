# Makefile
# Ondřej Míchal <xmicha80>
# FIT BUT
# 24/09/2020

ifeq (0, ${MAKELEVEL})
$(error invoke "make" from the root build directory)
endif

public_sources = \
	global.c \
	main.c \
	error.c \
	scanner.c \
	symtable.c \
	str.c \
	token.c

public_headers = \
	global.h \
	token.h \
	symtable.h \
	error.h \
	scanner.h \
	str.h

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
