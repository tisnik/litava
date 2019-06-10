#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "pixmap.h"

size_t pixmap_size(const Pixmap *pixmap)
{
    return pixmap->width * pixmap->height * pixmap->bpp;
}

Pixmap* pixmap_clone(const Pixmap *src)
{
    Pixmap *dest = (Pixmap*)malloc(sizeof(Pixmap));
    dest->width = src->width;
    dest->height = src->height;
    dest->bpp = src->bpp;
    dest->pixels = (unsigned char*)malloc(pixmap_size(src));
    memcpy(dest->pixels, src->pixels, pixmap_size(src));
    return dest;
}

Pixmap* pixmap_create(unsigned int width, unsigned int height, unsigned int bpp)
{
    Pixmap *pixmap = (Pixmap*)malloc(sizeof(Pixmap));
    pixmap->width = width;
    pixmap->height = height;
    pixmap->bpp = bpp;
    pixmap->pixels = (unsigned char*)malloc(pixmap_size(pixmap));
    return pixmap;
}

void pixmap_destroy(Pixmap *pixmap)
{
    free(pixmap->pixels);
    free(pixmap);
}

void pixmap_clear(Pixmap *pixmap)
{
    memset(pixmap->pixels, 0x00, pixmap_size(pixmap));
}

void pixmap_putpixel(Pixmap *pixmap, int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
    unsigned char *p;
    if (x<0 || y<0 || x>=(int)pixmap->width || y>=(int)pixmap->height) return;
    p=pixmap->pixels + (x + y * pixmap->width) * pixmap->bpp;
    *p++=b;
    *p++=g;
    *p=r;
}

void pixmap_getpixel(const Pixmap *pixmap, int x, int y, unsigned char *r, unsigned char *g, unsigned char *b)
{
    unsigned char *p;
    if (x<0 || y<0 || x>=(int)pixmap->width || y>=(int)pixmap->height) return;
    p=pixmap->pixels + (x + y * pixmap->width) * pixmap->bpp;
    *b=*p++;
    *g=*p++;
    *r=*p;
}

void pixmap_putpixel_max(Pixmap *pixmap, int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
    unsigned char *p;
    if (x<0 || y<0 || x>=(int)pixmap->width || y>=(int)pixmap->height) return;
    p=pixmap->pixels + (x + y * pixmap->width) * pixmap->bpp;
    if (*p<b) *p=b;
    p++;
    if (*p<g) *p=g;
    p++;
    if (*p<r) *p=r;
}

void pixmap_hline(Pixmap *pixmap, int x1, int x2, int y, unsigned char r, unsigned char g, unsigned char b)
{
    int x, fromX=MIN(x1, x2), toX=MAX(x1, x2);
    for (x=fromX; x<=toX; x++)
    {
        pixmap_putpixel(pixmap, x, y, r, g, b);
    }
}

void pixmap_vline(Pixmap *pixmap, int x, int y1, int y2, unsigned char r, unsigned char g, unsigned char b)
{
    int y, fromY=MIN(y1, y2), toY=MAX(y1, y2);
    for (y=fromY; y<=toY; y++)
    {
        pixmap_putpixel(pixmap, x, y, r, g, b);
    }
}

void pixmap_line(Pixmap *pixmap, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b)
{
    int dx=x2-x1;
    int dy=y2-y1;
    double s, p, e=0.0;
    int x, y, xdelta, ydelta, xpdelta, ypdelta, xp, yp;
    int i, imin, imax;

    if (x1==x2)
    {
        pixmap_vline(pixmap, x1, y1, y2, r, g, b);
    }

    if (y1==y2)
    {
        pixmap_hline(pixmap, x1, x2, y1, r, g, b);
    }
    if (x1>x2)
    {
        x1=x1^x2; x2=x1^x2; x1=x1^x2;
        y1=y1^y2; y2=y1^y2; y1=y1^y2;
    }

    if (abs(dx)>abs(dy))
    {
        s=(double)dy/(double)dx;
        imin=x1;  imax=x2;
        x=x1;     y=y1;
        xdelta=1; ydelta=0;
        xpdelta=0;
        xp=0;
        if (y2>y1)
        {
            ypdelta=1;
            yp=1;
        }
        else
        {
            s=-s;
            ypdelta=-1;
            yp=-1;
        }
    }
    else
    {
        s=(double)dx/(double)dy;
        xdelta=0; ydelta=1;
        ypdelta=0;
        yp=0;
        if (y2>y1)
        {
            imin=y1;    imax=y2;
            x=x1;       y=y1;
            xpdelta=1;
            xp=1;
        }
        else
        {
            s=-s;
            imin=y2;    imax=y1;
            x=x2;       y=y2;
            xpdelta=-1;
            xp=-1;
        }
    }
    p=s*256.0;
    for (i=imin; i<=imax; i++) {
        int c1=(int)e;
        int c2=255-c1;
        pixmap_putpixel_max(pixmap, x+xp, y+yp, c1, c1, c1);
        pixmap_putpixel_max(pixmap, x, y, c2, c2, c2);
        e=e+p;
        x+=xdelta;
        y+=ydelta;
        if (e>=256.0) {
            e-=256.0;
            x+=xpdelta;
            y+=ypdelta;
        }
    }
}

void pixmap_rectangle(Pixmap *pixmap, int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b)
{
    pixmap_hline(pixmap, x1, x2, y1, r, g, b);
    pixmap_hline(pixmap, x1, x2, y2, r, g, b);
    pixmap_vline(pixmap, x1, y1, y2, r, g, b);
    pixmap_vline(pixmap, x2, y1, y2, r, g, b);
}
