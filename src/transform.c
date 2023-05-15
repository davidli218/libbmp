#include "libbmp.h"

BmpError bmp_flip(Bitmap *bmp, FlipDirection direction) {
    if (bmp == NULL) return BMP_INVALID_ARGUMENT;

    if (direction == BMP_FLIP_HORIZONTAL) {
        for (int32_t i = 0; i < bmp->info_header.bi_height; i++)
            for (int32_t j = 0; j < bmp->info_header.bi_width / 2; j++) {
                uint8_t r1, g1, b1, r2, g2, b2;
                bmp_get_pixel(bmp, j, i, &r1, &g1, &b1);
                bmp_get_pixel(bmp, bmp->info_header.bi_width - j - 1, i, &r2, &g2, &b2);
                bmp_set_pixel(bmp, j, i, r2, g2, b2);
                bmp_set_pixel(bmp, bmp->info_header.bi_width - j - 1, i, r1, g1, b1);
            }
    } else if (direction == BMP_FLIP_VERTICAL) {
        for (int32_t i = 0; i < bmp->info_header.bi_height / 2; i++)
            for (int32_t j = 0; j < bmp->info_header.bi_width; j++) {
                uint8_t r1, g1, b1, r2, g2, b2;
                bmp_get_pixel(bmp, j, i, &r1, &g1, &b1);
                bmp_get_pixel(bmp, j, bmp->info_header.bi_height - i - 1, &r2, &g2, &b2);
                bmp_set_pixel(bmp, j, i, r2, g2, b2);
                bmp_set_pixel(bmp, j, bmp->info_header.bi_height - i - 1, r1, g1, b1);
            }
    } else {
        return BMP_INVALID_ARGUMENT;
    }

    return BMP_OK;
}
