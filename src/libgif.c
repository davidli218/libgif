#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libgif.h"

static int readHeader(FILE *file, GIF_HEADER *returnHeader) {
    /**
     * Reads Header
     *
     * @return 0: Read successfully without error.
     *         1: Unrecognisable Signature is found.
     *         2: Unrecognisable Version is found.
     */

    char stringBuffer[6];

    fread(stringBuffer, 1, 6, file);

    memcpy(returnHeader->Signature, stringBuffer, 3);
    memcpy(returnHeader->Version, stringBuffer + 3, 3);

    if (memcmp(returnHeader->Signature, "GIF", 3) != 0)
        return 1;
    if (memcmp(returnHeader->Version, "89a", 3) != 0
        && memcmp(returnHeader->Version, "87a", 3) != 0)
        return 2;

    return 0;
}

static int readLSD(FILE *file, GIF_LSD *returnLSD) {
    /**
     * Reads Logical Screen Descriptor
     */
    fread(&returnLSD->Width, 2, 1, file);
    fread(&returnLSD->Height, 2, 1, file);

    fread(&returnLSD->GlobalFlag, 1, 1, file);

    fread(&returnLSD->Background, 1, 1, file);
    fread(&returnLSD->Aspect, 1, 1, file);

    return 0;
}

static int readGlobalColorTable(FILE *file, int tSize, GIF_COLOR_TABLE *returnGCD) {
    /**
     * Reads Global Color Table
     */
    for (int i = 0; i < tSize; ++i) {
        fread((returnGCD + i), sizeof(GIF_COLOR_TABLE), 1, file);
    }

    return 0;
}

static int readImageDescriptors(FILE *file, GIF_IMAGE *returnImages, int *returnSize) {
    char charBuffer;
    *returnSize = 0;


    for (fread(&charBuffer, 1, 1, file);
         charBuffer ^ ','; fread(&charBuffer, 1, 1, file)) {
        returnSize++;
    }

    return 0;
}

static void printHeader(GIF_HEADER header) {
    /**
     * Print Header
     */
    printf("\n<Header>\n");
    printf("Format: %3.3s %3.3s\n", header.Signature, header.Version);
}

static void printLSD(GIF_LSD lsd) {
    /**
     * Print Logical Screen Descriptor
     */
    printf("\n<Logical Screen Descriptor>\n");
    printf("Resolution: %d x %d\n", lsd.Width, lsd.Height);

    printf("Global Color Table Flag: %s\n", lsd.GlobalFlag.GlobalPal ? "True" : "False");
    printf("Color Resolution: %d bits\n", lsd.GlobalFlag.ColorRes + 1);
    printf("Sort Flag: %s\n", lsd.GlobalFlag.SortFlag ? "True" : "False");
    printf("Size of Global Color Table: %d\n", 1 << (lsd.GlobalFlag.PalBits + 1));

    printf("BackgroundColor Index: %d\n", lsd.Background);

    if (lsd.Aspect == 0)
        printf("PixelAspect Ratio: None\n");
    else
        printf("PixelAspect Ratio: %.2lf\n", ((double) lsd.Aspect + 15) / 64);
}

static void printGlobalColorTable(GIF_COLOR_TABLE *gct, int tSize) {
    /**
     * Print Global Color Table
     */
    GIF_COLOR_TABLE tableBuffer;

    printf("\n<Global Color Table>\n");
    for (int i = 0; i < tSize; ++i) {
        tableBuffer = *(gct + i);
        printf("(%3.d, %3.d, %3.d)", tableBuffer.Red, tableBuffer.Green, tableBuffer.Blue);
        printf((i + 1) % 8 == 0 ? "\n" : "\t");
    }
}

int getGifInfo(FILE *file) {
    GIF_HEADER header;
    GIF_LSD lsd;
    GIF_COLOR_TABLE *globalTable = NULL;
    GIF_COLOR_TABLE *localTable = NULL;

    if (readHeader(file, &header) != 0)
        return 1;  /* Not a GIF */

    readLSD(file, &lsd);

    globalTable = (GIF_COLOR_TABLE *)
            malloc(sizeof(GIF_COLOR_TABLE) * (1 << (lsd.GlobalFlag.PalBits + 1)));

    readGlobalColorTable(file, 1 << (lsd.GlobalFlag.PalBits + 1), globalTable);

    printHeader(header);
    printLSD(lsd);
    printGlobalColorTable(globalTable, 1 << (lsd.GlobalFlag.PalBits + 1));

    free(globalTable);
    free(localTable);

    return 0;
}
