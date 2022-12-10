#pragma once

#include <stdint.h>

typedef enum parse_status {
    PARSE_SUCCESS,
    UNEXPECTED_CONTENT,
    UNEXPECTED_EOF,
    RECEIVED_NULL_PARAM,
} PARSE_STATUS;

typedef struct gif_header {
    /* GIF署名 */
    uint8_t signature[3];

    /* GIF文件版本号 */
    uint8_t version[3];
} GIF_HEADER;

typedef struct gif_lsd {
    /* GIF图象的宽度 */
    uint16_t width;

    /* GIF图象的高度 */
    uint16_t height;

    struct {
        /* Global Color Table Flag (全局颜色列表标志)
         * 当置位时表示有全局颜色列表, pixel值有意义 */
        uint8_t m: 1;

        /* Color Resolution (颜色深度)
         * cr+1确定图象的颜色深度 */
        uint8_t cr: 3;

        /* Sort Flag (分类标志)
         * 如果置位表示全局颜色列表分类排列 */
        uint8_t s: 1;

        /* 全局颜色列表大小
         * pixel+1确定颜色列表的索引数(2^(pixel+1)) */
        uint8_t pixel: 3;
    } globalFlag;

    /* 背景颜色
 * 在全局颜色列表中的索引，如果没有全局颜色列表，该值没有意义*/
    uint8_t background;

    /* Pixel aspect Radio (像素宽高比) */
    uint8_t aspect;
} GIF_LSD;

typedef struct gif_rgb_value {
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
} GIF_RGB_VALUE;

typedef struct gif_info {
    /* Header (文件头) */
    GIF_HEADER header;

    /* Logical Screen Descriptor (逻辑屏幕标识符) */
    GIF_LSD lsd;

    /* Global Color Table (全局颜色列表) */
    GIF_RGB_VALUE *gcb;
} GIF_INFO;

PARSE_STATUS getGifInfo(FILE *file);
