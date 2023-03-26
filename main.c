#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "bmp.h"


void pattern1(const char* filename, unsigned int no_of_blocks){

    FILE* f = fopen(filename, "ab");

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


void lsb_method_write(const char* from_file, const char* to_file, char* message){

    // FILE* tmp = fopen("tmp.txt", "w");
    FILE* from = fopen(from_file , "rb");
    FILE* to = fopen(to_file, "wb");

    fseek(from, 0xa, SEEK_SET);
    int offset;
    fread(&offset, sizeof(int), 1, from);

    int width, height;
    fseek(from, 0x12, SEEK_SET);
    fread(&width, sizeof(int), 1, from);
    fread(&height, sizeof(int), 1, from);

    fseek(from, 0 , SEEK_SET);

    // reading the BMP_header and DIB_header
    char buffer[54];
    fread(buffer, sizeof(char), 54, from);
    fwrite(buffer, sizeof(char), 54, to);

    unsigned int index = 0, no_of_bits = 8;
    int final_no_of_bits = 8;

    // printf("%i %i %i\n", offset, width, height);

    BGR aux;
    char bit;

    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            
            fread(&aux, sizeof(BGR), 1, from);

            if(message[index] != 0){

                if(no_of_bits >= 3){
                    bit = ((message[index] & (1 << (no_of_bits - 1) )) >> (no_of_bits - 1));
                    aux.b = ((aux.b >> 1) << 1) | bit;

                    bit = ((message[index] & (1 << (no_of_bits - 2) )) >> (no_of_bits - 2));
                    aux.g = ((aux.g >> 1) << 1) | bit;

                    bit = ((message[index] & (1 << (no_of_bits - 3) )) >> (no_of_bits - 3));
                    aux.r = ((aux.r >> 1) << 1) | bit;

                    no_of_bits -= 3;

                    if(no_of_bits == 0){
                        index ++;
                        no_of_bits = 8;
                    }
                }

                else if(no_of_bits == 2){
                    aux.b = ((aux.b >> 1) << 1) | ((message[index] & 2) >> 1);
                    aux.g = ((aux.g >> 1) << 1) | (message[index] & 1);

                    index ++;
                    aux.r = ((aux.r >> 1) << 1) | ((message[index] & (1 << 7)) >> 7);
                    no_of_bits = 7;
                }

                else if(no_of_bits == 1){
                    aux.b = ((aux.b >> 1) << 1) | (message[index] & 1);

                    index ++;
                    aux.g = ((aux.g >> 1) << 1) | ((message[index] & (1 << 7)) >> 7);
                    aux.r = ((aux.r >> 1) << 1) | ((message[index] & (1 << 6)) >> 6);
                    no_of_bits = 6;
                }

            }
            // if we end to write the message
            // we write 0xff
            else if(final_no_of_bits > -20){

                printf("Apelat %d", final_no_of_bits);
                
                aux.b = ((aux.b >> 1) << 1) | 1;
                aux.g = ((aux.g >> 1) << 1) | 1;
                aux.r = ((aux.r >> 1) << 1) | 1;

                final_no_of_bits -= 3;
            }
            
            // fprintf(tmp, "%c %c %c\n", (aux.b & 1) + '0', (aux.g & 1) + '0', (aux.r & 1) + '0');
            fwrite(&aux, sizeof(BGR), 1, to);
        }
    }

    // fclose(tmp);
    fclose(from);
    fclose(to);

}


void lsb_method_read(const char* from_file, const char* to_file){

    FILE* from = fopen(from_file , "rb");
    FILE* to = fopen(to_file, "wb");
    FILE* tmp = fopen("tmp2.txt", "w");

    fseek(from, 0xa, SEEK_SET);
    int offset;
    fread(&offset, sizeof(int), 1, from);

    int width, height;
    fseek(from, 0x12, SEEK_SET);
    fread(&width, sizeof(int), 1, from);
    fread(&height, sizeof(int), 1, from);

    fseek(from, 0, SEEK_SET);
    char c[54];
    fread(c, sizeof(char), 54, from);
    unsigned int size = ceil((width * height * 3) / 8);

    char* buffer = (char*) malloc(sizeof(char) * size);

    if(buffer == NULL){
        printf("Problem with memory allocation!\n");
        exit(-1);
    }
    
    int buffer_index = 0, no_of_bits = 0;

    BGR aux;


    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){

            fread(&aux, sizeof(BGR), 1, from);

            if(no_of_bits <= 5){

                buffer[buffer_index] <<= 1;
                buffer[buffer_index] |= aux.b & 1;

                buffer[buffer_index] <<= 1;
                buffer[buffer_index] |= aux.g & 1;

                buffer[buffer_index] <<= 1;
                buffer[buffer_index] |= aux.r & 1;

                no_of_bits += 3;
                
                if(buffer[buffer_index] == 127){
                    fwrite(buffer, sizeof(char), buffer_index-1, to);  
                    printf("Done\n");
                    return;
                }

                if(buffer_index != size-1){
                    if(no_of_bits == 8){
                        buffer_index ++;
                        buffer[buffer_index] = 0;
                        // clear_buffer(buffer);
                        no_of_bits = 0;
                    }
                }
            }

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

                buffer[buffer_index] |= aux.r & 1;

                no_of_bits = 1;
            }
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

                buffer[buffer_index] |= aux.g & 1;

                buffer[buffer_index] <<= 1;
                buffer[buffer_index] |= aux.r & 1;
                no_of_bits = 2;
            }

        }
    }              

    // if there is limited space
    fwrite(buffer, sizeof(char), buffer_index, to);  

    fclose(tmp);
    fclose(from);
    fclose(to);
}


int main(){

    unsigned int no_of_blocks = 100;
    unsigned int data_size = no_of_blocks * no_of_blocks * 3;

    char message[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.\nLorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.\n";

    BMP_header* h1 = BMP_header_init(data_size);
    DIB_header* d1 = DIB_header_init(no_of_blocks, no_of_blocks, data_size);

    BMP_header_write_to_file("test.bmp", h1);
    DIB_header_write_to_file("test.bmp", d1);
    
    pattern2("test.bmp", no_of_blocks);

    lsb_method_write("test.bmp", "test2.bmp", message);
    lsb_method_read("test2.bmp", "decoded.bin");

    free(h1);
    free(d1);

    return 0;

}
