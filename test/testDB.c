#include "../lib/db.h"
#include <string.h>

int check_point = 2;

char *test_str = "15 25 7 9 15 25 end";

void TestFail()
{
    LOG("Test failed.\n");
    exit(-1);
}

// block to read user torch pos
int GetTorchPos(struct Vector *vector)
{
    int consumed = 0;
    sleep(1);
    int r = sscanf(test_str, "%d %d%n", &vector->x, &vector->y, &consumed);
    if (r <= 0)
    {
       sleep(1000);
    }
    test_str += consumed;
    LOG("[Touch]Get pos:(%d,%d)\n", vector->x, vector->y);
    return 0;
}

int get_key(char *keyStatus){
    for(int i=0;i<4;i++)keyStatus[i]=1;
    static char flag = 0;
    if(flag==1){
        keyStatus[0]=1;
        usleep(1000*100);
        return 0;
    }
    flag = 1;
    keyStatus[0]=0;
    return 0;
}

int lcd_show_bmp(char *bmp_name)
{
    //LOG("Render %s.\n", bmp_name);
    return 0;
}

int ctrl_led(int led, char isOn)
{
    LOG("LED %d status: %d\n",led,isOn);
}

int lcd_test(){
    while(1){
        LOG("Show pic 1.\n");
        sleep(3);
        LOG("Show pic 2.\n");
        sleep(3);
        LOG("Show pic 3.\n");
        sleep(3);
    }
}

int main()
{
    struct Controller *controller = ConfigLoad("config.txt");
    int ret = Run(controller);
    LOG("Tests passed.\n");
    return ret;
}