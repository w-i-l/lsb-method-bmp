#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bmp.h"


#define size 456


void pattern1(FILE* f, unsigned int no_of_blocks){

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

}


void pattern2(FILE* f, unsigned int no_of_blocks){
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
}


void lsb_method_write(FILE* from, FILE* to, char* message){

    FILE* tmp = fopen("tmp.txt", "w");

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

    printf("%i %i %i\n", offset, width, height);

    BGR aux;
    char bit;

    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){
            fread(&aux, sizeof(BGR), 1, from);
            // printf("%c", message[index]);
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
                // fprintf(tmp, "%c %c %c\n", (aux.b & 1) + '0', (aux.g & 1) + '0', (aux.r & 1) + '0');
            }
            fwrite(&aux, sizeof(BGR), 1, to);
        }
    }
    fclose(tmp);

}


void lsb_method_read(FILE* from){

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

    // fwrite(buffer, sizeof(char), 54, to); 
    // printf("%d %d %d %ld\n", offset, width, height, ftell(from));

    // const int size = 100;
    char buffer[size+1] = {0}, current_char;
    int buffer_index = 0, no_of_bits = 0;

    BGR aux;

    // printf("debug\n");

    for(int i=0; i<width; i++){
        for(int j=0; j<height; j++){


            fread(&aux, sizeof(BGR), 1, from);
            // printf("%ld\n", ftell(from));

            if(no_of_bits <= 5){

                buffer[buffer_index] <<= 1;
                buffer[buffer_index] |= aux.b & 1;

                buffer[buffer_index] <<= 1;
                buffer[buffer_index] |= aux.g & 1;

                buffer[buffer_index] <<= 1;
                buffer[buffer_index] |= aux.r & 1;

                no_of_bits += 3;

                if(buffer_index != size-1){
                    if(no_of_bits == 8){
                        buffer_index ++;
                        buffer[buffer_index] = 0;
                        no_of_bits = 0;
                    }
                }
                else{
                    buffer[buffer_index+1] = 0;
                    printf("%s", buffer);
                    buffer_index = 0;
                    buffer[buffer_index] = 0;
                    no_of_bits = 0;
                }
            }
            else if(no_of_bits == 6){
                buffer[buffer_index] <<= 1;
                buffer[buffer_index] |= aux.b & 1;

                buffer[buffer_index] <<= 1;
                buffer[buffer_index] |= aux.g & 1;

                if(buffer_index != size-1){
                        buffer_index ++;
                }
                else{
                    buffer[buffer_index + 1] = 0;
                    printf("%s", buffer);
                    buffer_index = 0;
                }

                buffer[buffer_index] = 0;


                buffer[buffer_index] <<= 1;
                buffer[buffer_index] |= aux.r & 1;

                no_of_bits = 1;
            }
            else if(no_of_bits == 7){
                buffer[buffer_index] <<= 1;
                buffer[buffer_index] |= aux.b & 1;

                if(buffer_index != size-1){
                    buffer_index ++;
                }
                else{
                    buffer[buffer_index + 1] = 0;
                    printf("%s", buffer);
                    buffer_index = 0;
                }
                buffer[buffer_index] = 0;

                
                buffer[buffer_index] <<= 1;
                buffer[buffer_index] |= aux.g & 1;

                buffer[buffer_index] <<= 1;
                buffer[buffer_index] |= aux.r & 1;

                no_of_bits = 2;
            }

            fprintf(tmp, "%d %d %d\n", aux.b & 1, aux.g & 1, aux.r & 1);
        }
    }
    fclose(tmp);
}


int main(){
    
    FILE* f = fopen("img.bmp", "r+b");
    // FILE* file = fopen("img.bmp", "wb");
    FILE* g = fopen("out.bmp", "r+b");
    FILE* h = fopen("pointillist.bmp", "r+b");

    unsigned int no_of_blocks = 1000;
    unsigned int data_size = no_of_blocks * no_of_blocks * 3;

    char message[] = "am fost la ana si cornel sa aflu si eu ce s-a mai vandut am fost la ana si cornel sa aflu si eu ce s-a mai vandut am fost la ana si cornel sa aflu si eu ce s-a mai vandut am fost la ana si cornel sa aflu si eu ce s-a mai vandutam fost la ana si cornel sa aflu si eu ce s-a mai vandut am fost la ana si cornel sa aflu si eu ce s-a mai vandut am fost la ana si cornel sa aflu si eu ce s-a mai vandut am fost la ana si cornel sa aflu si eu ce s-a mai vandut";


    // lsb_method_write(f, g, message);
    lsb_method_read(g);


    // clock_t start = clock();

    // BMP_header* h1 = BMP_header_init(data_size);
    // DIB_header* d1 = DIB_header_init(no_of_blocks, no_of_blocks, data_size);

    // BMP_header_write_to_file(f, h1);
    // DIB_header_write_to_file(f, d1);
    
    // pattern1(f, no_of_blocks);

    

    // // printf("Initialisation: %lf\n", (clock() - start) / (double)CLOCKS_PER_SEC);

    // // start = clock();


    // printf("Writing: %lfs\n", (double)(clock() - start) / CLOCKS_PER_SEC);

    // free(h1);
    // free(d1);

    // fclose(f);

    return 0;

}
