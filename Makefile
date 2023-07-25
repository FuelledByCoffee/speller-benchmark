ifeq ($(CC),clang)
	CPPFLAGS += -MJ ../$@.json
endif
CFLAGS += -Wall -Wextra -Wshadow
LDLIBS += -lpthread -lm

all: debug

install: release
	ln -sf speller-benchmark/bench ../bench

release: CFLAGS += -O3 -DNDEBUG
release: bench

debug: CFLAGS += -g -fsanitize=undefined,thread
debug: bench

test: install
	cd ..; ./bench

clean:
	rm -rf bench
	rm -rf *.dSYM
	rm -rf ../bench.json

.PHONY: all install release debug clean
