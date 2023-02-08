#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libgif.h"

/**
 * Reads Header (文件头)
 */
static PARSE_STATUS readHeader(FILE *file, GIF_HEADER *returnHeader) {
    char stringBuffer[6];

    if (file == NULL || returnHeader == NULL) {
        return RECEIVED_NULL_PARAM;
    }

    if (fread(stringBuffer, 1, 6, file) != 6) {
        return UNEXPECTED_EOF;
    }

    if (memcmp(stringBuffer, "GIF87a", 6) != 0 &&
        memcmp(stringBuffer, "GIF89a", 6) != 0) {
        memset(returnHeader->signature, 0, 3);
        memset(returnHeader->version, 0, 3);

        return UNEXPECTED_CONTENT;
    }

    memcpy(returnHeader, stringBuffer, 6);

    return PARSE_SUCCESS;
}

/**
 * Reads Logical Screen Descriptor (逻辑屏幕标识符)
 */
static PARSE_STATUS readLSD(FILE *file, GIF_LSD *returnLSD) {
    if (file == NULL || returnLSD == NULL) {
        return RECEIVED_NULL_PARAM;
    }

    if (fread(returnLSD, 7, 1, file) != 1) {
        return UNEXPECTED_EOF;
    }

    return PARSE_SUCCESS;
}

/**
 * Reads Global Color Table (全局颜色列表)
 */
static PARSE_STATUS readGlobalColorTable(FILE *file, int tSize, GIF_RGB_VALUE *returnGCD) {
    if (file == NULL || returnGCD == NULL) {
        return RECEIVED_NULL_PARAM;
    }

    for (int i = 0; i < tSize; ++i) {
        if (fread((returnGCD + i), sizeof(GIF_RGB_VALUE), 1, file) != 1) {
            return UNEXPECTED_EOF;
        }
    }

    return PARSE_SUCCESS;
}

/**
* Print Header (文件头)
*/
static void printHeader(GIF_HEADER *header) {
    printf("\n<Header>\n");
    printf("Format: %3.3s %3.3s\n", header->signature, header->version);
}

/**
 * Print Logical Screen Descriptor (逻辑屏幕标识符)
 */
static void printLSD(GIF_LSD *lsd) {
    printf("\n<Logical Screen Descriptor>\n");
    printf("Resolution: %d x %d\n", lsd->width, lsd->height);

    printf("Have Global Color Table: %s\n", lsd->globalFlag.m ? "True" : "False");
    printf("Color Resolution: %d bits\n", lsd->globalFlag.cr + 1);
    printf("Sort Flag: %s\n", lsd->globalFlag.s ? "True" : "False");
    printf("Size of Global Color Table: %d\n", 1 << (lsd->globalFlag.pixel + 1));

    printf("BackgroundColor Index: %d\n", lsd->background);

    if (lsd->aspect == 0)
        printf("PixelAspect Ratio: None\n");
    else
        printf("PixelAspect Ratio: %.2lf\n", ((double) lsd->aspect + 15) / 64);
}

/**
 * Print Global Color Table (全局颜色列表)
 */
static void printGlobalColorTable(GIF_RGB_VALUE *gct, int tSize) {
    GIF_RGB_VALUE tableBuffer;

    printf("\n<Global Color Table>\n");
    for (int i = 0; i < tSize; ++i) {
        tableBuffer = *(gct + i);
        printf("(%3.d,%3.d,%3.d)", tableBuffer.Red, tableBuffer.Green, tableBuffer.Blue);
        printf((i + 1) % 8 == 0 ? "\n" : " ");
    }
}

PARSE_STATUS getGifInfo(FILE *file) {
    PARSE_STATUS error_code;
    GIF_INFO gifInfo;

    rewind(file);

    if (file == NULL) {
        return RECEIVED_NULL_PARAM;
    }

    if ((error_code = readHeader(file, &gifInfo.header)) != PARSE_SUCCESS) {
        goto cleaning;
    } else {
        printHeader(&gifInfo.header);
    }

    if ((error_code = readLSD(file, &gifInfo.lsd)) != PARSE_SUCCESS) {
        goto cleaning;
    } else {
        printLSD(&gifInfo.lsd);
    }

    if (gifInfo.lsd.globalFlag.m == 1) {
        int gcbSize = 1 << (gifInfo.lsd.globalFlag.pixel + 1);
        gifInfo.gcb = (GIF_RGB_VALUE *) malloc(sizeof(GIF_RGB_VALUE) * gcbSize);

        if ((error_code = readGlobalColorTable(file, gcbSize, gifInfo.gcb)) != PARSE_SUCCESS) {
            goto cleaning;
        } else {
            printGlobalColorTable(gifInfo.gcb, gcbSize);
        }
    }

    error_code = PARSE_SUCCESS;

    cleaning:
    free(gifInfo.gcb);
    return error_code;
}
