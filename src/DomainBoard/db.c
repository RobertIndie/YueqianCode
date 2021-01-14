#include "../../lib/db.h"
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>

int ReadBMP(char *name, struct BMP **ptrBMP)
{
    LOG("Load BMP: %s\n", name);
    struct BMP *result = (struct BMP *)malloc(sizeof(struct BMP));
    result->size = 1;
    result->data = (int *)malloc(sizeof(int) * result->size);
    for (int i = 0; i < result->size; i++)
    {
        result->data[i] = 5; // fake data
    }
    *ptrBMP = result;
    return ResultOK;
}

int FreeBMP(struct BMP **ptrBMP)
{
    struct BMP *bmp = *ptrBMP;
    free(bmp->data);
    free(bmp);
    *ptrBMP = NULL;
    return ResultOK;
}

int InitLCD(struct LCD **lcd)
{
    struct LCD *result = (struct LCD *)malloc(sizeof(struct LCD));
    result->fd = 0;
    result->size = 10;
    result->base = (int *)malloc(sizeof(int) * result->size);
    for (int i = 0; i < result->size; i++)
    {
        result->base[i] = i; // fake data
    }
    *lcd = result;
    return 0;
}

int ShowBMP(struct LCD *lcd, struct BMP *bmp)
{
    for (int i = 0; i < bmp->size; i++)
    {
        lcd->base[i] = bmp->data[i];
    }
    return 0;
}

int CloseLCD(struct LCD **lcd)
{
    struct LCD *l = *lcd;
    free(l->base);
    free(l);
    *lcd = NULL;
    return 0;
}

void InitButton(struct Button *button)
{
}

void *keyStatusThread(void *param)
{
    struct Button* b = (struct Button*)param;
    while (!b->modeParam.key.isStop)
    {
        char keyBuff[4];
        get_key(keyBuff);
        for (int i = 0; i < sizeof(keyBuff); i++)
        {
            ctrl_led(i, !keyBuff);
        }
    }
}

void *LCDThread(void *param){
    lcd_test();
}

void *AlbumThread(void *param){
    struct Button* b = (struct Button*)param;
    while(1){
        lcd_show_bmp(b->modeParam.album.photoName[b->modeParam.album.currentPhoto]);
        b->modeParam.album.currentPhoto = (b->modeParam.album.currentPhoto + 1)%b->modeParam.album.size;
    }
}

int ButtonInitEvent(struct Controller *controller, struct Button *button)
{
    switch (button->mode)
    {
    case KeyStatus:
        pthread_create(&button->modeParam.key.thread, NULL, keyStatusThread, button);
        break;
    case LCD:
        pthread_create(&button->modeParam.lcd.thread, NULL ,LCDThread, button);
        break;
    case Album:
        pthread_create(&button->modeParam.album.thread,NULL,AlbumThread,button);
        break;
    default:

        break;
    }
    return 0;
}

int ButtonExitEvent(struct Controller *controller, struct Button *button)
{
    switch (button->mode)
    {
    case KeyStatus:
        button->modeParam.key.isStop=1;
        pthread_join(&button->modeParam.key.thread, NULL);
        break;
    case LCD:
        pthread_cancel(&button->modeParam.lcd.thread);
        break;
    case Album:
        pthread_cancel(&button->modeParam.album.thread);
        break;
    default:
        break;
    }
    return 0;
}

int InitPage(struct Controller *controller, struct Page *page)
{
    for (int i = 0; i < page->buttonsCount; i++)
    {
        struct Button *b = page->buttons + i;
        ButtonInitEvent(controller, b);
    }
}

int ExitPage(struct Controller *controller, struct Page *page)
{
    for (int i = 0; i < page->buttonsCount; i++)
    {
        struct Button *b = page->buttons + i;
        ButtonExitEvent(controller, b);
    }
}

int ButtonClickEvent(struct Controller *controller, struct Button *button)
{
    switch (button->mode)
    {
    case Redirect:
        ExitPage(controller, controller->currentPage);
        controller->currentPage = button->modeParam.redirect.pointToPage;
        InitPage(controller, controller->currentPage);
        LOG("[Controller]Point to page %ld\n", controller->currentPage - controller->pagesList);
        break;
    case LED:
        // LED control
        LOG("LED triggered.\n");
        button->modeParam.led.ledState = !(button->modeParam.led.ledState);
        ctrl_led(button->modeParam.led.ledIndex, button->modeParam.led.ledState);
        break;
    default:
        // None
        break;
    }
    return 0;
}

void *TouchThread(void *param)
{
    struct Controller *controller = (struct Controller *)param;
    while (1)
    {
        struct Vector result;
        int ret = GetTorchPos(&result);
        if (ret == -1)
        {
            controller->isStop = 1;
            //sleep(1);
            //pthread_exit(NULL); // can not exit child thread unti other thread receive the signal.
        }
        pthread_mutex_lock(&controller->touch_mutex);
        controller->touch_thread_pos = result;
        pthread_cond_signal(&controller->touch_cond);
        pthread_mutex_unlock(&controller->touch_mutex);
    }
}

void initController(struct Controller *controller)
{
    controller->isStop = 0;
    pthread_mutex_init(&controller->lock,NULL);
    // pthread_mutex_init(&controller->touch_mutex, NULL);
    // pthread_cond_init(&controller->touch_cond, NULL);
    // int ret = pthread_create(&controller->touch_thread, NULL, TouchThread, (void *)controller);
    // if (ret != 0)
    // {
    //     LOG("Create torch thread error.");
    //     exit(-1);
    // }
}

uint64_t as_usec(struct timeval *ts)
{
    return ts->tv_sec * (uint64_t)1000000L + ts->tv_usec;
}
struct timespec get_time_spec(uint64_t usec)
{
    struct timespec ret = {0, 0};
    ret.tv_nsec = (usec % 1000000L) * 1000;
    ret.tv_sec = usec / 1000000L;
    return ret;
}

int Run(struct Controller *controller)
{
    initController(controller);
    LOG("[Controller]Start running ...\n");
    while (!controller->isStop)
    {
        struct Page *page = controller->currentPage;
        //LOG("[Controller]Current page:%ld\n", page - controller->pagesList);
        lcd_show_bmp(page->bgPath);
        struct Vector pos;
        int ret = GetTorchPos(&pos);
        if (ret == -1)
        {
            controller->isStop = 1;
            break;
        }
        // pthread_mutex_lock(&controller->touch_mutex);
        // struct timeval now;
        // gettimeofday(&now, NULL);
        // struct timespec time_to_wait = get_time_spec(as_usec(&now)+100*1000);
        // int ret = pthread_cond_timedwait(&controller->touch_cond, &controller->touch_mutex, &time_to_wait);
        // char isClick = 0;
        // if (ret == ETIMEDOUT) // when child thread exit until main thread receive the signal, it will always return ETIMEDOUT.
        //     isClick = 0;
        // else{
        //     if(ret!=0){
        //         LOG("timed wait fail: %d\n",ret);
        //         exit(-1);
        //     }
        //     isClick = 1;
        //     pos = controller->touch_thread_pos;
        // }
        // pthread_mutex_unlock(&controller->touch_mutex);

        for (int i = 0; i < page->buttonsCount; i++)
        {
            struct Button *b = page->buttons + i;
            //DEBUG_BUTTON((*b));
            if (pos.x >= b->rect.lt.x && pos.x <= b->rect.rd.x && pos.y >= b->rect.lt.y && pos.y <= b->rect.rd.y)
            {
                ButtonClickEvent(controller, b);
                break;
            }
        }
    }
}

struct Controller *ConfigLoad(char *configFilePath)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int configVersion = 0;

    struct Controller *controller = (struct Controller *)malloc(sizeof(struct Controller));

    fp = fopen(configFilePath, "r");
    if (fp == NULL)
    {
        LOG("Read config failed.\n");
        exit(-1);
    }

#define GET_LINE                                 \
    if ((read = getline(&line, &len, fp)) == -1) \
    {                                            \
        LOG("Error reading config fail.");       \
        exit(-1);                                \
    }

    // get config file version
    int versionMajor, versionMinor, versionMaintenace;
    GET_LINE
    sscanf(line, "%d %d %d", &versionMajor, &versionMinor, &versionMaintenace);
    configVersion = (versionMajor << 16) + (versionMinor << 8) + versionMaintenace;
    LOG("[Config]Config file version:%d.%d.%d|configVersion:%x\n", versionMajor, versionMinor, versionMaintenace, configVersion);

    GET_LINE
    sscanf(line, "%d", &(controller->pagesListSize));
    if (controller->pagesListSize <= 0)
    {
        LOG("[Error]No pages found.\n");
        exit(-1);
    }
    LOG("[Config]pages count: %d\n", controller->pagesListSize);
    controller->pagesList = (struct Page *)malloc(sizeof(struct Page) * controller->pagesListSize);
    controller->currentPage = controller->pagesList;
    for (int i = 0; i < controller->pagesListSize; i++)
    {
        struct Page *page = controller->pagesList + i;
        GET_LINE
        sscanf(line, "%d %s", &(page->buttonsCount), page->bgPath);
        LOG("[Config][Page %d]buttons count: %d | background path: %s\n", i, page->buttonsCount, page->bgPath);
        page->buttons = (struct Button *)malloc(sizeof(struct Button) * page->buttonsCount);
        for (int j = 0; j < page->buttonsCount; j++)
        {
            struct Button *button = page->buttons + j;
            InitButton(button);
            int pointPageIndex = 0;
            char workMode;
            char paramsBuff[1024]; // need more information to store comment, it would be great if we can remove this buff.
            GET_LINE
            sscanf(line, "%d %d %d %d %c %[^\n]",
                   &(button->rect.lt.x),
                   &(button->rect.lt.y),
                   &(button->rect.rd.x),
                   &(button->rect.rd.y),
                   &workMode,
                   paramsBuff);
            switch (workMode)
            {
            case 'r':
                button->mode = Redirect;
                sscanf(paramsBuff, "%d", &pointPageIndex);
                button->modeParam.redirect.pointToPage = controller->pagesList + pointPageIndex;
                break;
            case 'l':
                button->mode = LED;
                sscanf(paramsBuff, "%d", &button->modeParam.led.ledIndex);
                button->modeParam.led.ledState = 0;
                break;
            case 'k':
                button->mode = KeyStatus;
                button->modeParam.key.isStop = 0;
                break;
            case 'c':
                button->mode = LCD;
                break;
            case 'a':
                button->mode = Album;
                sscanf(paramsBuff, "%d", &button->modeParam.album.size);
                button->modeParam.album.currentPhoto = 0;
                for(int i=0;i<button->modeParam.album.size;i++){
                    GET_LINE;
                    sscanf(line, "%s %[^\n]", button->modeParam.album.photoName[i],paramsBuff);
                }
                break;
            default:
                button->mode = None;
                break;
            }
            LOG("[Config][Page %d][Button %d](%d %d %d %d) %d\n",
                i, j,
                (button->rect.lt.x),
                (button->rect.lt.y),
                (button->rect.rd.x),
                (button->rect.rd.y),
                (button->mode));
        }
    }

    fclose(fp);
    if (line)
        free(line);
    return controller;
}