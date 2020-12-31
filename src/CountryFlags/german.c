#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int fd_lcd;

int OpenLCD(){
	fd_lcd = open("/dev/fb0", O_RDWR);
	if(fd_lcd == -1)
	{
		perror("open lcd error");
		return -1;
	}
}

int Render(){
	int lcd_buf[800*480];
	for(int i=0;i<800*480;i++)
		lcd_buf[i] = 0x00000000;
	for(int j=128000;j<800*480;j++)
		lcd_buf[j] = 0x00FF0000;
	for(int k=256000;k<800*480;k++)
		lcd_buf[k] = 0x00FFFF00;
	write(fd_lcd, lcd_buf, 800*480*4);
}

int CloseLCD(){
	close(fd_lcd);
}


int main(){
	OpenLCD();
		Render();
	CloseLCD();
}