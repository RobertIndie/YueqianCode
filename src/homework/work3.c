#include <stdio.h>
#include <stdlib.h>

int main(int argc,char** args){
    if(argc!=2)return -1;
    unsigned int num = atoi(*(args+1));
    printf("%x\n",num);
    num = num | (1<<12);
    printf("%x\n",num);
    return 0;
}