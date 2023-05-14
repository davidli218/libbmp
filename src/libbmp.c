#include <stdio.h>
#include <stdlib.h>

#include "libbmp.h"

static void bmp_init(Bitmap *bmp) {
    bmp->file_header.bf_type = 0x4D42;
    bmp->file_header.bf_reserved1 = 0;
    bmp->file_header.bf_reserved2 = 0;
    bmp->file_header.bf_off_bits = 40 + 14;
    /* bmp->file_header.bf_size
     * will be set later */

    bmp->info_header.bi_size = 40;
    bmp->info_header.bi_planes = 1;
    bmp->info_header.bi_compression = 0;
    bmp->info_header.bi_x_pels_per_meter = 0;
    bmp->info_header.bi_y_pels_per_meter = 0;
    bmp->info_header.bi_clr_used = 0;
    bmp->info_header.bi_clr_important = 0;
    /* bmp->info_header.bi_width
     * bmp->info_header.bi_height
     * bmp->info_header.bi_bit_count
     * bmp->info_header.bi_size_image
     * will be set later */

    bmp->color_table = NULL;
    bmp->data = NULL;
}

static BmpError bmp_create(Bitmap *bmp, int32_t width, int32_t height, uint16_t bit_count) {
    if (bit_count != 24 && bit_count != 32) {
        return BMP_INVALID_BIT_COUNT;
    }

    bmp_init(bmp);

    /* Set rest of the fields in info header */
    bmp->info_header.bi_width = width;
    bmp->info_header.bi_height = height;
    bmp->info_header.bi_bit_count = bit_count;
    bmp->info_header.bi_size_image = width * height * (bit_count / 8);

    /* Set rest of the fields in file header */
    bmp->file_header.bf_size = bmp->info_header.bi_size_image + bmp->file_header.bf_off_bits;

    /* Allocate memory for color table */
    bmp->color_table = NULL;

    /* Allocate memory for image data */
    bmp->data = (uint8_t *) malloc(bmp->info_header.bi_size_image);

    if (bmp->data == NULL) { return BMP_OUT_OF_MEMORY; }

    /* Initialize image data to white */
    if (bit_count == 24) {
        for (int i = 0; i < bmp->info_header.bi_size_image; i += 3) {
            bmp->data[i] = 0xFF;
            bmp->data[i + 1] = 0xFF;
            bmp->data[i + 2] = 0xFF;
        }
    } else if (bit_count == 32) {
        for (int i = 0; i < bmp->info_header.bi_size_image; i += 4) {
            bmp->data[i] = 0xFF;
            bmp->data[i + 1] = 0xFF;
            bmp->data[i + 2] = 0xFF;
            bmp->data[i + 3] = 0xFF;
        }
    } else {
        return UNKNOWN_ERROR;
    }

    return BMP_OK;
}

Bitmap *bmp_new(int32_t width, int32_t height, uint16_t bit_count) {
    Bitmap *bmp = (Bitmap *) malloc(sizeof(Bitmap));

    if (bmp == NULL) { return NULL; }

    BmpError error = bmp_create(bmp, width, height, bit_count);

    if (error != BMP_OK) {
        free(bmp);
        return NULL;
    }

    return bmp;
}

BmpError bmp_free(Bitmap *bmp) {
    if (bmp->color_table != NULL) {
        free(bmp->color_table);
    }

    if (bmp->data != NULL) {
        free(bmp->data);
    }

    return BMP_OK;
}

BmpError bmp_print_header(Bitmap *bmp) {
    printf("<Bitmap header>\n");
    printf("| Type: %c%c\n", bmp->file_header.bf_type & 0xFF, bmp->file_header.bf_type >> 8);
    printf("| File size: %d\n", bmp->file_header.bf_size);
    printf("| Data offset: %d\n", bmp->file_header.bf_off_bits);

    printf("\n<Bitmap info header>\n");
    printf("| Header size: %d\n", bmp->info_header.bi_size);
    printf("| Width: %d\n", bmp->info_header.bi_width);
    printf("| Height: %d\n", bmp->info_header.bi_height);
    printf("| Planes: %d\n", bmp->info_header.bi_planes);
    printf("| Bit count: %d\n", bmp->info_header.bi_bit_count);
    printf("| Compression: %d\n", bmp->info_header.bi_compression);
    printf("| Image size: %d\n", bmp->info_header.bi_size_image);
    printf("| X pixels per meter: %d\n", bmp->info_header.bi_x_pels_per_meter);
    printf("| Y pixels per meter: %d\n", bmp->info_header.bi_y_pels_per_meter);
    printf("| Color used: %d\n", bmp->info_header.bi_clr_used);
    printf("| Color important: %d\n", bmp->info_header.bi_clr_important);

    return BMP_OK;
}

BmpError bmp_read_file(Bitmap *bmp, const char *filename) {
    FILE *fp = fopen(filename, "rb");

    if (fp == NULL) { return BMP_FILE_NOT_FOUND; }

    /* Read file header */
    fread(&bmp->file_header.bf_type, sizeof(uint16_t), 1, fp);
    if (bmp->file_header.bf_type != 0x4D42) {
        fclose(fp);
        return BMP_FILE_INVALID;
    }
    fread(&bmp->file_header.bf_size, sizeof(uint32_t), 1, fp);
    fread(&bmp->file_header.bf_reserved1, sizeof(uint16_t), 1, fp);
    fread(&bmp->file_header.bf_reserved2, sizeof(uint16_t), 1, fp);
    fread(&bmp->file_header.bf_off_bits, sizeof(uint32_t), 1, fp);

    /* Read info header */
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

    /* Read color table */
    size_t color_table_size = bmp->file_header.bf_off_bits - bmp->info_header.bi_size - 14;

    if (color_table_size > 0) {
        bmp->color_table = (RgbQuad *) malloc(color_table_size);
        if (bmp->color_table == NULL) { return BMP_OUT_OF_MEMORY; }
        fread(bmp->color_table, color_table_size, 1, fp);
    } else {
        bmp->color_table = NULL;
    }

    /* Read data */
    size_t data_size = bmp->file_header.bf_size - bmp->file_header.bf_off_bits;

    if (data_size > 0) {
        bmp->data = (uint8_t *) malloc(data_size);
        if (bmp->data == NULL) { return BMP_OUT_OF_MEMORY; }
        fread(bmp->data, data_size, 1, fp);
    } else {
        bmp->data = NULL;
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
            uint8_t *pixel =
                    bmp->data + (bmp->info_header.bi_height - i - 1) * row_size + j * pixel_size;
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