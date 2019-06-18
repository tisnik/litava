#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "pixmap.h"
#include "bmp_reader.h"
#include "bmp_writer.h"
#include "hercule.h"

int flag_specified(const char *short_flag, const char *long_flag, const char *arg) {
    return strcmp(short_flag, arg) == 0 || strcmp(long_flag, arg) == 0;
}

int main(int argc, char **argv) {
    Pixmap *src = NULL;
    Pixmap *pattern1 = NULL;
    Pixmap *dest1 = NULL;
    int status = -1;

    if (argc == 2) {
        if (flag_specified("-v", "--version", argv[1])) {
            puts("Litava version 1.0");
            return 0;
        } else if (flag_specified("-h", "--help", argv[1])) {
            puts("Usage ./litava tested_image.bmp pattern_to_find.bmp output_image.bmp");
            return 0;
        }
    }
    if (argc < 4) {
        puts("Usage ./litava tested_image.bmp pattern_to_find.bmp output_image.bmp");
        return 1;
    }

    src = bmp_read(argv[1]);
    pattern1 = bmp_read(argv[2]);

    dest1 = find_pattern(src, pattern1, &status);
    printf("Status: %d\n", status);
    bmp_write(dest1, argv[3]);

    return status;
}
