#ifndef DB
#define DB
#define DB_VERSION 1

#include <stdio.h>
#include <stdlib.h>
#define LOG printf

#define ResultOK 0

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>

int lcd_show_bmp(char *bmp_name);
int ctrl_led(int led, char isOn);

struct Vector
{
    int x;
    int y;
};

struct Rectangle
{
    struct Vector lt;
    struct Vector rd;
};

struct BMP
{
    int *data;
    int size;
};

struct LCD
{
    int fd;
    int *base;
    int size;
};

// 读取bmp文件，将为其分配内存
int ReadBMP(char *name, struct BMP **ptrBMP);
// 释放BMP及内存
int FreeBMP(struct BMP **ptrBMP);
int InitLCD(struct LCD **lcd);
int ShowBMP(struct LCD *lcd, struct BMP *bmp);
int CloseLCD(struct LCD **lcd);

enum WorkMode
{
    None,     // n
    Redirect, // r
    LED       // l TODO replace LED with an more abstrctly work mode
};

struct WorkModeRedirect
{
    struct Page *pointToPage;
};

struct WorkModeLED
{
    int ledIndex;
    char ledState;
};

union WorkModeUnion
{
    struct WorkModeRedirect redirect;
    struct WorkModeLED led;
};

struct Button
{
    enum WorkMode mode;
    struct Rectangle rect;
    union WorkModeUnion modeParam;
};

void InitButton(struct Button *button);

#define DEBUG_BUTTON(button) \
    LOG("[DEBUG][Button](%d,%d)(%d,%d)\n", button.rect.lt.x, button.rect.lt.y, button.rect.rd.x, button.rect.rd.y);

struct Page
{
    char bgPath[10]; // debug use
    struct BMP *background;
    struct Button *buttons;
    int buttonsCount;
};

// 获取触摸屏的触摸位置，将阻塞等待用户输入
int GetTorchPos(struct Vector *vector);

struct Controller
{
    struct Page *pagesList;
    int pagesListSize;
    struct Page *currentPage;

    char isStop;
    pthread_t touch_thread;
    pthread_mutex_t touch_mutex;
    pthread_cond_t touch_cond;
    struct Vector touch_thread_pos;
};

// 不断地循环检测触摸屏的触摸位置，检查按钮的点击事件，并进行页面跳转
int Run(struct Controller *controller);

// 读取配置文件，并加载生成Controller
struct Controller *ConfigLoad(char *configFilePath);

#endif // DB