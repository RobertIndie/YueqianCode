#include "../lib/db.h"
#include <string.h>

int check_point = 2;

char *test_str = "15 25 end";

void TestError()
{
    LOG("Test error.\n");
    exit(-1);
}

// block to read user torch pos
struct Vector GetTorchPos()
{
    struct Vector result;
    int consumed = 0;
    int r = sscanf(test_str, "%d %d%n", &result.x, &result.y, &consumed);
    if (r <= 0)
    {
        if(!check_point)
            LOG("Test passed.\n");
        else
            LOG("Test didn't go through all checkpoints.\n");
        exit(0);
    }
    test_str += consumed;
    LOG("[Touch]Get pos:(%d,%d)\n", result.x, result.y);
    return result;
}

int test_state = 0;

int lcd_show_bmp(char *bmp_name)
{
    switch (test_state)
    {
    case 0:
        if (strcmp(bmp_name, "main.bmp"))
        {
            TestError();
        }
        break;
    case 1:
        if (strcmp(bmp_name, "test.bmp"))
        {
            TestError();
        }
        break;

    default:
        break;
    }
    test_state++;
    check_point--;
    return 0;
}

int main()
{
    struct Controller *controller = ConfigLoad("config.txt");
    int ret = Run(controller);
    LOG("Tests passed.\n");
    return ret;
}