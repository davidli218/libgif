#pragma once

#include <stdint.h>

typedef struct gif_header {
    uint8_t Signature[3];
    uint8_t Version[3];
} GIF_HEADER;


typedef struct gif_lsd {
    uint16_t Width;
    uint16_t Height;
    struct global_flag {
        uint8_t GlobalPal: 1;
        uint8_t ColorRes: 3;
        uint8_t SortFlag: 1;
        uint8_t PalBits: 3;
    } GlobalFlag;
    uint8_t Background;
    uint8_t Aspect;
} GIF_LSD;

typedef struct gif_color_table {
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
} GIF_COLOR_TABLE;

typedef struct gif_image {
    uint16_t wLeft;
    uint16_t wTop;
    uint16_t wWidth;
    uint16_t wDepth;
    struct local_flag {
        uint8_t PalBits: 3;
        uint8_t Reserved: 2;
        uint8_t SortFlag: 1;
        uint8_t Interlace: 1;
        uint8_t LocalPal: 1;
    } LocalFlag;
} GIF_IMAGE;

int getGifInfo(FILE *file);
