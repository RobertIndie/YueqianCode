#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <math.h>

int fd_lcd;
int* fb_base;

int OpenLCD(){
	fd_lcd = open("/dev/fb0", O_RDWR);
	if(fd_lcd == -1)
	{
		perror("open lcd error");
		return -1;
	}
}

int lcd_show_color_xy(int x, int y, int w, int h,int color)
{
	fb_base = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED,  fd_lcd, 0);
	if (x+w>800||y+h>480||x<0||y<0||x+w<0||y+h<0)
	{
		printf("error");
		return -1;
	}
	for(int i=x;i<x+w;i++){
		for(int j=y;j<y+h;j++){
				*(fb_base+j*800+i) = color;
		}
	}
}

int CloseLCD(){
	munmap(fb_base, 800*480*4);
	close(fd_lcd);
}


int main(){
	OpenLCD();
	lcd_show_color_xy(0,0,800,480, 0x00FFFFFF);// Clear canvas
	lcd_show_color_xy(200,300,100,100, 0x00FF0000);
	CloseLCD();
}