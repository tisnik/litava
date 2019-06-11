#ifndef _HERCULE_H_
#define _HERCULE_H_

#include "pixmap.h"

#define MATCHER_EXACT_MATCH 0
#define MATCHER_FOUND 1
#define MATCHER_NOT_FOUND 2
#define MATCHER_ERROR 3

Pixmap *find_pattern(Pixmap * src, Pixmap * pattern, int *status);

#endif
