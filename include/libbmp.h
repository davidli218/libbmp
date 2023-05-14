#ifndef LIBBMP_LIBRARY_H
#define LIBBMP_LIBRARY_H

#include <stdint.h>

typedef struct tagBitmapFileHeader {
    /* The header field used to identify the BMP & DIB file
     * is 0x42 0x4D in hexadecimal, same as BM in ASCII. */
    uint16_t bf_type;

    /* Size of the BMP file in bytes */
    uint32_t bf_size;

    /* Reserved should be 0 */
    uint16_t bf_reserved1;
    uint16_t bf_reserved2;

    /* Offset to the start of image data in bytes from the beginning of the file */
    uint32_t bf_off_bits;
} BitmapFileHeader;

typedef struct tagBitmapInfoHeader {
    /* Size of this header in bytes */
    uint32_t bi_size;

    /* Width of bitmap in pixels */
    int32_t bi_width;

    /* Height of bitmap in pixels */
    int32_t bi_height;

    /* Number of color planes being used. */
    uint16_t bi_planes;

    /* Number of bits per pixel */
    uint16_t bi_bit_count;

    /* Type of compression being used */
    uint32_t bi_compression;

    /* Size of the image. */
    uint32_t bi_size_image;

    /* Horizontal resolution of the image in pixels per meter */
    int32_t bi_x_pels_per_meter;

    /* Vertical resolution of the image in pixels per meter */
    int32_t bi_y_pels_per_meter;

    /* Number of color indexes in the color table actually used by the bitmap */
    uint32_t bi_clr_used;

    /* Number of color indexes that are important.
     * When this value is set to zero, all colors are important. */
    uint32_t bi_clr_important;
} BitmapInfoHeader;

typedef struct tagRgbQuad {
    /* Blue value of color */
    uint8_t rgb_blue;

    /* Green value of color */
    uint8_t rgb_green;

    /* Red value of color */
    uint8_t rgb_red;

    /* Reserved should be 0 */
    uint8_t rgb_reserved;
} RgbQuad;

typedef struct tagBitmap {
    /* File header */
    BitmapFileHeader file_header;

    /* Info header */
    BitmapInfoHeader info_header;

    /* Color table */
    RgbQuad *color_table;

    /* Image data */
    uint8_t *data;
} Bitmap;

typedef enum tagBmpError {
    BMP_OK,
    BMP_FILE_NOT_FOUND,
    BMP_FILE_INVALID,
    BMP_OUT_OF_MEMORY,
    BMP_OUT_OF_RANGE,
    BMP_INVALID_BIT_COUNT,
    BMP_NULL_PTR,
    UNKNOWN_ERROR,
} BmpError;

Bitmap *bmp_new(int32_t width, int32_t height, uint16_t bit_count);

BmpError bmp_free(Bitmap *bmp);

BmpError bmp_print_header(Bitmap *bmp);

BmpError bmp_read_file(Bitmap *bmp, const char *filename);

BmpError bmp_write_file(Bitmap *bmp, const char *filename);

BmpError bmp_set_pixel(Bitmap *bmp, int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b);

BmpError bmp_fill(Bitmap *bmp, uint8_t r, uint8_t g, uint8_t b);

#endif //LIBBMP_LIBRARY_H
