# Makefile
# Ondřej Míchal <xmicha80>
# FIT BUT
# 24/09/2020

public_sources = \
	main.c \
	scanner.c

public_headers = \
	scanner.h \
	token.h

private_sources =

private_headers = \
	scanner-private.h \

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
