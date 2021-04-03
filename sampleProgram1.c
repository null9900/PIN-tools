#include <stdio.h>

int main(){
	FILE *fp;
	fp = fopen("/home/maysara/Desktop/filename.txt", "w+");
	fputs("Writting to file", fp);
	fclose(fp);
	return 0;
}
