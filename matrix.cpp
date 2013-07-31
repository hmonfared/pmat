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
matrix<T>::matrix(matrix &pother):data(nullptr),rows(pother.rows),columns(pother.columns)
{
	memcpy(data,pother.data,sizeif(T)*pother.rows*pother.columns);
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
template<class T>
matrix<T> & matrix<T>::operator =(matrix<T> &pright)
{
	if(this ==&pright)
		return *this;
	if(data)
		delete []data; // FIXME: memory realloc is better choice, (... care about shrinking in thid way )
	data=new T[pright.rows*pright.columns];
	memcpy(data,pright.data,sizeof(T)*pright.rows*pright.columns);
	rows=pright.rows;
	columns=pright.columns;
	return *this;
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
void matrix<T>::sum(matrix<T> &presult,const matrix<T> &pleft,const matrix<T> &pright,const size_t ppartition_index,const size_t ppartition_size,const short pparition_by)
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
void matrix<T>::multiply(matrix<T> &presult,const T &pscalar,const matrix<T> &pright,const size_t ppartition_index,const size_t ppartition_size,const short pparition_by)
{
	size_t part_loop_end=std::min(ppartition_index+ppartition_size,THE_SIZE(pparition_by,pright));
	size_t second_loop_end=(pparition_by == PMAT_PARTITION_BY_COLUMNS )? pright.rows;
	size_t rowcounter,columncounter,rowend,columnend,rowstart,columnstart;
	if(pparition_by == PMAT_PARTITION_BY_COLUMNS )
	{
		rowstart=0;
		rowend=pright.rows;
		columnstart=ppartition_index*ppartition_size;
		columnend=part_loop_end;
	}
	else
	{
		rowstart=ppartition_index*ppartition_size;
		rowend=part_loop_end;
		columnstart=0;
		columnend=pright.rows;
	}
	for(rowcounter=rowstart;rowcounter<rowend;++rowcounter)
		for(size_t columncounter=columnstart;columncounter<columnend;++columncounter)
				presult(rowcounter,columncounter)=pscalar*pright(rowcounter,columncounter);
}

template<class T>
void matrix<T>::multiply(matrix<T> &presult,const matrix<T>  &pleft,const matrix<T> &pright,const size_t ppartition_index,const size_t ppartition_size,const short pparition_by)
{
	size_t part_loop_end=std::min(ppartition_index+ppartition_size,THE_SIZE(pparition_by,pright));
	size_t second_loop_end=(pparition_by == PMAT_PARTITION_BY_COLUMNS )? pright.rows;
	size_t rowcounter,columncounter,rowend,columnend,rowstart,columnstart;
	if(pparition_by == PMAT_PARTITION_BY_COLUMNS )
	{
		rowstart=0;
		rowend=pright.rows;
		columnstart=ppartition_index*ppartition_size;
		columnend=part_loop_end;
	}
	else
	{
		rowstart=ppartition_index*ppartition_size;
		rowend=part_loop_end;
		columnstart=0;
		columnend=pright.rows;
	}
//TODO: continue from here
}
template<class T>
matrix<T> operator *(const matrix<T> &pleft,const matrix<T> &pright)
{
	if(pleft.columns != pright.rows )
		throw ("pmat: Invalid dimension for multiplication");
	matrix<T> result(pleft.rows,pright.columns);

	short orient=result.rows>result.columns ? PMAT_PARTITION_BY_ROWS:PMAT_PARTITION_BY_COLUMNS;
	size_t partition_size= THE_SIZE ( orient , result )/max_thread_num;
	size_t thread_nums=0;
	if(partition_size > PMAT_MIN_PARTITION_SIZE )
		thread_nums = max_thread_num;
	else
	{
		partition_size = ( orient == PMAT_PARTITION_BY_ROWS ) ? std::min(result.rows,PMAT_MIN_PARTITION_SIZE):
			std::min(result.columns,PMAT_MIN_PARTITION_SIZE);
		thread_nums=( orient == PMAT_PARTITION_BY_ROWS ) ? result.rows/partition_size:result.columns/partition_size;
	}
	std::vector<thread> threads(partition_size);
	
	for(size_t partition=0;partition < partition_size ; ++partition )
	{
		std::thread tr(multiply,result,pleft,pright,partition,partition_size,orient);
		threads[partition]=std::move(tr);
	}
	for(size_t partition=0;partition < partition_size ; ++partition )
		threads[partition].join();
	return std::move(result);
}

template<class T>
matrix<T> operator *(const double &pscalar,const matrix<T> &pright)
{
	matrix<T> result(pright);
}
template<class T>
matrix<T> operator *(const double &pscalar,matrix<T> &&pright)
{
	short orient=pright.rows>pright.columns ? PMAT_PARTITION_BY_ROWS:PMAT_PARTITION_BY_COLUMNS;
	size_t partition_size= THE_SIZE ( orient , pright )/max_thread_num;
	size_t thread_nums=0;
	if(partition_size > PMAT_MIN_PARTITION_SIZE )
		thread_nums = max_thread_num;
	else
	{
		partition_size = ( orient == PMAT_PARTITION_BY_ROWS ) ? std::min(pright.rows,PMAT_MIN_PARTITION_SIZE):
			std::min(pright.columns,PMAT_MIN_PARTITION_SIZE);
		thread_nums=( orient == PMAT_PARTITION_BY_ROWS ) ? pright.rows/partition_size:pright.columns/partition_size;
	}
	std::vector<thread> threads(partition_size);
	
	for(size_t partition=0;partition < partition_size ; ++partition )
	{
		std::thread tr(multiply,pright,pscalar,pright,partition,partition_size,orient);
		threads[partition]=std::move(tr);
	}
	for(size_t partition=0;partition < partition_size ; ++partition )
		threads[partition].join();
	return std::move(pright);
}


template<class T>
void matrix<T>::add_row(const size_t prow,const std::vector<T> &prow_vector)
{
	size_t last=std::min(prow_vector.size(),columns);
	size_t col=0;
	for(;col<columns && col<last;++cols)
		(prow,col)=prow_vector[col];
	for(;col<columns;++col) // if vector data is no enough , fill reminded columns with zero
		(prow,col)=0;
}
template<class T>
std::ostream &operator<<(std::ostream &pos,const matrix<T> &pmatrix)
{
	for(size_t row=0;row<rows;++row)
	{
		for(size_t col=0;col<columns;++col)
			pos << pmatrix(rows,col);
		pos<< std::endl;
	}
	return pos;
}


