#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libbmp.h"

static void bmp_init(Bitmap *bmp) {
    bmp->file_header.bf_type = 0x4D42;
    bmp->file_header.bf_reserved1 = 0;
    bmp->file_header.bf_reserved2 = 0;
    bmp->file_header.bf_off_bits = 40 + 14;

    bmp->info_header.bi_size = 40;
    bmp->info_header.bi_planes = 1;
    bmp->info_header.bi_compression = 0;
    bmp->info_header.bi_x_pels_per_meter = 0;
    bmp->info_header.bi_y_pels_per_meter = 0;
    bmp->info_header.bi_clr_used = 0;
    bmp->info_header.bi_clr_important = 0;

    /*
     * These field will be set in bmp_create():
     *     + bmp->file_header.bf_size
     *     + bmp->info_header.bi_width
     *     + bmp->info_header.bi_height
     *     + bmp->info_header.bi_bit_count
     *     + bmp->info_header.bi_size_image
     */

    bmp->color_table = NULL;
    bmp->data = NULL;
}

BmpError bmp_new(Bitmap *bmp, int32_t width, int32_t height, uint16_t bit_count) {
    if (bit_count != 24 && bit_count != 32) { return BMP_INVALID_BIT_COUNT; }

    bmp_init(bmp);

    /* Set rest of the fields in headers */
    bmp->info_header.bi_width = width;
    bmp->info_header.bi_height = height;
    bmp->info_header.bi_bit_count = bit_count;
    bmp->info_header.bi_size_image = width * height * (bit_count / 8);
    bmp->file_header.bf_size = bmp->info_header.bi_size_image + bmp->file_header.bf_off_bits;

    /* Allocate memory for color table */
    bmp->color_table = NULL;

    /* Allocate memory for image data */
    bmp->data = (uint8_t *) malloc(bmp->info_header.bi_size_image);
    if (bmp->data == NULL) { return BMP_OUT_OF_MEMORY; }

    /* Initialize image data to pure white */
    memset(bmp->data, 0xFF, bmp->info_header.bi_size_image);

    return BMP_OK;
}

BmpError bmp_free(Bitmap *bmp) {
    if (bmp->color_table != NULL) { free(bmp->color_table); }
    if (bmp->data != NULL) { free(bmp->data); }

    return BMP_OK;
}

BmpError bmp_print_header(Bitmap *bmp) {
    BmpFileHeader *fH = &bmp->file_header;
    BmpInfoHeader *iH = &bmp->info_header;

    printf("+--------------------------------+\n");
    printf("|          Bitmap header         |\n");
    printf("+--------------------------------+\n");
    printf("| Type:        %16c%c |\n", fH->bf_type & 0xFF, fH->bf_type >> 8);
    printf("| File size:   %15d B |\n", fH->bf_size);
    printf("| Data offset: %15d B |\n", fH->bf_off_bits);
    printf("+--------------------------------+\n");
    printf("|       Bitmap info header       |\n");
    printf("+--------------------------------+\n");
    printf("| Header size:    %12d B |\n", iH->bi_size);
    printf("| Width:         %12d px |\n", iH->bi_width);
    printf("| Height:        %12d px |\n", iH->bi_height);
    printf("| Planes:           %12d |\n", iH->bi_planes);
    printf("| Bit count:        %12d |\n", iH->bi_bit_count);
    printf("| Compression mode: %12d |\n", iH->bi_compression);
    printf("| Image size:     %12d B |\n", iH->bi_size_image);
    printf("| X pixels/meter:   %12d |\n", iH->bi_x_pels_per_meter);
    printf("| Y pixels/meter:   %12d |\n", iH->bi_y_pels_per_meter);
    printf("| Color used:       %12d |\n", iH->bi_clr_used);
    printf("| Color important:  %12d |\n", iH->bi_clr_important);
    printf("+--------------------------------+\n");
    return BMP_OK;
}

BmpError bmp_read_file(Bitmap *bmp, const char *filename) {
    FILE *fp = fopen(filename, "rb");

    if (fp == NULL) { return BMP_FILE_NOT_FOUND; }

    fread(&bmp->file_header.bf_type, sizeof(uint16_t), 1, fp);
    if (bmp->file_header.bf_type != 0x4D42) {
        fclose(fp);
        return BMP_FILE_INVALID;
    }

    /* Read file header */
    fread(&bmp->file_header.bf_size, sizeof(uint32_t), 1, fp);
    fread(&bmp->file_header.bf_reserved1, sizeof(uint16_t), 1, fp);
    fread(&bmp->file_header.bf_reserved2, sizeof(uint16_t), 1, fp);
    fread(&bmp->file_header.bf_off_bits, sizeof(uint32_t), 1, fp);
    fread(&bmp->info_header.bi_size, sizeof(uint32_t), 1, fp);
    fread(&bmp->info_header.bi_width, sizeof(int32_t), 1, fp);
    fread(&bmp->info_header.bi_height, sizeof(int32_t), 1, fp);
    fread(&bmp->info_header.bi_planes, sizeof(uint16_t), 1, fp);
    fread(&bmp->info_header.bi_bit_count, sizeof(uint16_t), 1, fp);
    fread(&bmp->info_header.bi_compression, sizeof(uint32_t), 1, fp);
    fread(&bmp->info_header.bi_size_image, sizeof(uint32_t), 1, fp);
    fread(&bmp->info_header.bi_x_pels_per_meter, sizeof(int32_t), 1, fp);
    fread(&bmp->info_header.bi_y_pels_per_meter, sizeof(int32_t), 1, fp);
    fread(&bmp->info_header.bi_clr_used, sizeof(uint32_t), 1, fp);
    fread(&bmp->info_header.bi_clr_important, sizeof(uint32_t), 1, fp);

    size_t color_table_size = bmp->file_header.bf_off_bits - bmp->info_header.bi_size - 14;
    size_t data_size = bmp->file_header.bf_size - bmp->file_header.bf_off_bits;

    /* Read color table */
    bmp->color_table = NULL;
    if (color_table_size > 0) {
        bmp->color_table = (BmpRgbQuad *) malloc(color_table_size);
        if (bmp->color_table == NULL) { return BMP_OUT_OF_MEMORY; }
        fread(bmp->color_table, color_table_size, 1, fp);
    }

    /* Read data */
    bmp->data = NULL;
    if (data_size > 0) {
        bmp->data = (uint8_t *) malloc(data_size);
        if (bmp->data == NULL) { return BMP_OUT_OF_MEMORY; }
        fread(bmp->data, data_size, 1, fp);
    }

    /* Cleanup */
    fclose(fp);

    return BMP_OK;
}

BmpError bmp_write_file(Bitmap *bmp, const char *filename) {
    FILE *fp = fopen(filename, "wb");

    if (fp == NULL) { return BMP_FILE_NOT_FOUND; }

    /* Write file header */
    fwrite(&bmp->file_header.bf_type, sizeof(uint16_t), 1, fp);
    fwrite(&bmp->file_header.bf_size, sizeof(uint32_t), 1, fp);
    fwrite(&bmp->file_header.bf_reserved1, sizeof(uint16_t), 1, fp);
    fwrite(&bmp->file_header.bf_reserved2, sizeof(uint16_t), 1, fp);
    fwrite(&bmp->file_header.bf_off_bits, sizeof(uint32_t), 1, fp);

    /* Write info header */
    fwrite(&bmp->info_header.bi_size, sizeof(uint32_t), 1, fp);
    fwrite(&bmp->info_header.bi_width, sizeof(int32_t), 1, fp);
    fwrite(&bmp->info_header.bi_height, sizeof(int32_t), 1, fp);
    fwrite(&bmp->info_header.bi_planes, sizeof(uint16_t), 1, fp);
    fwrite(&bmp->info_header.bi_bit_count, sizeof(uint16_t), 1, fp);
    fwrite(&bmp->info_header.bi_compression, sizeof(uint32_t), 1, fp);
    fwrite(&bmp->info_header.bi_size_image, sizeof(uint32_t), 1, fp);
    fwrite(&bmp->info_header.bi_x_pels_per_meter, sizeof(int32_t), 1, fp);
    fwrite(&bmp->info_header.bi_y_pels_per_meter, sizeof(int32_t), 1, fp);
    fwrite(&bmp->info_header.bi_clr_used, sizeof(uint32_t), 1, fp);
    fwrite(&bmp->info_header.bi_clr_important, sizeof(uint32_t), 1, fp);

    /* Calculate sizes */
    size_t color_table_size = bmp->file_header.bf_off_bits - bmp->info_header.bi_size - 14;
    size_t data_size = bmp->file_header.bf_size - bmp->file_header.bf_off_bits;

    /* Write color table and data */
    if (color_table_size > 0) {
        fwrite(bmp->color_table, color_table_size, 1, fp);
    }
    if (data_size > 0) {
        fwrite(bmp->data, data_size, 1, fp);
    }

    /* Cleanup */
    fclose(fp);

    return BMP_OK;
}

BmpError bmp_get_pixel(Bitmap *bmp, int32_t x, int32_t y, uint8_t *r, uint8_t *g, uint8_t *b) {
    if (bmp == NULL) { return BMP_NULL_PTR; }
    if (bmp->data == NULL) { return BMP_NULL_PTR; }
    if (x < 0 || x >= bmp->info_header.bi_width) { return BMP_OUT_OF_RANGE; }
    if (y < 0 || y >= bmp->info_header.bi_height) { return BMP_OUT_OF_RANGE; }
    if (bmp->info_header.bi_bit_count != 24 && bmp->info_header.bi_bit_count != 32) {
        return BMP_INVALID_BIT_COUNT;
    }

    size_t pixel_size = bmp->info_header.bi_bit_count / 8;
    size_t row_size = bmp->info_header.bi_width * pixel_size;

    uint8_t *pixel = bmp->data + (bmp->info_header.bi_height - y - 1) * row_size + x * pixel_size;

    *b = pixel[0];
    *g = pixel[1];
    *r = pixel[2];

    return BMP_OK;
}

BmpError bmp_set_pixel(Bitmap *bmp, int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b) {
    if (bmp == NULL) { return BMP_NULL_PTR; }
    if (bmp->data == NULL) { return BMP_NULL_PTR; }
    if (x < 0 || x >= bmp->info_header.bi_width) { return BMP_OUT_OF_RANGE; }
    if (y < 0 || y >= bmp->info_header.bi_height) { return BMP_OUT_OF_RANGE; }
    if (bmp->info_header.bi_bit_count != 24 && bmp->info_header.bi_bit_count != 32) {
        return BMP_INVALID_BIT_COUNT;
    }

    size_t pixel_size = bmp->info_header.bi_bit_count / 8;
    size_t row_size = bmp->info_header.bi_width * pixel_size;

    uint8_t *pixel = bmp->data + (bmp->info_header.bi_height - y - 1) * row_size + x * pixel_size;

    pixel[0] = b;
    pixel[1] = g;
    pixel[2] = r;

    if (bmp->info_header.bi_bit_count == 32) {
        pixel[3] = 0;
    }

    return BMP_OK;
}

BmpError bmp_fill(Bitmap *bmp, uint8_t r, uint8_t g, uint8_t b) {
    if (bmp == NULL) { return BMP_NULL_PTR; }
    if (bmp->data == NULL) { return BMP_NULL_PTR; }
    if (bmp->info_header.bi_bit_count != 24 && bmp->info_header.bi_bit_count != 32) {
        return BMP_INVALID_BIT_COUNT;
    }

    size_t pixel_size = bmp->info_header.bi_bit_count / 8;
    size_t row_size = bmp->info_header.bi_width * pixel_size;

    for (int i = 0; i < bmp->info_header.bi_height; ++i) {
        for (int j = 0; j < bmp->info_header.bi_width; ++j) {
            uint8_t *pixel = bmp->data + i * row_size + j * pixel_size;
            pixel[0] = b;
            pixel[1] = g;
            pixel[2] = r;
            if (bmp->info_header.bi_bit_count == 32) {
                pixel[3] = 0;
            }
        }
    }

    return BMP_OK;
}
