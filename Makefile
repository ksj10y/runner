CC ?= gcc
CFLAGS ?= -O2 -Wall -Wextra -pedantic

.PHONY: all test clean

all: bin/local-record-maker

bin/local-record-maker: src/local_record_maker.c
	mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $<

test: bin/local-record-maker
	python3 -m unittest tests.test_cli -v

clean:
	rm -rf bin records.jsonl
