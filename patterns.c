#include <stdlib.h>
#include <stdio.h>
#include "Headers/patterns.h"
#include "Headers/bmp.h"

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

    fclose(f);

}


