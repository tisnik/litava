#include <stdio.h>

#include "hercule.h"


void draw_area(Pixmap *pixmap, int x, int y, int width, int height)
{
    pixmap_rectangle(pixmap, x-1, y-1, x+width+1, y+height+1, 0, 0, 0);
    pixmap_rectangle(pixmap, x, y, x+width, y+height, 255, 255, 0);
    pixmap_rectangle(pixmap, x+1, y+1, x+width-1, y+height-1, 0, 0, 0);
}

float *** float_3d_array(int height, int width, int depth)
{
    int i, j;
    float ***array = (float***)malloc(height * sizeof(float**));

    for (i=0; i<height; i++) {
        array[i] = (float**)malloc(width * sizeof(float*));
        for (j=0; j<width; j++) {
            array[i][j] = (float*)malloc(depth * sizeof(float));
        }
    }
    return array;
}

void free_float_3d_array(float ***array, int height, int width)
{
    int i, j;

    for (i=0; i<height; i++) {
        for (j=0; j<width; j++) {
            free(array[i][j]);
        }
        free(array[i]);
    }
    free(array);
}

/*
 * Perform conversion from Pixmap into three dimensional float
 * arrays. It's much faster to work with float arrays on current CPUs even
 * if it's memory inefficient. Pixels in the float array are stored with
 * offset=average_value. This means, that value of color components might
 * be negative. 
 * 
 * @param pixmap
 *            image to convert
 * @param perform_averaging
 *            whether to offset pixel values by the image average value
 */
float *** image_to_float_array(Pixmap *pixmap, int perform_averaging)
{
    float ***array = float_3d_array(pixmap->height, pixmap->width, 3);

    int pixels = pixmap->width * pixmap->height;

    float red_average = 0;
    float green_average = 0;
    float blue_average = 0;

    int x, y;

    if (perform_averaging) {
        for (y = 0; y < pixmap->height; y++)
        {
            for (x = 0; x < pixmap->width; x++)
            {
                unsigned char red, green, blue;
                pixmap_getpixel(pixmap, x, y, &red, &green, &blue);

                red_average += (float)red;
                green_average += (float)green;
                blue_average += (float)blue;
            }
        }
        red_average /= pixels;
        green_average /= pixels;
        blue_average /= pixels;
    }

    for (y = 0; y < pixmap->height; y++)
    {
        for (x = 0; x < pixmap->width; x++)
        {
            unsigned char red, green, blue;
            pixmap_getpixel(pixmap, x, y, &red, &green, &blue);

            array[y][x][0] = (float)red - red_average;
            array[y][x][1] = (float)green - green_average;
            array[y][x][2] = (float)blue - blue_average;
        }
    }
    return array;
}

/*
 * Compute correlation for given two images and a 2D offset.
 * 
 * @param max_x
 * @param max_y
 * @param test_image_array
 *            first input array
 * @param pattern_image_array
 *            second input array
 * @param xoffset
 *            horizontal part of an offset
 * @param yoffset
 *            vertical part of an offset
 * @return correlation of two arrays with the given offset
 */
float compute_correlation(int max_x, int max_y,
                float ***test_image_array,
                float ***pattern_image_array,
                int xoffset, int yoffset)
{
    float correlation = 0;
    int x, y, rgb_index;

    for (y = 0; y < max_y; y++)
    {
        for (x = 0; x < max_x; x++)
        {
            for (rgb_index = 0; rgb_index < 3; rgb_index++)
            {
                float color_component1 = pattern_image_array[y][x][rgb_index];
                float color_component2 = test_image_array[yoffset + y][xoffset + x][rgb_index];
                correlation += color_component1 * color_component2;
            }
        }
    }
    return correlation;
}

#define CORRELATION_THRESHOLD 0.9

/*
 * Tries to find given pattern in an input image.
 * 
 * @param src
 *            input image
 * @param pattern
 *            given pattern stored represented as Pixmap
 * @return new image with highlightet pattern found
 */
Pixmap* find_pattern(Pixmap *src, Pixmap *pattern, int *status)
{
    Pixmap *dst = pixmap_clone(src);

    /* it is much faster to work directly with color components stored as float values */
    float ***src_f = image_to_float_array(src, 0);
    float ***pattern_f = image_to_float_array(pattern, 0);

    int max_x = src->width - pattern->width;
    int max_y = src->height - pattern->height;

    /* autocorellation value is needed to compute and compare correlation values */
    /* and to find the exact pattern (if it exists in the input image) */
    float autocorrelation = compute_correlation(pattern->width, pattern->height, pattern_f, pattern_f, 0, 0);

    int best_x = -1;
    int best_y = -1;
    float best_correlation = -1e10;
    int found_exact = 0;
    int xoffset, yoffset;

    for (yoffset = 0; (yoffset < max_y) && !found_exact; yoffset++ )
    {
        for (xoffset = 0; (xoffset < max_x) && !found_exact; xoffset++)
        {
            float correlation = compute_correlation(pattern->width, pattern->height, src_f, pattern_f, xoffset, yoffset);
            if (correlation >= best_correlation)
            {
                best_correlation = correlation;
                best_x = xoffset;
                best_y = yoffset;
                if (correlation == autocorrelation)
                {
                    found_exact = 1;
                }
            }
        }
    }

    free_float_3d_array(src_f, src->height, src->width);
    free_float_3d_array(pattern_f, pattern->height, pattern->width);

    printf("Autocorrelation:  %f\n", autocorrelation);
    printf("Best correlation: %f\n", best_correlation);
    printf("x: %d\n", best_x);
    printf("y: %d\n", src->height - pattern->height - best_y);
    printf("w: %d\n", pattern->width);
    printf("h: %d\n", pattern->height);
    printf("p: %3.0f\n", 100.0 * best_correlation/autocorrelation);

    if (found_exact) {
        printf("Result: exact match\n");
        *status = MATCHER_EXACT_MATCH;
    }
    else if (best_correlation / autocorrelation < CORRELATION_THRESHOLD)
    {
        *status = MATCHER_NOT_FOUND;
        printf("Result: not found\n");
    }
    else {
        *status = MATCHER_FOUND;
        printf("Result: found\n");
    }

    draw_area(dst, best_x, best_y, pattern->width, pattern->height);

    return dst;
}

