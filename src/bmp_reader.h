#ifndef _BMP_READER_H_
#define _BMP_READER_H_

#include <stdio.h>
#include "pixmap.h"

Pixmap *bmp_read(const char *file_name);
Pixmap *bmp_read_from_stream(const char *file_name, FILE * fin);

#endif
