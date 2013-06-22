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
int size;
int filled;
int max;
int sepCount;

void printRow()
{
	printf("\nPrinting\n");
	int i = 0;
	for(i = 0; i < sepCount; ++i)
	{
		printf("%s ", out[order[i]]);
	}
	
}

int Reading(){

	_Bool colonNotEnded = false;
	_Bool lastIt = false;
	int curSep = 0;
	int outFilled = 0;

	filled = 0;
	while(1){ 
		//printf("while\n");
		int readed = read(0, inBuf + filled, size - filled);
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
		else
		{
			//printf("Was Not Printed false\n");
			//wasNotPrinted = false;
		}

		
		int curLen = filled + readed;
		filled = curLen;
		//printf("curLen:%d+%d\n", filled , readed);

		int cWord = 0;
		int i = 0;
		
		char * s = strstr(inBuf, sep[curSep]);
		
		if(s == NULL)
		{
			if(curLen < max)
			{
				if(!lastIt)
				{
					continue;
				}
				else
				{
					return -1;
				}
			}
			else
			{
				return -1;
			}
		} 
		else 
		{
			int i = 0;
			int sPos = s - inBuf;
			printf("sPos:%d", sPos);
			for(i = 0; i < sPos; ++i)
			{
				out[curSep][i] = inBuf[i];
			}
			out[curSep][i] = '\0';
			outFilled = 0;
			filled = curLen - (sPos + sepLen[curSep]);
			memmove(inBuf, s + sepLen[curSep], filled);
			
		    
			if(curSep == sepCount - 1)
			{
				printRow();
			}
			else
			{
				curSep++;
			}
			
		}
	}
		
	return 1;
}



int main(int argc, char * argv[])
{
	
	printf("begin\n");

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
		size = max;
		
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
	}
	
	
	
	printf("begin\n");
	int i;
	for(i = 0; i < sepCount; ++i)
	{
		printf("sep:%s sepLen:%d ord:%d\n", sep[i], sepLen[i], order[i]);
	}


	inBuf = malloc(size);
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
 
