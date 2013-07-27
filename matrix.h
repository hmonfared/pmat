/*
 * matrix.h
 *
 *  Created on: Jul 27, 2013
 *      Author: Hassan H. Monfared ( hmonfared@gmail.com )
 */

#ifndef _PMAT_MATRIX_H_
#define _PMAT_MATRIX_H_
#include <stdio.h>
#include <iostream>

template<class T>
class matrix
{
private:
	T *data;
	size_t rows;
	size_t columns;

public:
	matrix(void);
	matrix(matrix &&pother); // move constructor
	matrix(size_t prows,size_t pcolumns);
	void setsize(size_t prows,size_t pcolumns);
	virtual ~matrix(void);

	T operator()(size_t prow,size_t pcolumn);

	matrix &operator =(matrix &&pright); //
	matrix &operator =(const matrix &pright);
	template<class U>
	friend auto operator +(matrix<U> &&pleft,const matrix<U> &pright)->matrix<U>;
	template<class U>
	friend auto operator +(const matrix<U> &pleft,matrix<U> &&pright)->matrix<U>;
	template<class U>
	friend auto operator +(const matrix<U> &pleft,const matrix<U> &pright)->matrix<U>;
	template<class U>
	friend auto operator +(matrix<U> &&pleft,matrix<U> &&pright)->matrix<U>;
	template<class U>
	friend auto operator *(const matrix<U> &pleft,const matrix<U> &pright)->matrix<U>;


};

#endif /* MATRIX_H_ */
