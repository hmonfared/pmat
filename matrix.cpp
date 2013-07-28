#include "matrix.h"

template<class T>
matrix<T>::matrix(void)
{
	data=nullptr;
	columns=0;
	rows=0;
}
template<class T>
matrix<T>::matrix(matrix &&pother):data(nullptr),rows(0),columns(0)
{
	*this=std::move(pother);
}

template<class T>
matrix<T>::matrix(size_t prows,size_t pcolumns)
{
	if(prows <=0 || pcols <=0)
		throw ("pmat exception: Invalid row/column size");
	data=new T[prows*pcolumns];
	rows=prows;
	columns=pcolumns;

}

template<class T>
matrix<T>::~matrix(void)
{
	if(data)
		delete []data;
}
template<class T>
T matrix<T>::operator()(size_t prow,size_t pcolumn)
{
	if(!data)
		throw ("pmat exception:no memory allocated for matrix");
	if(prow<0 || prow >= rows || pcol<0 || prow>=rows)
		throw ("pmat exception: invaid row/column specified");
	return data[prow*columns+pcolumn];
}

template<class T>
void matrix<T>::setsize(size_t prows,size_t pcolumns)
{
	if(prows <=0 || pcolumns <=0)
		throw ("pmat exception: invaid row/column size");
	if(data)
		delete data;
	data=new T[prows*pcolumns];
	rows=prows;
	columns=pcolumns;
}

template<class T>
friend auto operator +(matrix<T> &&pleft,const matrix<T> &pright)->matrix<T>
{
	//TODO:  sumation goes here:

	return std::move(pleft);
}
template<class T>
friend auto operator +(const matrix<T> &pleft,matrix<T> &&pright)->matrix<T>
{
	return std::move(pright)+pleft;
}
template<class T>
friend auto operator +(matrix<T> &&pleft,matrix<T> &&pright)->matrix<T>
{
	return std::move(pleft)+pright;
}
template<class T>
friend auto operator +(const matrix<T> &pleft,const matrix<T> &pright)->matrix<T>
{
	matrix<T> tmp(pleft);
	return std::move(pleft)+pright;
}


///////////////////////////////// MULTIPLICATION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

template<class T>
friend auto operator *(const matrix<T> &pleft,const matrix<T> &pright)->matrix<T>
{
	if(pleft.columns != pright.rows )
		throw ("pmat: Invalid dimention for multiplication");
	matrix<T> result(pleft.rows,pright.columns);
	//TODO: multipliation goes here
	return std::move(result);
}

template<class T>
matrix<T> & matrix<T>::operator =(matrix<T> &&pright)
{
	if(this ==&pright)
		return *this;
	if(data)
		delete []data;
	data=pright.data;
	rows=pright.rows;
	columns=pright.columns;
	pright.data=nullptr;
	pright.rows=0;
	pright.columns=0;
	return *this;
}


