#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct{
    unsigned char b, g, r;
}BGR;


BGR* BGR_init(unsigned char b, unsigned char g, unsigned char r){

    BGR* coppy = (BGR*)malloc(sizeof(BGR));

    coppy->b = b;
    coppy->g = g;
    coppy->r = r;

    return coppy;

}


void BGR_write_to_file(FILE* f, BGR* coppy){

    fwrite(coppy, sizeof(*coppy), 1, f);

}


typedef struct{

    unsigned short file_type;
    unsigned int file_size;
    unsigned int empty_space; // empty space
    unsigned int offset;

}BMP_header;


BMP_header* BMP_header_init(unsigned int bmp_data){

    BMP_header* coppy = (BMP_header*)malloc(sizeof(BMP_header));

    coppy->file_type = 0x4d42;
    coppy->file_size = (54 + bmp_data);
    coppy->empty_space = 0;
    coppy->offset = 0x00000036;

    return coppy;

}


void BMP_header_display(BMP_header* coppy){

    printf("%d %d %d %d\n", coppy->file_type, coppy->file_size, coppy->empty_space, coppy->offset);

}


void BMP_header_write_to_file(FILE* f, BMP_header* coppy){

    fwrite(&coppy->file_type, 2, 1, f);
    fwrite(&coppy->file_size, 4, 1, f);
    fwrite(&coppy->empty_space, 4, 1, f);
    fwrite(&coppy->offset, 4, 1, f);

}


typedef struct{

    unsigned int dib_size;
    unsigned int width;
    unsigned int height;
    unsigned short planes;
    unsigned short no_of_bits;
    unsigned int bi_rgb;
    unsigned int data_size;
    unsigned int resolution_width;
    unsigned int resolution_height;
    unsigned int no_of_colors;
    unsigned int no_of_imp_colors;

}DIB_header;


DIB_header* DIB_header_init(unsigned int width, unsigned int height, unsigned int data_size){

    DIB_header* coppy = (DIB_header*)malloc(sizeof(DIB_header));

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


void DIB_header_write_to_file(FILE* f, DIB_header* coppy){

    fwrite(coppy, sizeof(*coppy), 1, f);
    // fwrite(&coppy->dib_size, 4, 1, f);
    // fwrite(&coppy->width, 4, 1, f);
    // fwrite(&coppy->height, 4, 1, f);
    // fwrite(&coppy->planes, 2, 1, f);
    // fwrite(&coppy->no_of_bits, 2, 1, f);
    // fwrite(&coppy->bi_rgb, 4, 1, f);
    // fwrite(&coppy->data_size, 4, 1, f);
    // fwrite(&coppy->resolution_width, 4, 1, f);
    // fwrite(&coppy->resolution_height, 4, 1, f);
    // fwrite(&coppy->no_of_colors, 4, 1, f);
    // fwrite(&coppy->no_of_imp_colors, 4, 1, f);

}


int main(){
    
    FILE* f = fopen("img.bmp", "wb");


    unsigned int no_of_blocks = 10000;
    unsigned int data_size = no_of_blocks * no_of_blocks * 3;

    clock_t start = clock();

    BMP_header* h1 = BMP_header_init(data_size);
    DIB_header* d1 = DIB_header_init(no_of_blocks, no_of_blocks, data_size);

    BGR* first = BGR_init(0,0,255);
    BGR* second = BGR_init(0,255,0);
    BGR* third = BGR_init(255, 0, 0);
    BGR* fourth = BGR_init(0, 0, 0);

    printf("Initialisation: %lf\n", (clock() - start) / (double)CLOCKS_PER_SEC);

    start = clock();

    BMP_header_write_to_file(f, h1);
    DIB_header_write_to_file(f, d1);


    unsigned short space = 0;

    for(int i=0; i<no_of_blocks; i++){
        for(int j=0; j<no_of_blocks; j++){
            if(j%2==0 && i%3 == 0)
                BGR_write_to_file(f, first);
            else if(j%3 == 1 && i%2 == 1)
                BGR_write_to_file(f, second);
            else if(j%4 == 2 && i%4 == 2)
                BGR_write_to_file(f, third);
            else
                BGR_write_to_file(f, fourth);
        }
    }

    printf("Writing: %lfs\n", (double)(clock() - start) / CLOCKS_PER_SEC);

    free(h1);
    free(d1);

    fclose(f);

    return 0;

}
