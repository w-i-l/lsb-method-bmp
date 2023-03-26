#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bmp.h"

BGR* BGR_init(unsigned char b, unsigned char g, unsigned char r){

    BGR* coppy = (BGR*)malloc(sizeof(BGR));

    if(coppy == NULL){
        printf("Problem with memory allocation\n");
        exit(-1);
    }

    coppy->b = b;
    coppy->g = g;
    coppy->r = r;

    return coppy;

}


void BGR_write_to_file(FILE* f, BGR* coppy){

    fwrite(coppy, sizeof(*coppy), 1, f);

}


BMP_header* BMP_header_init(unsigned int bmp_data){

    BMP_header* coppy = (BMP_header*)malloc(sizeof(BMP_header));
    
    if(coppy == NULL){
        printf("Problem with memory allocation\n");
        exit(-1);
    }

    coppy->file_type = 0x4d42;
    coppy->file_size = (54 + bmp_data);
    coppy->empty_space = 0;
    coppy->offset = 0x00000036;

    return coppy;

}


void BMP_header_display(BMP_header* coppy){

    printf("%d %d %d %d\n", coppy->file_type, coppy->file_size, coppy->empty_space, coppy->offset);

}


void BMP_header_write_to_file(const char* filename, BMP_header* coppy){

    FILE* f = fopen(filename, "wb");

     if(f == NULL){
        printf("Can't open %s\n", filename);
        exit(-1);
    }

    fwrite(&coppy->file_type, 2, 1, f);
    fwrite(&coppy->file_size, 4, 1, f);
    fwrite(&coppy->empty_space, 4, 1, f);
    fwrite(&coppy->offset, 4, 1, f);

    fclose(f);

}


DIB_header* DIB_header_init(unsigned int width, unsigned int height, unsigned int data_size){

    DIB_header* coppy = (DIB_header*)malloc(sizeof(DIB_header));
    
    if(coppy == NULL){
        printf("Problem with memory allocation\n");
        exit(-1);
    }

    coppy->dib_size = 0x28;
    coppy->width = width;
    coppy->height = height;
    coppy->planes = 0x1;
    coppy->no_of_bits = 0x18;
    coppy->bi_rgb = 0x0;
    coppy->data_size = data_size;
    coppy->resolution_width = 0xffff;
    coppy->resolution_height = 0xffff;
    coppy->no_of_colors = 0x0;
    coppy->no_of_imp_colors = 0x0;

    return coppy;

}


void DIB_heaader_display(DIB_header* coppy){

    printf("%d\n", coppy->dib_size);
    printf("%d %d\n", coppy->width, coppy->height);
    printf("%hu %hu\n", coppy->planes, coppy->no_of_bits);
    printf("%d \n", coppy->bi_rgb);
    printf("%d\n", coppy->data_size);
    printf("%d %d\n", coppy->resolution_width, coppy->resolution_height);
    printf("%d %d\n", coppy->no_of_colors, coppy->no_of_imp_colors);

}


void DIB_header_write_to_file(const char* filename, DIB_header* coppy){

    FILE* f = fopen(filename, "ab");
    
    if(f == NULL){
        printf("Can't open %s\n", filename);
        exit(-1);
    }

    fwrite(coppy, sizeof(*coppy), 1, f);

    fclose(f);

}
