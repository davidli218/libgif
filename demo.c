#include <stdio.h>

#include <libgif.h>

int main() {
    PARSE_STATUS error_code;
    FILE *gifFile = fopen("./test_source/test_gif_1.gif", "rb");

    error_code = getGifInfo(gifFile);

    fclose(gifFile);

    return error_code;
}
