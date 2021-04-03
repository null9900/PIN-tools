#include <stdio.h>

int function1(){
	return 0;
}

int main(){
	int r = function1();
	if(r==1){
		printf("%s\n", "returned value is 1");
	}else{
		printf("%s\n", "returned value is 0");
	}
}