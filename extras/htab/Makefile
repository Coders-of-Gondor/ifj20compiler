# Makefile
# Ondřej Míchal <xmicha80>
# FIT BUT
# 13/10/2020

ifeq (0, ${MAKELEVEL})
$(error invoke "make" from the root build directory)
endif

target := $(LIB_DIR)/libhtab.so

htab_sources = $(wildcard *.c)
htab_objects = $(htab_sources:%.c=objects/%.o)

.PHONY: all includes dirs clean

all: dirs includes $(target)
$(target): $(htab_objects)
	$(CC) $(LDFLAGS) -shared -fPIC -o $(target) $^
$(htab_objects): objects/%.o: %.c
	$(CC) $(CFLAGS) -c -fPIC $< -o $@
includes: *.h
	@cp $? $(INCLUDE_DIR)
dirs:
	@mkdir -p ./objects

clean:
	@rm -f -r ./objects
