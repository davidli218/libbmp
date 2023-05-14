#include <stdio.h>
#include <libbmp.h>

static Bitmap *gr_checkerboard(int32_t size, uint16_t color_depth) {
    int32_t unit_size = 32;

    Bitmap *img = bmp_new(size, size, color_depth);

    bmp_fill(img, 0x00, 0xFF, 0x00);

    for (int32_t i = 0; i < size; i++)
        for (int32_t j = 0; j < size; j++)
            if ((i / unit_size) % 2 == (j / unit_size) % 2)
                bmp_set_pixel(img, i, j, 0xFF, 0x00, 0x00);

    return img;
}

int main() {
    Bitmap *patten;

    printf("gr_checkerboard(64, 24)\n");
    patten = gr_checkerboard(64, 24);
    bmp_print_header(patten);
    bmp_write_file(patten, "gr_checkerboard_64_24.bmp");
    bmp_free(patten);

    printf("\ngr_checkerboard(256, 32)\n");
    patten = gr_checkerboard(256, 32);
    bmp_print_header(patten);
    bmp_write_file(patten, "gr_checkerboard_256_32.bmp");
    bmp_free(patten);

    return 0;
}
