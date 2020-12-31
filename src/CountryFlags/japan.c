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

int Render(){
	fb_base = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED,  fd_lcd, 0);
	int x=400;
	int y=240;
	int r=150;
	for(int i=0;i<800;i++){
		for(int j=0;j<480;j++){
			double d = sqrt(pow(i-x,2)+pow(j-y,2));
			if(d<r){
				*(fb_base+j*800+i) = 0x00FF0000;
			}else{
				*(fb_base+j*800+i) = 0x00FFFFFF;
			}
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