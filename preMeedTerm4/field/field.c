#include<stdio.h>
#include<unistd.h>
#include <stdlib.h>

#define true 1
#define false 0
char** sep;
char** out;
char* inBuf;
char* outBuf;
int* sepLen;
int* order;
int filled;
int max;
int sepCount;

void showRow();

int Reading()
{
    _Bool colonNotEnded = false;
    _Bool lastIt = false;
    int curSep = 0;

    filled = 0;
    while(1)
    { 
        //printf("while\n");
        int readed = read(0, inBuf + filled, max - filled) - 1; // minus '\n' from the end
        if (readed == 0 || readed > max) {
            //printf("readed 0");
            if(filled == 0)
            {
                return 0;
            }
            else
            {
                lastIt = true;
            }
        }

        filled += readed;
        char * s = strstr(inBuf, sep[curSep]);
        while(s != NULL)
        {

            int i = 0;
            int sPos = s - inBuf;
            //printf("sepPos:%d\n", sPos);
            for(i = 0; i < sPos; ++i)
            {
                out[curSep][i] = inBuf[i];
            }
            out[curSep][i] = '\0';
            //printf("out[%d]:%s\n",curSep, out[curSep]);
            filled = filled - (sPos + sepLen[curSep]);
            memmove(inBuf, s + sepLen[curSep], filled);

            //printf("inBuf:%s\n", inBuf);
            if(curSep == sepCount - 1)
            {
                showRow();
                curSep = 0;
            }
            else
            {
                curSep++;
            }

            s = strstr(inBuf, sep[curSep]);	
        }

        if(!lastIt && filled < max)
        {
            //continue;
        }
        else
        {
            return -1;
        }
    }
    return 1;
}

void showRow()
{
    printf(">> ");

    int i;

    for(i = 0; i < sepCount; ++i)
    {
        printf("%s ", out[order[i] - 1]);
    }
    printf("\n");

}



int main(int argc, char * argv[])
{
    if(argc > 1){

        //printf("ok1\n");
        sepCount = 0;
        int i;
        for(i = 1 ; argv[i][0] != ';'; ++i)
        {
            //printf("ok2 %d\n", i);
            sepCount++;
        }

        //printf("ok3\n");
        //printf("sepCount:%d\n", sepCount);

        sep = malloc(sepCount);
        out = malloc(sepCount);
        sepLen = malloc(sepCount);
        order = malloc(sepCount);
        max = 128;

        for(i = 0; i < sepCount; ++i)
        {
            sep[i] = malloc(max);
            out[i] = malloc(max);
            sepLen[i] = 0;
            int j;
            for(j = 0; argv[i + 1][j] != '\0'; ++j)
            {
                sep[i][j] = argv[i + 1][j];	
                sepLen[i]++;
            }
            sep[i][j] = '\0';
        }
        
        //printf("ok4\n");
        ++i;
        ++i;
        int j = 0;
        order[j] = 0;
        for(i = i; i < argc; ++i)
        {
            int l;
            for(l = 0; argv[i][l] != '\0'; ++l)
            {
                //printf("a:%d\n",argv[i][l] );
                order[j] = order[j] * 10 + argv[i][l] - '0';
            }
            j++;
            //printf("\n\n\n");
        }
        //printf("ok5\n");
    }
    else
    {
        printf("wrong Args!\n");
        return 0;
    }

    int i;
    for(i = 0; i < sepCount; ++i)
    {
        //printf("sep:%s sepLen:%d ord:%d\n", sep[i], sepLen[i], order[i]);
    }


    inBuf = malloc(max);
    //outBuf = malloc(size);

    Reading(); 

    free(inBuf);
    //free(outBuf);

    for(i = 0; i < sepCount; ++i)
    {
        free(sep[i]);
        free(out[i]);
    }
    free(sep);
    free(out);
    return 0; 
}
