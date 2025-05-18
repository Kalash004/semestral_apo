
#include "img.h"
/*
This module provides functionality to load PPM (P6) images from disk into a structured format
and convert pixel RGB values to 16-bit RGB565 hexadecimal format suitable for display. 
*/

// convert to rgb565 format
uint32_t convert_rgb_to_hexa(Pixel rgb) {
    uint16_t r = ((uint32_t)rgb.red  >> 3) & 0x1F; // 5 bits
    uint16_t g = ((uint32_t)rgb.green >> 2) & 0x3F; // 6 bits
    uint16_t b = ((uint32_t)rgb.blue >> 3) & 0x1F; // 5 bits

    return (r << 11) | (g << 5) | b;
}

// load img to an Img structure
Img* ppm_load_image(char *path) {
    char buff[16];
    Img *img;
    FILE *fp;
    int rgbscanval;

    fp = fopen(path, "rb");
    if (!fp) {
        fprintf(stderr, "Unable to open img file\n");
        return NULL;
    }

    if (!fgets(buff, sizeof(buff), fp)) {
        fclose(fp);
        return NULL;
    }

    if (buff[0] != 'P' || buff[1] != '6') {
        fprintf(stderr, "Invalid P6 format\n");
        fclose(fp);
        return NULL;
    }

    img = (Img *)malloc(sizeof(Img));
    if (!img) {
        fprintf(stderr, "Malloc fail\n");
        fclose(fp);
        return NULL;
    }

    char c;
    if (fscanf(fp, "%c", &c) == 1 && c == '#') {
        while (fscanf(fp, "%c", &c) == 1 && c != '\n') {}
    }
    fseek(fp, -1, SEEK_CUR);

    if (fscanf(fp, "%d %d", &img->w, &img->h) != 2) {
        fprintf(stderr, "Invalid image size\n");
        free(img);
        fclose(fp);
        return NULL;
    }

    if (fscanf(fp, "%d", &rgbscanval) != 1) {
        fprintf(stderr, "Invalid max rgb value\n");
        free(img);
        fclose(fp);
        return NULL;
    }

    while (fgetc(fp) != '\n');

    img->data = (Pixel*)malloc(img->w * img->h * sizeof(Pixel));
    if (!img->data) {
        fprintf(stderr, "Image data malloc fail\n");
        free(img);
        fclose(fp);
        return NULL;
    }

    if (fread(img->data, 3 * img->w, img->h, fp) != img->h) {
        fprintf(stderr, "Error loading img file\n");
        free(img->data);
        free(img);
        fclose(fp);
        return NULL;
    }

    fclose(fp);
    return img;
}
