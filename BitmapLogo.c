#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <stdlib.h>


/* 
Due to padding, the Struct allocates the size of highest member times number of members in the Struct [Hence though logically, we may think it is
14 bytes, C allocates 16 bytes]

This issue is handled by avoiding the Structure padding, this is forced using __attribute__((packed))
*/

typedef struct {
    char name[2];
    unsigned int size;
    int garbage;
    unsigned int image_offset;
    unsigned int dib_header;
    unsigned int width;
    unsigned int height;
    short no_of_color_planes;
    short bits_per_pixel;
    unsigned int pixel_array_compression;
    unsigned int raw_bitmap_size;
    unsigned int x_pixel_per_meter;
    unsigned int y_pixel_per_meter;
    unsigned int no_colors_palattes;
    unsigned int important_colors;
    char data[24];
} __attribute__((packed)) Bitmap_Header;



int main(){
	// Frame Buffer Mapping
	int frame; 	//FB File Descriptor
	int image; 	//BMP File Descriptor
	int ret; 	//IOCTL Return Value
	int ret2;	//IOCTL Return Values
	struct fb_var_screeninfo temp; 		//Variable Screen Info
	struct fb_fix_screeninfo screen;	//Fixed Screen Info
	struct stat fileinfo;				//File Info (fstat)
	int resolution; 
	int fb_resolution; 	//Frame Buffer Resolution
	int bmp_resolution;	//BMP Resolution
	
	int val;
    ssize_t read_data; 
    Bitmap_Header head; //Struct
	
	int line_length;
	char* fb_buffer;
	char* bmp_buffer;

	unsigned char* bmp;

	char r, g, b, a;

	frame = open("/dev/fb0",O_RDWR);

	if (frame < 0){
		perror("Opening Frame Buffer Failed! \n");
		exit(0);
	}

	ret = ioctl(frame, FBIOGET_VSCREENINFO, &temp);

	if (ret < 0){
		perror("IOCTL Failed! \n");
		exit(0);
	}

	printf("xres: %d, yres: %d, bits per pixed: %d \n", temp.xres, temp.yres, temp.bits_per_pixel);

	ret2 = ioctl(frame, FBIOGET_FSCREENINFO, &screen);

	if (ret2 < 0){
		perror("IOCTL Failed on Second thing! \n");
		exit(0);
	}

	printf("The Line Length is %d \n", screen.line_length);
	
	resolution = temp.xres * temp.yres * (temp.bits_per_pixel / 8);

	line_length = screen.line_length * temp.yres;

	fb_resolution = temp.xres * temp.yres * temp.bits_per_pixel;

	printf("The Resolution is %d \n", resolution);
	printf("The lenght is %d \n", line_length);

	fb_buffer = (unsigned char*)mmap(0, line_length, PROT_READ|PROT_WRITE, MAP_SHARED, frame, 0);

	// BMP Image Mapping

	val = open("RAY.BMP", O_RDONLY);

	fstat(val, &fileinfo);

    if(val < 0){
        perror("Opening the file failed! \n");
        exit(0);
    }

    read_data =  read(val, &head, sizeof(Bitmap_Header));

    printf("The type of BMP is %c%c \n", head.name[0], head.name[1]);
    printf("The Size of the BMP file is %d \n", head.size);
    printf("The Offset is %d \n", head.image_offset);
    printf("The DIB header size is %d \n", head.dib_header);
    printf("The width is %d px and height is %d px\n", head.width, head.height);
    printf("The no of color planes is %d \n", head.no_of_color_planes);
    printf("Bits Per Pixel is %d \n", head.bits_per_pixel);
    printf("The x pixel per meter is %d and y pixel per meter is %d\n", head.x_pixel_per_meter, head.y_pixel_per_meter);
    printf("The header struct size is %ld \n", sizeof(Bitmap_Header));
	
	bmp_resolution = head.width * head.height * head.bits_per_pixel;

	bmp_buffer = (unsigned char*)mmap(0, fileinfo.st_size, PROT_READ, MAP_SHARED, val, 0);
	
	// Setting Image Position
	bmp = bmp_buffer + 54;
	
	int used_line_length = screen.line_length/4;
	
	printf("The Frame Buffer Resolution is %ld \n", fb_resolution);
	printf("The BMP Image Resolution is %ld \n", bmp_resolution);

	//Comparing BMP resoultion with Frame Buffer Resolution

	if(bmp_resolution < fb_resolution){
		printf("Perfect! Apt Requirement\n");

		for(int i= head.height-1; i>=0; i--){
		for(int j =0; j<head.width; j++){
			r = bmp[0];
			g = bmp[1];
			b = bmp[2];
			a = bmp[3];
			bmp += 4;
			fb_buffer[i*used_line_length+j] = (r|(g<<8)|(b<<16)|(a<<24));
		}
	}
	}

	

	munmap(fb_buffer, line_length);
	munmap(bmp_buffer, fileinfo.st_size);
	close(frame);
	close(val);
	return 0;
}
