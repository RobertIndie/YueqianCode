#include "../lib/db.h"
#include <string.h>

int check_point = 3;

char *test_str = "15 25 7 9 end";

void TestFail()
{
    LOG("Test failed.\n");
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

int lcd_show_bmp(char *bmp_name)
{
    static int test_state = 0;
    switch (test_state)
    {
    case 0:
        if (strcmp(bmp_name, "main.bmp"))
        {
            TestFail();
        }
        break;
    case 1:
        if (strcmp(bmp_name, "test.bmp"))
        {
            TestFail();
        }
        break;

    default:
        return 0;
    }
    test_state++;
    check_point--;
    return 0;
}

int ctrl_led(int led, char isOn){
    static int state = 0;
    switch (state)
    {
    case 0:
        if(led!=0||isOn!=1){
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