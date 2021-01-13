#include "../../lib/db.h"

char *test_str = "15 25 15 25 end";

// 获取触摸屏的触摸位置，将阻塞等待用户输入
int GetTorchPos(struct Vector *result)
{
	char other[32];
	int r = sscanf(test_str, "%d %d %[^\n]", &result->x, &result->y, other);
	if (r <= 1)
	{
		exit(0);
	}
	test_str = other;
	LOG("[Touch]Get pos:(%d,%d)\n", result->x, result->y);
	return result;
}

int lcd_show_bmp(char *bmp_name)
{
	LOG("[Controller]Render %s\n", bmp_name);
	return 0;
}

int main()
{
	struct Controller *controller = ConfigLoad("config.txt");
	return Run(controller);
}