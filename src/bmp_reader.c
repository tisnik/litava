#include <stdio.h>
#include <stdlib.h>

#include "bmp_reader.h"
#include "pixmap.h"

int read4bytes(unsigned char *array, int offset)
{
    return array[offset] +
           (array[offset+1] << 8) +
           (array[offset+2] << 16) +
           (array[offset+3] << 24);
}

Pixmap* bmp_read_from_stream(const char *file_name, FILE *fin)
{
    Pixmap *pixmap = NULL;
    unsigned char bmp_header[54];
    int width, height, bpp;
    int scanline;
    size_t read;

    read = fread(bmp_header, sizeof(bmp_header), 1, fin);
    if (read < 1) {
        puts("Can not read BMP header");
        return NULL;
    }

    width = read4bytes(bmp_header, 18);
    height = read4bytes(bmp_header, 22);
    bpp = bmp_header[28] >> 3;

    pixmap = pixmap_create(width, height, bpp);

    scanline = width * bpp;

    /* align ok! */
    if (scanline % 4 == 0) {
        read = fread(pixmap->pixels, pixmap_size(pixmap), 1, fin);
        if (read < 1) {
            pixmap_destroy(pixmap);
            puts("Can not read pixels from BMP file");
            return NULL;
        }
    }
    else {
        int y;
        int offset = 0;
        char padding_array[4];
        int padding = (4 - scanline % 4) & 0x03;

        for (y=0; y < height; y++) {
            read = fread(pixmap->pixels + offset, scanline, 1, fin);
            if (read < 1) {
                pixmap_destroy(pixmap);
                puts("Can not read pixels from BMP file");
                return NULL;
            }
            offset += scanline;
            read = fread(padding_array, padding, 1, fin);
            if (read < 1) {
                pixmap_destroy(pixmap);
                puts("Can not read pixels from BMP file");
                return NULL;
            }
        }
    }

    return pixmap;
}

Pixmap* bmp_read(const char *file_name)
{
    FILE *fin;
    Pixmap *pixmap = NULL;

    fin = fopen(file_name, "rb");
    if (!fin)
    {
        return NULL;
    }

    pixmap = bmp_read_from_stream(file_name, fin);

    if (fclose(fin) == EOF)
    {
        return NULL;
    }

    return pixmap;
}

