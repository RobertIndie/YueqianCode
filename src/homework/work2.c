#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **arg){
    if(argc!=2)return -1;
    unsigned int num = atoi(*(arg+1));
    int result = 0;
    for(int i=0;i<8*sizeof(unsigned int);i++){
        if(num%2==1)result++;
        num=num>>1;
    }
    printf("%d\n",result);
    return 0;
}