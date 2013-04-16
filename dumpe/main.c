#include<stdio.h>
#include<unistd.h>
#include <stdlib.h>

#define true 1
#define false 0
char* inBuf;
char* outBuf;
int size;
int filled;


int Printing(char * str, int len){
    write(1, str, len );
    int n = write(1, str, len);
    return n;
}

int Reading(){

    _Bool wasLongWord = false;
    ////_Bool wasPartWord = false;
    _Bool wasNotPrinted = false;
    int lastWordStart;
    
    int j = 0;
    
    while(1){ 
        //printf("while\n");
	int readed = read(0, inBuf + filled, size - filled);
        if ( readed == 0) {
	    if(wasNotPrinted){
		outBuf[j] = '\n';
		Printing(outBuf, j + 1); 
	    }
            //printf("readed 0");
            return  0;
        } else {
	    //printf("Was Not Printed false\n");
	    wasNotPrinted = false;
	}
        
        
        int curLen = filled + readed;
        //printf("curLen:%d+%d\n", filled , readed);
	
        int cWord = 0;
	int i = 0;
	
	
	for(i = 0; i <= curLen - 1; ++i) {
	
	    j = 0;
	    while( (inBuf[i] != '\n') && (i <= curLen - 1) ) {      
		outBuf[j] = inBuf[i];
		++j;
		++i;
	    }
	    
	    
	    if(inBuf[i] == '\n' &&  i <= curLen - 1 ){
		++cWord;
		if(!wasLongWord){
		    //printf("Addet str end.\n");
		    outBuf[j] = '\n';
		    lastWordStart = i + 1;
		    Printing(outBuf, j + 1);
		    filled = 0;
		    
		} else {
		    wasLongWord = false;
		    lastWordStart = i + 1;
		}
		j = 0;
	    } else {
	       
		--i;
		if(cWord == 0) {
		    if(curLen == size){
			//printf("Long word detected\n");
			wasLongWord = true;
			filled = 0;
			j = 0;
		    }else{
			////wasPartWord = true;
			filled = i + 1;
			//printf("Was Not Printed true\n");
			////wasNotPrinted = true;
		    }
		} else {
		    
		    //printf("Part word detected\n");
		    //printf("copy part word to begi of buf\n");
		    int z;
		    for(z = 0; lastWordStart + z <= curLen -1; ++z){ 
			inBuf[z] = inBuf[lastWordStart + z];
			//printf(" %d", inBuf[z] - '0');
		    }
		    
		    
		    filled = size - lastWordStart;
		    //printf("Filled = %d \n", filled);
		    //printf("Was Not Printed true\n");
		    wasNotPrinted = true;
		}  
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
    //printf("begin\n");
    size = k;
    filled = 0;
    inBuf = malloc(size);
    outBuf = malloc(size);

    Reading(); 
   
    free(inBuf);
    free(outBuf);
    return 0; 
        
}
