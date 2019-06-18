#include <stdio.h>
#include <stdlib.h>

#include "bmp_writer.h"

unsigned char bmp_header[] =
{
    0x42, 0x4d,             /* magic number */
    0x46, 0x00, 0x00, 0x00, /* size of BMP file */
    0x00, 0x00,             /* unused */
    0x00, 0x00,             /* unused */
    0x36, 0x00, 0x00, 0x00, /* 54 bytes - offset to data */
    0x28, 0x00, 0x00, 0x00, /* 40 bytes - bytes in DIB header */
    0x00, 0x00, 0x00, 0x00, /* width of bitmap */
    0x00, 0x00, 0x00, 0x00, /* height of bitmap */
    0x01, 0x00,             /* 1 pixel plane */
    0x00, 0x00,             /* 24 bpp */
    0x00, 0x00, 0x00, 0x00, /* no compression */
    0x00, 0x00, 0x00, 0x00, /* size of pixel array */
    0x13, 0x0b, 0x00, 0x00, /* 2835 pixels/meter */
    0x13, 0x0b, 0x00, 0x00, /* 2835 pixels/meter */
    0x00, 0x00, 0x00, 0x00, /* color palette */
    0x00, 0x00, 0x00, 0x00, /* important colors */
};

void write4bytes(unsigned char *array, int offset, int value)
{
    array[offset] =  value & 0xff;
    array[offset+1] = (value >> 8) & 0xff;
    array[offset+2] = (value >> 16) & 0xff;
    array[offset+3] = (value >> 24) & 0xff;
}

int bmp_write_to_stream(const Pixmap *pixmap, FILE *fout)
{
    int width, height, bpp;
    int pixel_array_size = pixmap_size(pixmap);
    int bmp_size = pixel_array_size + sizeof(bmp_header);
    int scanline;

    width = pixmap -> width;
    height = pixmap -> height;
    bpp = pixmap -> bpp * 8;

    /* size of BMP header */
    write4bytes(bmp_header, 2, bmp_size);

    /* width */
    write4bytes(bmp_header, 18, width);

    /* height */
    write4bytes(bmp_header, 22, height);

    /* bpp */
    bmp_header[28] = bpp;

    /* size of pixel array */
    write4bytes(bmp_header, 34, pixel_array_size);

    fwrite(bmp_header, sizeof(bmp_header), 1, fout);

    scanline = width * pixmap->bpp;
    printf("%d %d %d\n", scanline, width, bpp);

    /* align ok! */
    if (scanline % 4 == 0) {
        fwrite(pixmap->pixels, pixmap_size(pixmap), 1, fout);
    }
    else {
        int y;
        int offset = 0;
        char padding_array[4];
        int padding = (4 - scanline % 4) & 0x03;

        for (y=0; y < height; y++) {
            fwrite(pixmap->pixels + offset, scanline, 1, fout);
            offset += scanline;
            fwrite(padding_array, padding, 1, fout);
        }
    }
    return 1;
}

int bmp_write(const Pixmap *pixmap, const char *file_name)
{
    FILE *fout;

    fout = fopen(file_name, "wb");
    if (!fout)
    {
        return -1;
    }

    bmp_write_to_stream(pixmap, fout);

    if (fclose(fout) == EOF)
    {
        return -1;
    }
    return 0;
}

