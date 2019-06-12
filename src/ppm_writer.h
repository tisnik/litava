#ifndef _PPM_WRITER_H_
#define _PPM_WRITER_H_

#include <stdio.h>
#include "pixmap.h"

int ppm_write(const Pixmap * pixmap, const char *file_name);
void ppm_write_to_stream(const Pixmap * pixmap, FILE * fout);

#endif
