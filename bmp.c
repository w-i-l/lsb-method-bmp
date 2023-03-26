#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "Headers/bmp.h"

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


// encrypt the message using the content
// from_file and writes it to_file
void lsb_method_write(const char* from_file, const char* to_file, char* message){

    FILE* from = fopen(from_file , "rb");

    if(from == NULL){
        printf("Can't open %s\n", from_file);
        exit(-1);
    }

    FILE* to = fopen(to_file, "wb");

    if(to == NULL){
        printf("Can't open %s\n", to_file);
        exit(-1);
    }

    // reading the offset of RGB values
    fseek(from, 0xa, SEEK_SET);
    int offset;
    fread(&offset, sizeof(int), 1, from);

    // reading the width and height
    int width, height;
    fseek(from, 0x12, SEEK_SET);
    fread(&width, sizeof(int), 1, from);
    fread(&height, sizeof(int), 1, from);
    
    // reseting the file offset
    fseek(from, 0 , SEEK_SET);

    // reading the BMP_header and DIB_header
    // and writing to the new file
    char buffer[54];
    fread(buffer, sizeof(char), 54, from);
    fwrite(buffer, sizeof(char), 54, to);

    // no of bits unparsed from the current block of message
    unsigned int index = 0, no_of_bits = 8;
    int final_no_of_bits = 8;

    BGR aux;
    char bit;

    // iterating all RGB values
    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            
            fread(&aux, sizeof(BGR), 1, from);

            // until the message isn't empty
            if(message[index] != 0){

                // if we can write 3 bits
                if(no_of_bits >= 3){

                    // we take the bit from the position (no_of_bits -1)
                    bit = ((message[index] & (1 << (no_of_bits - 1) )) >> (no_of_bits - 1));
                    // and put the bit from our message in BGR 
                    aux.b = ((aux.b >> 1) << 1) | bit;

                    bit = ((message[index] & (1 << (no_of_bits - 2) )) >> (no_of_bits - 2));
                    aux.g = ((aux.g >> 1) << 1) | bit;

                    bit = ((message[index] & (1 << (no_of_bits - 3) )) >> (no_of_bits - 3));
                    aux.r = ((aux.r >> 1) << 1) | bit;

                    no_of_bits -= 3;

                    // if all the bits were written
                    if(no_of_bits == 0){
                        index ++;
                        no_of_bits = 8;
                    }
                }

                // if we can write only 2 bits
                else if(no_of_bits == 2){

                    // we write them
                    aux.b = ((aux.b >> 1) << 1) | ((message[index] & 2) >> 1);
                    aux.g = ((aux.g >> 1) << 1) | (message[index] & 1);

                    // go to the next block of the message
                    index ++;

                    // write the next bit
                    aux.r = ((aux.r >> 1) << 1) | ((message[index] & (1 << 7)) >> 7);
                    no_of_bits = 7;
                }

                // if we can write only one bit
                else if(no_of_bits == 1){
                    aux.b = ((aux.b >> 1) << 1) | (message[index] & 1);
                    
                    // go to the next block of the message
                    index ++;

                    // write the next bits
                    aux.g = ((aux.g >> 1) << 1) | ((message[index] & (1 << 7)) >> 7);
                    aux.r = ((aux.r >> 1) << 1) | ((message[index] & (1 << 6)) >> 6);

                    no_of_bits = 6;
                }

            }

            // if we end to write the entire message
            // we write a "final sequence" of 9 1s
            // on decode we should get a 0xff
            else if(final_no_of_bits > -20){
                
                aux.b = ((aux.b >> 1) << 1) | 1;
                aux.g = ((aux.g >> 1) << 1) | 1;
                aux.r = ((aux.r >> 1) << 1) | 1;

                final_no_of_bits -= 3;
            }
            
            // write the updated version of the pixel
            fwrite(&aux, sizeof(BGR), 1, to);
        }
    }

    fclose(from);
    fclose(to);

}


// decrypt the message using the content
// from_file and writes it to_file
void lsb_method_read(const char* from_file, const char* to_file){

    FILE* from = fopen(from_file , "rb");

    if(from == NULL){
        printf("Can't open %s\n", from_file);
        exit(-1);
    }

    FILE* to = fopen(to_file, "wb");
    
    if(to == NULL){
            printf("Can't open %s\n", to_file);
            exit(-1);
        }

    // read the RGBs offset
    fseek(from, 0xa, SEEK_SET);
    int offset;
    fread(&offset, sizeof(int), 1, from);

    // read the width and height
    int width, height;
    fseek(from, 0x12, SEEK_SET);
    fread(&width, sizeof(int), 1, from);
    fread(&height, sizeof(int), 1, from);

    // fseek(from, 0, SEEK_SET);
    // char c[54];
    // fread(c, sizeof(char), 54, from);

    // skip the headers
    fseek(from, 54, SEEK_SET);

    // get the message size
    // how many BGR structs we have - width * height
    // multiply by 3 colors (RGB)
    // divided by 8 to convert from bits to bytes
    unsigned int size = ceil((width * height * 3) / 8);

    // the buffer to store the message
    char* buffer = (char*) malloc(sizeof(char) * size);

    if(buffer == NULL){
        printf("Problem with memory allocation!\n");
        exit(-1);
    }
    
    // how many bits we have written 
    // to the current block of buffer
    int buffer_index = 0, no_of_bits = 0;

    BGR aux;

    for(int i=0; i<width; i++){
        // parse all BGR or the buffer is full
        // and we had more bytes written
        for(int j=0; j<height && buffer_index<size; j++){
            
            // get the current BGR
            fread(&aux, sizeof(BGR), 1, from);

            // if we can read 3 bits
            if(no_of_bits <= 5){
                
                // we create space to write
                buffer[buffer_index] <<= 1;
                // and write the last bit
                buffer[buffer_index] |= aux.b & 1;

                buffer[buffer_index] <<= 1;
                buffer[buffer_index] |= aux.g & 1;

                buffer[buffer_index] <<= 1;
                buffer[buffer_index] |= aux.r & 1;

                no_of_bits += 3;
                
                // if we get the final char
                if(buffer[buffer_index] == 127){
                    fwrite(buffer, sizeof(char), buffer_index-1, to);  
                    return;
                }

                // if the current block is full
                if(no_of_bits == 8){
                    
                    // go to next one
                    buffer_index ++;
                    buffer[buffer_index] = 0;
                    no_of_bits = 0;
                }
            }

            // if we can write 2 bits
            else if(no_of_bits == 6){

                buffer[buffer_index] <<= 1;
                buffer[buffer_index] |= aux.b & 1;

                buffer[buffer_index] <<= 1;
                buffer[buffer_index] |= aux.g & 1;

                if(buffer[buffer_index] == 127){
                    fwrite(buffer, sizeof(char), buffer_index-1, to);
                    return;
                }

                if(buffer_index != size-1){
                    buffer_index ++;
                    buffer[buffer_index] = 0;
                }

                // write the next bit
                buffer[buffer_index] |= aux.r & 1;

                no_of_bits = 1;
            }

            // if we can write only onw bit
            else if(no_of_bits == 7){

                buffer[buffer_index] <<= 1;
                buffer[buffer_index] |= aux.b & 1;

                if(buffer[buffer_index] == 127){
                    fwrite(buffer, sizeof(char), buffer_index-1, to);  
                    printf("Done\n");
                    return;
                }

                if(buffer_index != size-1){
                    buffer_index ++;
                    buffer[buffer_index] = 0;
                }

                // write the last 2 bits
                buffer[buffer_index] |= aux.g & 1;

                buffer[buffer_index] <<= 1;
                buffer[buffer_index] |= aux.r & 1;

                no_of_bits = 2;
            }
        }
    }              

    // if the file wasn't large enough to write the final sequence
    // we write the buffer
    fwrite(buffer, sizeof(char), buffer_index, to);  

    fclose(from);
    fclose(to);
}
