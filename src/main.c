#include <stdio.h>

enum a{
    a,b
};
int main(){
    char*str = "1 2 r 2";
    char buff[100];
    int a,b;
    sscanf(str,"%d %d %[^\n]",&a,&b,buff);
    printf("%d %d \n%s",a,b,buff);
    
    return 0;
}