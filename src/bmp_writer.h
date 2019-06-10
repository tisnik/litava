#ifndef _BMP_WRITER_H_
#define _BMP_WRITER_H_

#include <stdio.h>
#include "pixmap.h"

int bmp_write(const Pixmap *pixmap, const char *file_name);
int bmp_write_to_stream(const Pixmap *pixmap, FILE *fout);

#endif
