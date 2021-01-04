#include <stdio.h>
#include <unistd.h>
#include "../../lib/lib.h"

int main(void)
{
	while(1)
	{
		lcd_show_bmp("main.bmp");
	}
	return 0;
}