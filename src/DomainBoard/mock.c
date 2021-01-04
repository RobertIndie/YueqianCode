#include "../../lib/db.h"

int ReadBMP(char* name, struct BMP** ptrBMP){
    LOG("Load BMP: %s", name);
    struct BMP* result = (struct BMP*)malloc(sizeof(struct BMP));
    result->size = 1;
    result->data = (int*)malloc(sizeof(int)*result->size);
    for(int i=0;i<result->size;i++){
        result->data[i]=5; // fake data
    }
    *ptrBMP = result;
    return ResultOK;
}

int FreeBMP(struct BMP** ptrBMP){
    struct BMP* bmp = *ptrBMP;
    free(bmp->data);
    free(bmp);
    *ptrBMP=NULL;
    return ResultOK;
}

int InitLCD(struct LCD** lcd){
    struct LCD* result = (struct LCD*)malloc(sizeof(struct LCD));
    result->fd = 0;
    result->size = 10;
    result->base = (int*)malloc(sizeof(int)*result->size);
    for(int i=0;i<result->size;i++){
        result->base[i]=i; // fake data
    }
    *lcd = result;
    return 0;
}

int ShowBMP(struct LCD* lcd, struct BMP* bmp){
    for(int i=0;i<bmp->size;i++){
        lcd->base[i] = bmp->data[i];
    }
    return 0;
}

int CloseLCD(struct LCD** lcd){
    struct LCD* l = *lcd;
    free(l->base);
    free(l);
    *lcd = NULL;
    return 0;
}

// 获取触摸屏的触摸位置，将阻塞等待用户输入
struct Vector GetTorchPos(){
    struct Vector result;
    result.x = 400;
    result.y = 240;
    return result;
}

int Run(struct Controller* controller){
    while(1){
        struct Vector pos = GetTorchPos();
        struct Page* page = controller->currentPage;
        for(int i=0;i<page->buttonsCount;i++){
            struct Button* b = page->buttons + i;
            if(pos.x>=b->rect.lt.x && pos.x <=b->rect.rd.x
                && pos.y>=b->rect.rd.y && pos.y<=b->rect.lt.y
            ){
                controller->currentPage = b->pointToPage;
                break;
            }
        }
    }
}

struct Controller* ConfigLoad(char* configFilePath){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    struct Controller* controller = (struct Controller*) malloc(sizeof(struct Controller));

    fp = fopen(configFilePath, "r");
    if (fp == NULL)
    {
        LOG("Read config failed.\n");
        exit(-1);   
    }

    #define GET_LINE if((read=getline(&line,&len,fp))==-1) {LOG("Error reading config fail.");exit(-1);}

    GET_LINE
    sscanf("%d",&(controller->pagesListSize));
    controller->pagesList = (struct Page*)malloc(sizeof(struct Page)*controller->pagesListSize);
    for(int i=0;i<controller->pagesListSize;i++){
        struct Page* page = controller->pagesList+i;
        sscanf("%d",&(page->buttonsCount));
        page->buttons = (struct Button*)malloc(sizeof(struct Button)*page->buttonsCount);
        for(int j=0;j<page->buttonsCount;j++){
            struct Button* button = page->buttons+i;
            sscanf("%d %d %d %d %d",&(button->rect.lt.x),
                                    &(button->rect.lt.y),
                                    &(button->rect.rd.x),
                                    &(button->rect.rd.y));
        }
    }

    fclose(fp);
    if (line)
        free(line);
}