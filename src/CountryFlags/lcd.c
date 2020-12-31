#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>

int lcd_show_color(unsigned int color)
{
	int fd_lcd;
	int *fb_base = NULL;//Ӧ�ó����У�ָ���Դ��׵�ַ��ָ��
	//1.��Һ�������豸�ļ�
	fd_lcd = open("/dev/fb0", O_RDWR);
	if(fd_lcd == -1)
	{
		perror("open lcd error");
		return -1;
	}
	//2.�Դ��ַӳ��--�������������Դ�ĵ�ַӳ�䵽Ӧ�ó�����
	fb_base = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED,  fd_lcd, 0);
	if(fb_base == NULL)
	{
		printf("fb mmap error\n");
		return -2;
	}
	//3.ͨ���Դ�ĵ�ַ����ɫ����д���Դ�
	for(int i=0;i<800*480;i++)
		*(fb_base+i) = color;	
	
	//4.����Դ��ӳ��
	munmap(fb_base, 800*480*4);
	
	//5.�ر�Һ����
	close(fd_lcd);
	
	return 0;	
}

int lcd_show_bmp(char *bmp_name)
{
	int fd_lcd, fd_bmp;
	int *fb_base = NULL;//Ӧ�ó����У�ָ���Դ��׵�ַ��ָ��
	char bmp_buf[800*480*3];//1152000 ----- bmpͼƬ
	int lcd_buf[800*480];//800*480*4 ---�Դ�
	//int buf[800*480];
	//1. open()��ͼƬ�ļ�
	fd_bmp = open(bmp_name, O_RDONLY);
	if(fd_lcd == -1)
	{
		perror("open bmp error");
		return -1;
	}

	//2. lseek()�ƶ�ͼƬ�ļ���ָ��54B���ļ�ָ��ָ��ͼ���������
	lseek(fd_bmp, 54, SEEK_SET);
	
	//3. read()��ȡͼƬ������
	read(fd_bmp, bmp_buf, sizeof(bmp_buf));//800*480*3
		
	//4. close()�ر�ͼƬ�ļ�
	close(fd_bmp);

	//5. ��bmpÿ�����ص�3���ֽڵ�����ת�����Դ�ÿ�����ص�4���ֽڵ�����
	for(int i=0;i<800*480;i++)
		lcd_buf[i] = (0x00<<23) + (bmp_buf[3*i+2]<<16) +  (bmp_buf[3*i+1]<<8) + (bmp_buf[3*i]<<0);

	
	
	//7.��Һ�������豸�ļ�
	fd_lcd = open("/dev/fb0", O_RDWR);
	if(fd_lcd == -1)
	{
		perror("open lcd error");
		return -1;
	}
	//8.�Դ��ַӳ��--�������������Դ�ĵ�ַӳ�䵽Ӧ�ó�����
	fb_base = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED,  fd_lcd, 0);
	if(fb_base == NULL)
	{
		printf("fb mmap error\n");
		return -2;
	}
	//9.ͨ���Դ�ĵ�ַ����ɫ����д���Դ�
	//for(int i=0;i<800*480;i++)
	//	*(fb_base+i) = buf[i];	
	//6. ��ͼƬ�������·�ת
	//ͬѧ���Լ���
	for(int j=0;j<480;j++){
		int k=479-j;
		for(int i=0;i<800;i++){
			*(fb_base+i+k*800)=lcd_buf[i+j*800];

		}
	}
	
	//10.����Դ��ӳ��
	munmap(fb_base, 800*480*4);
	
	//11.�ر�Һ����
	close(fd_lcd);
	
	return 0;	
}