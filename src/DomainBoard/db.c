#include "../../lib/db.h"

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

void InitButton(struct Button* button){

}

int ButtonEvent(struct Controller *controller, struct Button *button)
{
    switch (button->mode)
    {
    case Redirect:
        controller->currentPage = button->modeParam.redirect.pointToPage;
        LOG("[Controller]Point to page %ld\n", controller->currentPage - controller->pagesList);
        break;
    case LED:
        // LED control
        LOG("LED triggered.\n");
        button->modeParam.led.ledState = !(button->modeParam.led.ledState);
        ctrl_led(button->modeParam.led.ledIndex,button->modeParam.led.ledState);
        break;
    default:
        // None
        break;
    }
    return 0;
}

int Run(struct Controller *controller)
{
    LOG("[Controller]Start running ...\n");
    while (1)
    {
        struct Page *page = controller->currentPage;
        LOG("[Controller]Current page:%ld\n", page - controller->pagesList);
        lcd_show_bmp(page->bgPath);
        struct Vector pos = GetTorchPos();
        for (int i = 0; i < page->buttonsCount; i++)
        {
            struct Button *b = page->buttons + i;
            DEBUG_BUTTON((*b));
            if (pos.x >= b->rect.lt.x && pos.x <= b->rect.rd.x && pos.y >= b->rect.lt.y && pos.y <= b->rect.rd.y)
            {
                ButtonEvent(controller,b);
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
                button->modeParam.led.ledState=0;
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