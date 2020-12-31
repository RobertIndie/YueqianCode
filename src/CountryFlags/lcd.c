#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>

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

int lcd_show_bmp(char *bmp_name)
{
	int fd_lcd, fd_bmp;
	int *fb_base = NULL;//应用程序中，指向显存首地址的指针
	char bmp_buf[800*480*3];//1152000 ----- bmp图片
	int lcd_buf[800*480];//800*480*4 ---显存
	//int buf[800*480];
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
	read(fd_bmp, bmp_buf, sizeof(bmp_buf));//800*480*3
		
	//4. close()关闭图片文件
	close(fd_bmp);

	//5. 将bmp每个像素点3个字节的数据转换成显存每个像素点4个字节的数据
	for(int i=0;i<800*480;i++)
		lcd_buf[i] = (0x00<<23) + (bmp_buf[3*i+2]<<16) +  (bmp_buf[3*i+1]<<8) + (bmp_buf[3*i]<<0);

	
	
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
	//for(int i=0;i<800*480;i++)
	//	*(fb_base+i) = buf[i];	
	//6. 将图片数据上下翻转
	//同学们自己做
	for(int j=0;j<480;j++){
		int k=479-j;
		for(int i=0;i<800;i++){
			*(fb_base+i+k*800)=lcd_buf[i+j*800];

		}
	}
	
	//10.解除显存的映射
	munmap(fb_base, 800*480*4);
	
	//11.关闭液晶屏
	close(fd_lcd);
	
	return 0;	
}