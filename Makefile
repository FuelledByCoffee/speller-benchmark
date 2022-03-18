CFLAGS += -WCL4 -Wshadow
LDLIBS += -lpthread

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

.PHONY: all install release debug clean
