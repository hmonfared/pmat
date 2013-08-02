/*
 * testmain.cpp
 *
 *  Created on: Jul 30, 2013
 *      Author: Hassan H. Monfared
  */
#include "matrix.h"
#define MAT_ROWS 20
#define MAT_COLS 20
int main(int argc, char **argv)
{
	matrix<int> a(MAT_ROWS,MAT_COLS);
	for(int i=0;i<MAT_ROWS;++i)
		for(int j=0;j<MAT_COLS;++j)
			a(i,j)=1;
	//a=1.5;
//	a.add_row(0,{3,3,3});
//	a.set_max_thread_num(1);
   std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();

	time_t tnow = std::chrono::system_clock::to_time_t(now);
	//std::cout << a;
	//a = 5*a;
	//std::cout << a;
	//a= a*a*a;
	//std::cout << a;
	//a.transpose();
	a=a+2*a;
	std::cout << a;
   std::chrono::time_point<std::chrono::system_clock> then = std::chrono::system_clock::now();

   std::cout << "elapsed time="<< std::chrono::duration_cast<std::chrono::milliseconds>(then-now).count();

	getchar();
	return 0;
}
