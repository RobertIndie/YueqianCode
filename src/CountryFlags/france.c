#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include "lcd.h"

int fd_lcd;
int *fb_base = NULL;

int OpenLCD(){
	fd_lcd = open("/dev/fb0", O_RDWR);
	if(fd_lcd == -1)
	{
		perror("open lcd error");
		return -1;
	}
}

int Render(){
	fb_base = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED,  fd_lcd, 0);
	if(fb_base == NULL)
	{
		printf("fb mmap error\n");
		return -2;
	}
	for(int i=0;i<800;i++){
		for(int j=0;j<480;j++){
			int color = 0;
			if(i<800/3)color=BLUE;
			else if(i<800*2/3)color=WHITE;
			else color=RED;
			*(fb_base+j*800+i)=color;
		}
	}	
	
}

int CloseLCD(){
	
	munmap(fb_base, 800*480*4);
	close(fd_lcd);
}


int main(){
	OpenLCD();
	Render();
	CloseLCD();
}