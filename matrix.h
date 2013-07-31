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
#include <vector>
#include <thread>
#include <exception>
#include <cstring>
#include <iomanip>
#define PMAT_DEBUG



#define PMAT_MIN_PARTITION_SIZE 50
#define THE_SIZE(PART_BY,MAT) (( PART_BY == PMAT_PARTITION_BY_ROWS ) ? MAT.rows:MAT.columns)
#define MIN(a,b) ( ((a)<(b)) ? (a):(b) )

enum PMAT_PARTITION_BY
{
	PMAT_PARTITION_BY_NONE=0,
	PMAT_PARTITION_BY_ROWS=1,
	PMAT_PARTITION_BY_COLUMNS=2
};
template<class T>
class matrix
{
private:
	T *data;
	size_t rows;
	size_t columns;
	size_t max_thread_num;
	static void sum(
		matrix &presult,
		const matrix &pleft,
		const matrix &pright,
		const size_t ppartition_index,
		const size_t ppartition_size,
		const short pparition_by)
	{
		size_t part_loop_end=MIN(ppartition_index*ppartition_size+ppartition_size,THE_SIZE(pparition_by,pleft));
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
	static void multiply(matrix &presult,const T &pscalar,const matrix &pright,const size_t ppartition_index,const size_t ppartition_size,const short pparition_by)
	{
		size_t part_loop_end=MIN(ppartition_index*ppartition_size+ppartition_size,THE_SIZE(pparition_by,pright));
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
			{
				T r=pscalar*pright(rowcounter,columncounter);
				presult(rowcounter,columncounter)=r;
				//std::cout <<" (i,j )=" << presult(rowcounter,columncounter)<< "r="<<r<<std::endl;
			}
	}
	static void multiply(matrix &presult,const matrix &pleft,const matrix &pright,const size_t ppartition_index,const size_t ppartition_size,const short pparition_by)
	{
		size_t part_loop_end=MIN(ppartition_index*ppartition_size+ppartition_size,THE_SIZE(pparition_by,pright));
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
		T inner_sum;
		for(size_t row=rowstart;row<rowend;++row)
		{
			for(size_t col=columnstart;col<columnend;++col)
			{
				inner_sum=0;
				for(size_t inner=0;inner<pright.rows;++inner)
					inner_sum+=pleft(row,inner)*pright(inner,col);
				presult(row,col)=inner_sum;
			}
		}
	}

public:
	matrix() // default constructor
	{
		data=nullptr;
		columns=0;
		rows=0;
		max_thread_num=std::thread::hardware_concurrency();
	}
	matrix(matrix &&pother):data(nullptr),rows(0),columns(0),max_thread_num(std::thread::hardware_concurrency())  // move constructor
	{
#ifdef PMAT_DEBUG
		std::cout << "in move cor"<< std::endl;
#endif

		*this=std::move(pother);
	}
	matrix(matrix const &pother):data(nullptr),rows(pother.rows),columns(pother.columns),max_thread_num(pother.max_thread_num)  // copy constructor
	{
		#ifdef PMAT_DEBUG
		std::cout << "in copy cor"<< std::endl;
#endif

		data=new T[pother.rows*pother.columns]; // alocate memory for data
		memcpy(data,pother.data,sizeof(T)*pother.rows*pother.columns); // copy data to allocated space
	}
	matrix(size_t prows,size_t pcolumns)
	{
		if(prows <=0 || pcolumns <=0)
			throw ("pmat exception: Invalid row/column size");
		data=new T[prows*pcolumns];
		rows=prows;
		columns=pcolumns;
		max_thread_num=std::thread::hardware_concurrency();

	}
	void setsize(size_t prows,size_t pcolumns)
	{
		if(prows <=0 || pcolumns <=0)
			throw ("pmat exception: invalid row/column size");
		if(data)
			delete data;
		data=new T[prows*pcolumns];
		rows=prows;
		columns=pcolumns;
	}
	void set_max_thread_num(const size_t pthread_num)
	{
		if(pthread_num<1)
			throw ("pmat exception: Invalid thread number");
		max_thread_num=pthread_num;
	}
	virtual ~matrix(void)
	{
		if(data)
			delete []data;
	}

	T& operator()(size_t prow,size_t pcolumn)
	{
		if(!data)
			throw ("pmat exception:no memory allocated for matrix");
		if(prow<0 || prow >= rows || pcolumn<0 || prow>=rows)
			throw ("pmat exception: invalid row/column specified");
		return data[prow*columns+pcolumn];
	}
	T operator()(size_t prow,size_t pcolumn) const
	{
		if(!data)
			throw ("pmat exception:no memory allocated for matrix");
		if(prow<0 || prow >= rows || pcolumn<0 || prow>=rows)
			throw ("pmat exception: invalid row/column specified");
		return data[prow*columns+pcolumn];
	}

	matrix &operator =(matrix &&pright)
	{
#ifdef PMAT_DEBUG
		std::cout << "in move = "<< std::endl;
#endif

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
	matrix &operator =(const matrix &pright)
	{
#ifdef PMAT_DEBUG
		std::cout << "in copy = "<< std::endl;
#endif

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
	matrix &operator =(const T &pright)
	{
		if(!data)
			return *this;
		std::fill(data,data+rows*columns,pright);
	}
	// summation functions and operators
	template<class U>
	friend matrix<U> operator +(matrix<U> &&pleft,const matrix<U> &pright)
	{
		short orient=pleft.rows>pleft.columns ? PMAT_PARTITION_BY_ROWS:PMAT_PARTITION_BY_COLUMNS;
		size_t partition_size= THE_SIZE ( orient , pleft )/pleft.max_thread_num;
		size_t thread_nums=0;
		if(partition_size > PMAT_MIN_PARTITION_SIZE )
			thread_nums = pleft.max_thread_num;
		else
		{
			partition_size = ( orient == PMAT_PARTITION_BY_ROWS ) ? MIN(pleft.rows,PMAT_MIN_PARTITION_SIZE):
				MIN(pleft.columns,PMAT_MIN_PARTITION_SIZE);
			thread_nums=( orient == PMAT_PARTITION_BY_ROWS ) ? pleft.rows/partition_size:pleft.columns/partition_size;
		}
		std::vector<std::thread> threads(thread_nums);
#ifdef PMAT_DEBUG
		std::cout << "++part size="<<partition_size <<" thread nums="<<thread_nums<<std::endl;
#endif 
		for(size_t partition=0;partition < thread_nums ; ++partition )
		{
			threads[partition]=std::thread([&](){matrix<T>::sum(pleft,pleft,pright,partition,partition_size,orient);});
		}
		for(size_t partition=0;partition <thread_nums ; ++partition )
			threads[partition].join();
		return std::move(pleft);
	}
	template<class U>
	friend matrix<U> operator +(const matrix<U> &pleft,matrix<U> &&pright)
	{
		return std::move(pright)+pleft;
	}
	template<class U>
	friend matrix<U> operator +(const matrix<U> &pleft,const matrix<U> &pright)
	{
		matrix<T> tmp(pleft);
		return std::move(tmp)+pright;
	}
	template<class U>
	friend matrix<U> operator +(matrix<U> &&pleft,matrix<U> &&pright);

	/////multipliation functions and operators //

	template<class U>
	friend matrix<U> operator *(const matrix<U> &pleft,const matrix<U> &pright)
	{
		if(pleft.columns != pright.rows )
			throw ("pmat: Invalid dimension for multiplication");
		matrix<T> result(pleft.rows,pright.columns);

		short orient=result.rows>result.columns ? PMAT_PARTITION_BY_ROWS:PMAT_PARTITION_BY_COLUMNS;
		size_t partition_size= THE_SIZE ( orient , result )/pleft.max_thread_num;
		size_t thread_nums=0;
		if(partition_size > PMAT_MIN_PARTITION_SIZE )
			thread_nums = pleft.max_thread_num;
		else
		{
			partition_size = ( orient == PMAT_PARTITION_BY_ROWS ) ? MIN(result.rows,PMAT_MIN_PARTITION_SIZE):
				MIN(result.columns,PMAT_MIN_PARTITION_SIZE);
			thread_nums=( orient == PMAT_PARTITION_BY_ROWS ) ? result.rows/partition_size:result.columns/partition_size;
		}
		std::vector<std::thread> threads(partition_size);

		for(size_t partition=0;partition < partition_size ; ++partition )
		{
			
			threads[partition]=std::thread([&](){matrix<T>::multiply(result,pleft,pright,partition,partition_size,orient);});
		}
		for(size_t partition=0;partition < partition_size ; ++partition )
			threads[partition].join();
		return std::move(result);
	}
	template<class U>
	friend matrix<U> operator *(const U &pscalar,matrix<U> &pright)
	{
		short orient=pright.rows>pright.columns ? PMAT_PARTITION_BY_ROWS:PMAT_PARTITION_BY_COLUMNS;
		size_t partition_size= THE_SIZE ( orient , pright )/pright.max_thread_num;
		size_t thread_nums=0;
		matrix<U> result(pright);
		if(partition_size > PMAT_MIN_PARTITION_SIZE )
			thread_nums = pright.max_thread_num;
		else
		{
			partition_size = ( orient == PMAT_PARTITION_BY_ROWS ) ? MIN(pright.rows,PMAT_MIN_PARTITION_SIZE):
				MIN(pright.columns,PMAT_MIN_PARTITION_SIZE);
			thread_nums=( orient == PMAT_PARTITION_BY_ROWS ) ? pright.rows/partition_size:pright.columns/partition_size;
		}
		std::vector<std::thread> threads(thread_nums);
#ifdef PMAT_DEBUG
		std::cout << "**part size="<<partition_size <<" thread nums="<<thread_nums<<std::endl;
#endif 
		int s=2;
		for(size_t partition=0;partition < thread_nums ; ++partition )
		{
			threads[partition]=std::thread([&](){matrix<U>::multiply(result,pscalar,pright,partition,partition_size,orient);s++;});
		}
		std::cout <<" SSSSSSSSSSSSSSSSS="<<s<<std::endl;
		{
			std::vector<std::thread>::iterator iter=threads.begin();
			for(;iter!=threads.end();++iter)
			{
				std::cout <<" joining :" << iter->get_id()<<std::endl;
				iter->join();

			}
		}
		return std::move(result);
	}

	void add_row(const size_t prow,const std::vector<T> &prow_vector)
	{
		size_t last=NIN(prow_vector.size(),columns);
		size_t col=0;
		for(;col<columns && col<last;++col)
			(prow,col)=prow_vector[col];
		for(;col<columns;++col) // if vector data is no enough , fill reminded columns with zero
			(prow,col)=0;
	}
	template<class U>
	friend std::ostream &operator<< (std::ostream &pos,const matrix<U> &pmatrix)
	{
		for(size_t row=0;row<pmatrix.rows;++row)
		{
			for(size_t col=0;col<pmatrix.columns;++col)
				pos <<std::setw(5)<< pmatrix(row,col) << std::setprecision(3);
			pos<< std::endl;
		}
		return pos;
	}

};

#endif /* MATRIX_H_ */
