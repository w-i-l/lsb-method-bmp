#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "Headers/bmp.h"
#include "Headers/patterns.h"

void menu(){

    FILE* from, *to;
    char from_file[200], to_file[200], aux[100];
    BMP_header* bmp_h;
    DIB_header* dib_h;
    unsigned int no_of_blocks, data_size;
    int option;
    // array of patterns functions 
    void (*patterns[3]) (const char*, unsigned int) = {pattern1, pattern2, pattern3};
    char* message = NULL;

    while(1){
        printf("\t\tMENU\n");
        printf("1. Create a BMP from a pattern\n");
        printf("2. Encode a message\n");
        printf("3. Decode a message from a BMP\n");
        printf("Choose an option: ");

        scanf("%d", &option);

        system("cls");
        printf("option: %d\n", option);

        if(option == 1){
            first:printf("Choose a pattern(1-3): ");

            scanf("%d", &option);

            if(1 <= option && option <= 3){

                printf("Enter the amount of pixels per one line: ");
                scanf("%d", &no_of_blocks);

                printf("\nEnter the filename: ");
                scanf(" %s", to_file);

                data_size = no_of_blocks * no_of_blocks * 3;
                
                bmp_h = BMP_header_init(data_size);
                dib_h = DIB_header_init(no_of_blocks, no_of_blocks, data_size);

                BMP_header_write_to_file(to_file, bmp_h);
                DIB_header_write_to_file(to_file, dib_h);

                (*(patterns[option - 1])) (to_file, no_of_blocks);

                printf("Image %s created!\n", to_file);

            }
            else{
                printf("Enter a correct option!\n");
                goto first;
            }
        }
        else if(option == 2){
            printf("Enter the input filename:");
            fscanf(stdin, "%s", from_file);

            printf("Enter the output filename:");
            fscanf(stdin, "%s", to_file);

            printf("Enter the source filename for message:");
            fscanf(stdin, "%s", aux);

            from = fopen(aux, "r");

            if(from == NULL){
                printf("Can't open %s\n", aux);
                exit(-1);
            }

            fseek(from, 0, SEEK_END);
            int dim = ftell(from);

            fseek(from, 0, SEEK_SET);

            message = (char*)malloc(sizeof(char) * (dim+1));

            fread(message, sizeof(char), dim, from);
            message[dim] = 0;

            lsb_method_write(from_file, to_file, message);

            printf("Done encrypting\n");

            free(message);
            fclose(from);
        }
        else if(option == 3){

            printf("Enter the input filename:");
            scanf("%s", from_file);

            printf("Enter the output filename:");
            scanf("%s", to_file);

            lsb_method_read(from_file, to_file);

            printf("Decoded the file\n");

        }
        else{
            printf("Enter a valid option!\n");
        }
    }


}


int main(){

    menu();

    return 0;
}
