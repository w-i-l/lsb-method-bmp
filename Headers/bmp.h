#ifndef BMP_FILE_INCLUDED
#define BMP_FILE_INCLUDED

typedef struct{
    unsigned char b, g, r;
}BGR;

BGR* BGR_init(unsigned char b, unsigned char g, unsigned char r);
void BGR_write_to_file(FILE* f, BGR* coppy);

typedef struct{

    unsigned short file_type;
    unsigned int file_size;
    unsigned int empty_space; // empty space
    unsigned int offset;

}BMP_header;

BMP_header* BMP_header_init(unsigned int bmp_data);
void BMP_header_display(BMP_header* coppy);
void BMP_header_write_to_file(const char* filename, BMP_header* coppy);


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

DIB_header* DIB_header_init(unsigned int width, unsigned int height, unsigned int data_size);
void DIB_heaader_display(DIB_header* coppy);
void DIB_header_write_to_file(const char* filename, DIB_header* coppy);


void lsb_method_write(const char* from_file, const char* to_file, char* message);
void lsb_method_read(const char* from_file, const char* to_file);

#endif
