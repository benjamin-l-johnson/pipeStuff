#include <stdlib.h>
#include <stdio.h>
#include "apue.h"

#define BUFFSIZE 2048

int main (int argc, char *argv[] )
{

	char * p;
	char buff[BUFFSIZE];

	//read 2048 chars from stdin 
	if(read(STDIN_FILENO,buff,BUFFSIZE-1)<0)
	{
		fprintf(stderr, "Read error");
	}

	//set the start of our buffer
	p = buff;
	for ( ; *p; ++p) *p = printf("%c",toupper(*p));
}
