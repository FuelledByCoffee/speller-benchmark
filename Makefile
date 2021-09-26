CFLAGS += -WCL4 -Wshadow

all: debug
	ln -fs benchmark/bench ../bench

run: release
	ln -fs $${PWD##*/}/bench ../bench

release: CFLAGS += -O3 -DNDEBUG
release: bench

debug: CFLAGS += -g -fsanitize=undefined,thread
debug: bench

test: run
	bench

clean:
	rm -rf bench
	rm -rf *.dSYM

.PHONY: all run release debug clean
