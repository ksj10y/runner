#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(const char *name) {
    fprintf(stderr, "usage: %s [-n count] [-o output.jsonl] [--seed text]\n", name);
}

static uint64_t fnv1a(const char *text) {
    uint64_t h = 1469598103934665603ULL;
    while (*text) {
        h ^= (unsigned char)*text++;
        h *= 1099511628211ULL;
    }
    return h;
}

static uint64_t next_u64(uint64_t *state) {
    uint64_t x = *state;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    *state = x;
    return x;
}

static int parse_count(const char *text, long *out) {
    char *end = NULL;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0') {
        return -1;
    }
    if (value < 0 || value > 1000000L) {
        return -1;
    }
    *out = value;
    return 0;
}

int main(int argc, char **argv) {
    long count = 10;
    const char *output_path = "records.jsonl";
    const char *seed_text = "local-record-maker";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0) {
            if (++i >= argc || parse_count(argv[i], &count) != 0) {
                fprintf(stderr, "count must be an integer from 0 to 1000000\n");
                return 2;
            }
        } else if (strcmp(argv[i], "-o") == 0) {
            if (++i >= argc || argv[i][0] == '\0') {
                fprintf(stderr, "output path is required after -o\n");
                return 2;
            }
            output_path = argv[i];
        } else if (strcmp(argv[i], "--seed") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "seed text is required after --seed\n");
                return 2;
            }
            seed_text = argv[i];
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "unknown argument: %s\n", argv[i]);
            usage(argv[0]);
            return 2;
        }
    }

    FILE *out = fopen(output_path, "w");
    if (!out) {
        perror(output_path);
        return 1;
    }

    uint64_t state = fnv1a(seed_text);
    if (state == 0) {
        state = 0xA5A5A5A5A5A5A5A5ULL;
    }

    for (long i = 1; i <= count; i++) {
        uint64_t a = next_u64(&state);
        uint64_t b = next_u64(&state);
        unsigned value = (unsigned)(a % 100000U);
        fprintf(
            out,
            "{\"id\":%ld,\"source\":\"synthetic\",\"label\":\"record-%04ld\","
            "\"value\":%u,\"note\":\"%016llx%016llx\"}\n",
            i,
            i,
            value,
            (unsigned long long)a,
            (unsigned long long)b
        );
    }

    if (fclose(out) != 0) {
        perror(output_path);
        return 1;
    }

    printf("wrote %ld records to %s\n", count, output_path);
    return 0;
}
