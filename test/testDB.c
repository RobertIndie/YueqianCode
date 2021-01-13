#include "../lib/db.h"
#include <string.h>

int check_point = 1;

char *test_str = "15 25 7 9 end";

void TestFail()
{
    LOG("Test failed.\n");
    exit(-1);
}

// block to read user torch pos
int GetTorchPos(struct Vector *vector)
{
    int consumed = 0;
    usleep(500 * 1000);
    int r = sscanf(test_str, "%d %d%n", &vector->x, &vector->y, &consumed);
    if (r <= 0)
    {
        if (!check_point)
            LOG("Test passed.\n");
        return -1;
    }
    test_str += consumed;
    LOG("[Touch]Get pos:(%d,%d)\n", vector->x, vector->y);
    return 0;
}

int lcd_show_bmp(char *bmp_name)
{
    LOG("Render %s.\n", bmp_name);
    return 0;
}

int ctrl_led(int led, char isOn)
{
    static int state = 0;
    switch (state)
    {
    case 0:
        if (led != 0 || isOn != 1)
        {
            LOG("Test led fail.");
            TestFail();
        }
        break;

    default:
        TestFail();
        break;
    }
    state++;
    check_point--;
}

int main()
{
    struct Controller *controller = ConfigLoad("config.txt");
    int ret = Run(controller);
    LOG("Tests passed.\n");
    return ret;
}