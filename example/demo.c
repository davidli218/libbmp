#include <stdio.h>
#include <stdlib.h>
#include <libbmp.h>

static Bitmap *gr_checkerboard(int32_t size, uint16_t color_depth) {
    int32_t unit_size = 32;

    Bitmap *bmp = (Bitmap *) malloc(sizeof(Bitmap));
    bmp_new(bmp, size, size, color_depth);

    bmp_fill(bmp, 0x00, 0xFF, 0x00);

    for (int32_t i = 0; i < size; i++)
        for (int32_t j = 0; j < size; j++)
            if ((i / unit_size) % 2 == (j / unit_size) % 2)
                bmp_set_pixel(bmp, i, j, 0xFF, 0x00, 0x00);

    return bmp;
}

static Bitmap *gradient_patten(int32_t size, uint16_t color_depth) {
    Bitmap *bmp = (Bitmap *) malloc(sizeof(Bitmap));
    bmp_new(bmp, size, size, color_depth);

    double step = 256.0 / size;

    for (int32_t i = 0; i < size; i++)
        for (int32_t j = 0; j < size; j++) {
            uint8_t r = (uint8_t) (j * step),
                    g = (uint8_t) ((i + j) * step / 2),
                    b = (uint8_t) (i * step);
            bmp_set_pixel(bmp, i, j, r, g, b);
        }

    return bmp;
}

int main() {
    Bitmap *patten;

    int32_t size = 256;
    uint16_t color_depth = 24;

    printf("\n1. gr_checkerboard(%d, %d)\n", size, color_depth);
    patten = gr_checkerboard(size, color_depth);
    bmp_print_header(patten);
    bmp_write_file(patten, "gr_checkerboard.bmp");
    bmp_free(patten);

    printf("\n2. gradient_patten(%d, %d)\n", size, color_depth);
    patten = gradient_patten(size, color_depth);
    bmp_print_header(patten);
    bmp_write_file(patten, "gradient_patten.bmp");
    bmp_free(patten);

    return 0;
}
