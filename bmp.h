#ifndef BMP_FILE_INCLUDED
#define BMP_FILE_INCLUDED

typedef struct{
    unsigned char b, g, r;
}BGR;

BGR* BGR_init(unsigned char, unsigned char, unsigned char);
void BGR_write_to_file(FILE*, BGR*);

typedef struct{

    unsigned short file_type;
    unsigned int file_size;
    unsigned int empty_space; // empty space
    unsigned int offset;

}BMP_header;

BMP_header* BMP_header_init(unsigned int);
void BMP_header_display(BMP_header*);
void BMP_header_write_to_file(const char*, BMP_header*);


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

DIB_header* DIB_header_init(unsigned int, unsigned int, unsigned int);
void DIB_heaader_display(DIB_header*);
void DIB_header_write_to_file(const char*, DIB_header*);



#endif
