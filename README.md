# libbmp

A Bitmap (BMP) Parsing & Rendering Library.



## Usage

### 1. Checkerboard Pattern (Green/Red)

![](/example/output/gr_checkerboard.bmp)

```c
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
```

### 2. Gradient Patten

![](/example/output/gradient_patten.bmp)

```c
#include <libbmp.h>

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
```

