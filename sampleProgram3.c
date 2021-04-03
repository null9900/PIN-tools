#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int function1(){
}

int function2(){
}

int function3(){
	function2();
	function1();
}

int main(){ 
	function1();
	function2();
	function3();
	function1();
	return 0;
}
