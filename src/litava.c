#include <stdio.h>
#include <assert.h>

#include "pixmap.h"
#include "bmp_reader.h"
#include "bmp_writer.h"
#include "hercule.h"

int main(int argc, char **argv) {
    Pixmap *src = NULL;
    Pixmap *pattern1 = NULL;
    Pixmap *dest1 = NULL;
    int status = -1;

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
