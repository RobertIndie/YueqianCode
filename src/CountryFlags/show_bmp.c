#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include "../../lib/lib.h"
#include <stdlib.h>

int lcd_show_color(unsigned int color)
{
	int fd_lcd;
	int *fb_base = NULL;//应用程序中，指向显存首地址的指针
	//1.打开液晶屏的设备文件
	fd_lcd = open("/dev/fb0", O_RDWR);
	if(fd_lcd == -1)
	{
		perror("open lcd error");
		return -1;
	}
	//2.显存地址映射--将驱动程序中显存的地址映射到应用程序中
	fb_base = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED,  fd_lcd, 0);
	if(fb_base == NULL)
	{
		printf("fb mmap error\n");
		return -2;
	}
	//3.通过显存的地址将颜色数据写入显存
	for(int i=0;i<800*480;i++)
		*(fb_base+i) = color;	
	
	//4.解除显存的映射
	munmap(fb_base, 800*480*4);
	
	//5.关闭液晶屏
	close(fd_lcd);
	
	return 0;	
}

int lcd_show_bmp_xy(int x, int y, int w, int h,char *bmp_name)
{
	int fd_lcd, fd_bmp;
	int *fb_base = NULL;//应用程序中，指向显存首地址的指针
	//char bmp_buf[800*480*3];//1152000 ----- bmp图片
    size_t bmp_size = w*h*3;
    char* bmp_buf = (char*)malloc(bmp_size);
	//int lcd_buf[800*480];//800*480*4 ---显存
	int* lcd_buf = (int *)malloc(sizeof(int)*w*h);
    //1. open()打开图片文件
	fd_bmp = open(bmp_name, O_RDONLY);
	if(fd_lcd == -1)
	{
		perror("open bmp error");
		return -1;
	}

	//2. lseek()移动图片文件的指针54B，文件指针指向图像的数据区
	lseek(fd_bmp, 54, SEEK_SET);
	
	//3. read()读取图片的数据
	read(fd_bmp, bmp_buf, bmp_size);//w*h*3
	
	//4. close()关闭图片文件
	close(fd_bmp);

	//5. 将bmp每个像素点3个字节的数据转换成显存每个像素点4个字节的数据
	for(int i=0;i<w*h;i++)
		lcd_buf[i] = (0x00<<23) + (bmp_buf[3*i+2]<<16) +  (bmp_buf[3*i+1]<<8) + (bmp_buf[3*i]<<0);
	//6. 将图片数据上下翻转
	//同学们自己做
	
	
	
	//7.打开液晶屏的设备文件
	fd_lcd = open("/dev/fb0", O_RDWR);
	if(fd_lcd == -1)
	{
		perror("open lcd error");
		return -1;
	}
	//8.显存地址映射--将驱动程序中显存的地址映射到应用程序中
	fb_base = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED,  fd_lcd, 0);
	if(fb_base == NULL)
	{
		printf("fb mmap error\n");
		return -2;
	}
	//9.通过显存的地址将颜色数据写入显存
    if(x+w*800>800*480||y+h*480>800*480)
    {
        printf("error");
    }
    for(int i=0;i<800*480;i++){
        *(fb_base+i)=0x00FFFFFF;
    }
    int p=0;
    for(int j=y;j<y+h;j++)
    { 
        for(int i=x;i<x+w;i++)
        { 
            *(fb_base+j*800+i) = lcd_buf[(h-(j-y)-1)*w+i-x];
        }
    }
    //printf("Debug %d\n",p);

	//10.解除显存的映射
	munmap(fb_base, 800*480*4);
	
	//11.关闭液晶屏
	close(fd_lcd);
    free(bmp_buf);
    free(lcd_buf);
	
	return 0;	
}
int main(void)
{
    lcd_show_bmp_xy(100, 100, 200, 120, "sp.bmp"); // w->h h->w
	return 0;
}