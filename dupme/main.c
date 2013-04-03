#include<stdio.h>
#include<unistd.h>
#include <stdlib.h>
//#include<stdio>l
char* inBuf;
char* outBuf;
int size;
int filled;
int empty;


void Printing(){
    write(1, outBuf, filled);
    write(1, outBuf, filled);
    filled = 0;
    return;
}

int Reading(){
while(1){ 
   if(filled < size){ 
        int readed = read(0, inBuf + filled, size - filled);
        if(readed == 0) return  0;
       // printf("k:%d\n", readed);
        if((readed == size) && (inBuf[readed - 1] != '\n')){
           //printf("to longEE\n");
           char * tmp;
           tmp = malloc(1);
           read(0, tmp, 1);
           while(tmp[0] != '\n'){
               read(0, tmp, 1);
           }
           
        } else {

            if( inBuf[readed - 1] != '\n'){
                ++readed; 
                inBuf[readed - 1] = '\n';
            } 
            memcpy(outBuf, inBuf, readed * sizeof(char));
            filled += readed;
            Printing(); 
        }
   }

}
   return 1;
}

int main(int argc, char * argv[]){
    int k = 3;
    if(argc > 1){
        int i = 0; 
        k = 0;
        while(argv[1][i] != 0 ){
            k = k*10 + argv[1][i] - '0';
            ++i;
        }
        ++k;
    }
    size = k;
    empty = k;
    filled = 0;
    inBuf = malloc(size);
    outBuf = malloc(size);

    if(Reading() == 1)
   
    free(inBuf);
    free(outBuf);
    return 0; 
        
}
