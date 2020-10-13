# Makefile
# Ondřej Míchal <xmicha80>
# FIT BUT
# 24/09/2020

ifeq (0, ${MAKELEVEL})
$(error invoke "make" from the root build directory)
endif

target := $(BIN_DIR)/$(NAME)

public_sources = \
	main.c \
	scanner.c

public_headers = \
	scanner.h \
	token.h

private_sources = \
	scanner-private.c

private_headers = \
	scanner-private.h \

compiler_sources = \
	$(public_sources) \
	$(private_sources)

compiler_headers = \
	$(public_headers) \
	$(private_headers)

compiler_objects = $(compiler_sources:%.c=objects/%.o)

.PHONY: all dirs clean

all: dirs $(target)
$(target): $(compiler_objects)
	$(CC) -o $(target) $^ $(LDFLAGS)
$(compiler_objects): objects/%.o: $(compiler_sources)
	$(CC) $(CFLAGS) -c $< -o $@
dirs:
	@mkdir -p ./objects

clean:
	@rm -f -r ./objects
