# Makefile
# Ondřej Míchal <xmicha80>
# FIT BUT
# 24/09/2020

ifeq (0, ${MAKELEVEL})
$(error invoke "make" from the root build directory)
endif

public_sources = \
	main.c

public_headers = \
	token.h

private_sources =

private_headers =

compiler_sources = \
	$(public_sources) \
	$(private_sources)

compiler_headers = \
	$(public_headers) \
	$(private_headers)

compiler_objects = $(BUILD_DIR)/$(compiler_sources:.c=.o)

.PHONY: all clean

all: $(compiler_objects)
	$(CC) $(LDFLAGS) $(compiler_objects) -o $(BUILD_DIR)/$(NAME)
$(BUILD_DIR)/%.o: $(compiler_sources)
	$(CC) -c $(CFLAGS) -o $@ $<
$(compiler_objects): $(compiler_headers)
