/*
 * testmain.cpp
 *
 *  Created on: Jul 30, 2013
 *      Author: Hassan H. Monfared
  */
#include "matrix.h"
int main(int argc, char **argv)
{
	matrix<int> a(2,2);
	a(0,0)=1;a(0,1)=1;
	a(1,0)=1;a(1,1)=1;
	std::cout << a;
	a = 2*a;
	std::cout << a;
	getchar();
}
