#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;

typedef struct {
    BITMAPINFOHEADER bmiHeader;
    // Я пока не придумал что здесь для версии 4
} BITMAPV4HEADER;

typedef struct {
    BITMAPV4HEADER bmiV4Header;
    //  Я пока не придумал что здесь для версии 5
} BITMAPV5HEADER;
#pragma pack(pop)

void printBitmapFileHeader(BITMAPFILEHEADER *bfh) {
    printf("BITMAPFILEHEADER:\n");
    printf("  bfType: 0x%04X\n", bfh->bfType);
    printf("  bfSize: %u\n", bfh->bfSize);
    printf("  bfReserved1: %u\n", bfh->bfReserved1);
    printf("  bfReserved2: %u\n", bfh->bfReserved2);
    printf("  bfOffBits: %u\n", bfh->bfOffBits);
}

void printBitmapInfoHeader(BITMAPINFOHEADER *bih) {
    printf("BITMAPINFOHEADER:\n");
    printf("  biSize: %u\n", bih->biSize);
    printf("  biWidth: %d\n", bih->biWidth);
    printf("  biHeight: %d\n", bih->biHeight);
    printf("  biPlanes: %u\n", bih->biPlanes);
    printf("  biBitCount: %u\n", bih->biBitCount);
    printf("  biCompression: %u\n", bih->biCompression);
    printf("  biSizeImage: %u\n", bih->biSizeImage);
    printf("  biXPelsPerMeter: %d\n", bih->biXPelsPerMeter);
    printf("  biYPelsPerMeter: %d\n", bih->biYPelsPerMeter);
    printf("  biClrUsed: %u\n", bih->biClrUsed);
    printf("  biClrImportant: %u\n", bih->biClrImportant);
}

void printBitmapV4Header(BITMAPV4HEADER *biv4h) {
    printBitmapInfoHeader(&biv4h->bmiHeader);
    printf("BITMAPV4HEADER:\n");
    //  Я пока не придумал что здесь для версии 4
}

void printBitmapV5Header(BITMAPV5HEADER *biv5h) {
    printBitmapV4Header(&biv5h->bmiV4Header);
    // Дополнительные поля для версии 5
    printf("BITMAPV5HEADER:\n");
    //  Я пока не придумал что здесь для версии 5
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <bmp_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("Failed to open file");
        return 2;
    }

    BITMAPFILEHEADER bfh;
    fread(&bfh, sizeof(BITMAPFILEHEADER), 1, file);
    if (bfh.bfType != 0x4D42) {
        fprintf(stderr, "Not a BMP file\n");
        fclose(file);
        return 1;
    }

    

    BITMAPINFOHEADER bih;
    fread(&bih, sizeof(BITMAPINFOHEADER), 1, file);
    if (bih.biSize == sizeof(BITMAPINFOHEADER)) {
        printf("  versionBMP: %u\n", 3);
        printBitmapFileHeader(&bfh);
        printBitmapInfoHeader(&bih);
    } else if (bih.biSize == sizeof(BITMAPV4HEADER)) {
        BITMAPV4HEADER biv4h;
        printBitmapFileHeader(&bfh);
        printf("  versionBMP: %u\n", 4);
        fseek(file, -sizeof(BITMAPINFOHEADER), SEEK_CUR);
        fread(&biv4h, sizeof(BITMAPV4HEADER), 1, file);
        printBitmapV4Header(&biv4h);
    } else if (bih.biSize == sizeof(BITMAPV5HEADER)) {
        BITMAPV5HEADER biv5h;
        printf("  versionBMP: %u\n", 5);
        printBitmapFileHeader(&bfh);
        fseek(file, -sizeof(BITMAPINFOHEADER), SEEK_CUR);
        fread(&biv5h, sizeof(BITMAPV5HEADER), 1, file);
        printBitmapV5Header(&biv5h);
    } else {
        printBitmapFileHeader(&bfh);
        fprintf(stderr, "Unsupported BMP version\n");
        fclose(file);
        return 1;
    }

    fclose(file);
    return 0;
}
