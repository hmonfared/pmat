/*
* matrix.cpp
*
*  Created on: Jul 27, 2013
*      Author: root
*/

#include "matrix.h"

template<class T>
matrix<T>::matrix(void)
{
	data=nullptr;
	columns=0;
	rows=0;
	max_thread_num=8;
}
template<class T>
void matrix<T>::set_max_thread_num(const size_t pthread_num)
{
	if(pthread_num<1)
		thorw ("pmat exception: Invalid thread number");
	max_hread_num=pthread_num;
}

template<class T>
matrix<T>::matrix(matrix &&pother):data(nullptr),rows(0),columns(0)
{
	*this=std::move(pother);
}

template<class T>
matrix<T>::matrix(size_t prows,size_t pcolumns)
{
	if(prows <=0 || pcolumns <=0)
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
T& matrix<T>::operator()(size_t prow,size_t pcolumn)
{
	if(!data)
		throw ("pmat exception:no memory allocated for matrix");
	if(prow<0 || prow >= rows || pcolumn<0 || prow>=rows)
		throw ("pmat exception: invalid row/column specified");
	return data[prow*columns+pcolumn];
}

template<class T>
void matrix<T>::setsize(size_t prows,size_t pcolumns)
{
	if(prows <=0 || pcolumns <=0)
		throw ("pmat exception: invalid row/column size");
	if(data)
		delete data;
	data=new T[prows*pcolumns];
	rows=prows;
	columns=pcolumns;
}
template<class T>
void matrix<T>::sum(matrix &presult,const matrix &pleft,const matrix &pright,const size_t ppartition_index,const size_t ppartition_size,const short pparition_by)
{
	size_t part_loop_end=std::min(ppartition_index+ppartition_size,THE_SIZE(pparition_by,pleft));
	size_t second_loop_end=(pparition_by == PMAT_PARTITION_BY_COLUMNS )? pleft.rows;
	size_t rowcounter,columncounter,rowend,columnend,rowstart,columnstart;
	if(pparition_by == PMAT_PARTITION_BY_COLUMNS )
	{
		rowstart=0;
		rowend=pleft.rows;
		columnstart=ppartition_index*ppartition_size;
		columnend=part_loop_end;
	}
	else
	{
		rowstart=ppartition_index*ppartition_size;
		rowend=part_loop_end;
		columnstart=0;
		columnend=pleft.rows;
	}
	for(rowcounter=rowstart;rowcounter<rowend;++rowcounter)
		for(size_t columncounter=columnstart;columncounter<columnend;++columncounter)
				presult(rowcounter,columncounter)=pleft(rowcounter,columncounter)+pright(rowcounter,columncounter);
}
template<class T>
matrix<T> operator +(matrix<T> &&pleft,const matrix<T> &pright)
{
	//TODO:  summation goes here:
	short orient=pleft.rows>pleft.columns ? PMAT_PARTITION_BY_ROWS:PMAT_PARTITION_BY_COLUMNS;
	size_t partition_size= THE_SIZE ( orient , pleft )/max_thread_num;
	size_t thread_nums=0;
	if(partition_size > PMAT_MIN_PARTITION_SIZE )
		thread_nums = max_thread_num;
	else
	{
		partition_size = ( orient == PMAT_PARTITION_BY_ROWS ) ? std::min(pleft.rows,PMAT_MIN_PARTITION_SIZE):
			std::min(pleft.columns,PMAT_MIN_PARTITION_SIZE);
		thread_nums=( orient == PMAT_PARTITION_BY_ROWS ) ? pleft.rows/partition_size:pleft.columns/partition_size;
	}
	std::vector<thread> threads(partition_size);
	
	for(size_t partition=0;partition < partition_size ; ++partition )
	{
		std::thread tr(sum,pleft,pleft,pright,partition,partition_size,orient);
		threads[partition]=std::move(tr);
	}
	for(size_t partition=0;partition < partition_size ; ++partition )
		threads[partition].join();
	return std::move(pleft);
}
template<class T>
matrix<T> operator +(const matrix<T> &pleft,matrix<T> &&pright)
{
	return std::move(pright)+pleft;
}
template<class T>
matrix<T> operator +(matrix<T> &&pleft,matrix<T> &&pright)
{
	return std::move(pleft)+pright;
}
template<class T>
matrix<T> operator +(const matrix<T> &pleft,const matrix<T> &pright)
{
	matrix<T> tmp(pleft);
	return std::move(pleft)+pright;
}


/**** multiplication ******/

template<class T>
matrix<T> operator *(const matrix<T> &pleft,const matrix<T> &pright)
{
	if(pleft.columns != pright.rows )
		throw ("pmat: Invalid dimension for multiplication");
	matrix<T> result(pleft.rows,pright.columns);
	//TODO: multiplication goes here
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


