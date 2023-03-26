#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "bmp.h"


void pattern1(const char* filename, unsigned int no_of_blocks){

    FILE* f = fopen(filename, "ab");

    if(f == NULL){
        printf("Can't open %s\n", filename);
        exit(-1);
    }

    BGR* first = BGR_init(0,0,255);
    BGR* second = BGR_init(0,255,0);
    BGR* third = BGR_init(255, 0, 0);
    BGR* fourth = BGR_init(0, 0, 0);

    
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

    fclose(f);

}


void pattern2(const char* filename, unsigned int no_of_blocks){

    FILE* f = fopen(filename, "ab");

    if(f == NULL){
        printf("Can't open %s\n", filename);
        exit(-1);
    }

    BGR *colors[1000];
    int index = 0;

    for(int i=0; i<10; i++){
        for(int j=0; j<10; j++){
            for(int k=0; k<10; k++, index++){
                colors[index] = BGR_init(i * 25, j * 25, k * 25);
            }
        }
    }

    for(int i=0; i<no_of_blocks; i++){
        for(int j=0; j<no_of_blocks; j++){
            BGR_write_to_file(f, colors[i * j % 1000]);
        }
    }

    fclose(f);
}


void pattern3(const char* filename, unsigned int no_of_blocks){
    
    FILE* f = fopen(filename, "ab");

    if(f == NULL){
        printf("Can't open %s\n", filename);
        exit(-1);
    }

    BGR *colors[1000];
    int index = 0;

    for(int i=0; i<10; i++){
        for(int j=0; j<10; j++){
            for(int k=0; k<10; k++, index++){
                colors[index] = BGR_init(i * 25, j * 25, k * 25);
            }
        }
    }

    int i, j;

    for(i=0; i<no_of_blocks; i++){
        for(j=0; j<no_of_blocks; j++){
            BGR_write_to_file(f, colors[((i*i/2) % (j+1)) % 1000]);
        }
    }
    
    printf("%d %d\n", i, j);

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
                    printf("Done\n");
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


int main(){

    unsigned int no_of_blocks = 1000;
    unsigned int data_size = no_of_blocks * no_of_blocks * 3;

    char message[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.\nLorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.\n";

    BMP_header* h1 = BMP_header_init(data_size);
    DIB_header* d1 = DIB_header_init(no_of_blocks, no_of_blocks, data_size);

    BMP_header_write_to_file("test.bmp", h1);
    DIB_header_write_to_file("test.bmp", d1);
    
    pattern3("test.bmp", no_of_blocks);

    lsb_method_write("test.bmp", "test2.bmp", message);
    lsb_method_read("test2.bmp", "decoded.bin");

    free(h1);
    free(d1);

    return 0;

}
