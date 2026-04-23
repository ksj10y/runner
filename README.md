# local-record-maker

`local-record-maker` is a small compiled CLI that generates synthetic local records and writes them to a JSONL file.

It does not use networking, persistence hooks, background execution, credential access, or system modification. It only writes to the output path you pass.

## Usage

```bash
./bin/local-record-maker -n 5 -o records.jsonl --seed demo
```

Options:

- `-n <count>`: number of records to write. Default: `10`.
- `-o <path>`: output JSONL path. Default: `records.jsonl`.
- `--seed <text>`: deterministic seed text. Default: `local-record-maker`.

## Build

```bash
gcc -O2 -Wall -Wextra -pedantic -o bin/local-record-maker src/local_record_maker.c
```

The checked-in binary under `bin/local-record-maker` is built for Linux x86_64.
