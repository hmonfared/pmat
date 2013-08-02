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

/*
 All partitions ( row or column blocks ) of matrix to be processed in a single thread must be greater than or equal to this value.
 this is for avoiding lots of small partitions which makes many threads and waste system resources.
 */
#define PMAT_MIN_PARTITION_SIZE 50  

/*
 returns rows of matrix if partitioning is based on rows of marix. else will return columns
 */
#define THE_SIZE(PART_BY,MAT) (( PART_BY == PMAT_PARTITION_BY_ROWS ) ? MAT.rows:MAT.columns)

/*
 returns min value of given pair
 */
#define MIN(a,b) ( ((a)<(b)) ? (a):(b) )

/*
 for less coding and aoding function calling, this is used instead of *this(i,j)
 */
#define __CELL_THIS(i,j) (*(data+(i)*columns+j))

/*
 for less coding and aoding function calling, this is used instead of mat(i,j)
 */
#define __CELL_THAT(mat,i,j) (*(mat.data+(i)*mat.columns+j))

/*
 this enum is used for identifying , how the matrix is partitioned for processing
 */
enum PMAT_PARTITION_BY {
	PMAT_PARTITION_BY_NONE = 0,
	PMAT_PARTITION_BY_ROWS = 1,
	PMAT_PARTITION_BY_COLUMNS = 2
};

/*
 this  is header only high performance matrix operation library based on c++11.
 It uses c++11 threading, rvalue reference and moveing features from c++11.
 Also It is optimized to devide data into smaller parts to be processed in the best way.
 For example for multplying A(2*5000) * B(5000*1000), data partitioning will done by deviding '1000' in to 10( for example)
 threads instead of trying to devide data by rows of A

 example :

 matrix<int> a(2,3);
 a=1; // sets all cells to 1;
 matrix<int> b=2*a;
 std::cout << b;

 */
template<class T>
class matrix {
private:
	T *data; // elements of matrix will be stored in data
	size_t rows; // number of rows
	size_t columns; // number of columns
	size_t max_thread_num; // maximum number of threads ( partitions ) which this matrix can be spreaded for processing
	std::string name_;
	/*
	 adds a subset of pleft and pright and sets in subset of presult.
	 because cells of matrix are stored in row level linear memory. its cache friendly to add two arrays
	 linearly. ( there is no need for result(i,j)=left(i,j)+right(i,j) )
	 */
	static void sum(matrix &presult, const matrix &pleft, const matrix &pright,
			const size_t ppartition_index, const size_t ppartition_size) {
		size_t
				part_end =
						MIN(ppartition_index*ppartition_size+ppartition_size,(pleft.rows*pleft.columns));
		size_t part_start = ppartition_index * ppartition_size;
		for (size_t cellcounter = part_start; cellcounter < part_end; ++cellcounter)
			*(presult.data + cellcounter) = *(pleft.data + cellcounter)
					+ (*(pright.data + cellcounter));
	}
	static void multiply(matrix &presult, const T &pscalar,
			const matrix &pright, const size_t ppartition_index,
			const size_t ppartition_size) {
		size_t
				part_end =
						MIN(ppartition_index*ppartition_size+ppartition_size,(pright.rows*pright.columns));
		size_t part_start = ppartition_index * ppartition_size;
		std::cout <<"in multiply\n";
		for (size_t cellcounter = part_start; cellcounter < part_end; ++cellcounter)
		{
			*(presult.data+cellcounter) = pscalar * *(pright.data+cellcounter);
			std::cout << presult.data[cellcounter];
		}
	}
	void transpose_block(matrix &presult, const size_t ppartition_index,
			const size_t ppartition_size, const short pparition_by) {
		size_t
				part_loop_end =
						MIN(ppartition_index*ppartition_size+ppartition_size,THE_SIZE(pparition_by,presult));
		size_t rowcounter, columncounter, rowend, columnend, rowstart,
				columnstart;
		presult.print_info();
		if (pparition_by == PMAT_PARTITION_BY_COLUMNS) {
			rowstart = 0;
			rowend = presult.rows;
			columnstart = ppartition_index * ppartition_size;
			columnend = part_loop_end;
		} else {
			rowstart = ppartition_index * ppartition_size;
			rowend = part_loop_end;
			columnstart = 0;
			columnend = presult.columns;
		}
		//TODO: continue from here
		T inner_sum;
		for (size_t row = rowstart; row < rowend; ++row)
			for (size_t col = columnstart; col < columnend; ++col)
				__CELL_THAT(presult,row,col)=__CELL_THIS(col,row);
				//presult(row, col) = 1;//T(this->operator()(row, col));

	}

	static void multiply(matrix &presult, const matrix &pleft,
			const matrix &pright, const size_t ppartition_index,
			const size_t ppartition_size, const short pparition_by) {
		size_t
				part_loop_end =
						MIN(ppartition_index*ppartition_size+ppartition_size,THE_SIZE(pparition_by,pright));
		size_t rowcounter, columncounter, rowend, columnend, rowstart,
				columnstart;
		if (pparition_by == PMAT_PARTITION_BY_COLUMNS) {
			rowstart = 0;
			rowend = pright.rows;
			columnstart = ppartition_index * ppartition_size;
			columnend = part_loop_end;
		} else {
			rowstart = ppartition_index * ppartition_size;
			rowend = part_loop_end;
			columnstart = 0;
			columnend = pright.columns;
		}
		//TODO: continue from here
		T inner_sum;
		for (size_t row = rowstart; row < rowend; ++row) {
			for (size_t col = columnstart; col < columnend; ++col) {
				inner_sum = 0;
				for (size_t inner = 0; inner < pright.rows; ++inner)
					inner_sum += __CELL_THAT(pleft,row,inner)
							*__CELL_THAT(pright,inner,col);
				__CELL_THAT(presult,row,col) = inner_sum;
			}
		}
	}

public:
	matrix() // default constructor
	{
		data = nullptr;
		columns = 0;
		rows = 0;
		max_thread_num = std::thread::hardware_concurrency();
		name_ = "";
	}
	matrix(matrix &&pother):data(nullptr),rows(0),columns(0),max_thread_num(std::thread::hardware_concurrency()) // move constructor
	{
#ifdef PMAT_DEBUG
		std::cout << "in move ctor"<< std::endl;
#endif

		*this=std::move(pother);
	}
	matrix(matrix const &pother) :
		data(nullptr), rows(pother.rows), columns(pother.columns),
				max_thread_num(pother.max_thread_num) // copy constructor
	{
#ifdef PMAT_DEBUG
		std::cout << "in copy ctor" << std::endl;
#endif

		data = new T[pother.rows * pother.columns]; // alocate memory for data
		name_ = "copy()_of_" + pother.name_;
		memcpy(data, pother.data, sizeof(T) * pother.rows * pother.columns); // copy data to allocated space

	}
	matrix(size_t prows, size_t pcolumns) {
		if (prows <= 0 || pcolumns <= 0) {
			throw("pmat exception: Invalid row/column size");
		}
		data = new T[prows * pcolumns];
		rows = prows;
		columns = pcolumns;
		max_thread_num = std::thread::hardware_concurrency();
		name_ = "";
	}
	void setsize(size_t prows, size_t pcolumns) {
		if (prows <= 0 || pcolumns <= 0)
			throw("pmat exception: invalid row/column size");
		if (data)
			delete data;
		data = new T[prows * pcolumns];
		rows = prows;
		columns = pcolumns;
	}
	void set_name(const std::string &pname) {
		name_ = pname;
	}
	void print_info() {
		std::cout << "name=" << name_ << " rows=" << rows << " columns= "
				<< columns << std::endl;
	}
	void set_max_thread_num(const size_t pthread_num) {
		if (pthread_num < 1)
			throw("pmat exception: Invalid thread number");
		max_thread_num = pthread_num;
	}
	virtual ~matrix(void) {
		if (data)
			delete[] data;
	}

	T& operator()(size_t prow, size_t pcolumn) {
		if (!data)
			throw("pmat exception:no memory allocated for matrix");
		if (prow < 0 || prow >= rows || pcolumn < 0 || pcolumn >= columns) {
			print_info();
			std::cout << " invalid row: " << prow << "/column :" << pcolumn
					<< std::endl;
			throw ("pmat exception: invalid row/column specified");
		}
		return __CELL_THIS(prow,pcolumn); // data[prow*columns+pcolumn];
	}
	T operator()(size_t prow, size_t pcolumn) const {
		if (!data)
			throw("pmat exception:no memory allocated for matrix");
		if (prow < 0 || prow >= rows || pcolumn < 0 || pcolumn >= columns) {
			print_info();
			std::cout << " invalid row: " << prow << "/column :" << pcolumn
					<< std::endl;
			throw ("pmat exception: invalid row/column specified");
		}
		return __CELL_THIS(prow,pcolumn);
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
		max_thread_num=pright.max_thread_num;
		columns=pright.columns;
		name_="move_of_"+pright.name_;
		pright.data=nullptr;
		pright.rows=0;
		pright.columns=0;


		return *this;
	}
	matrix &operator =(const matrix &pright) {
#ifdef PMAT_DEBUG
		std::cout << "in copy = " << std::endl;
#endif

		if (this == &pright)
			return *this;
		if (data)
			delete[] data; // FIXME: memory realloc is better choice, (... care about shrinking in thid way )
		data = new T[pright.rows * pright.columns];
		memcpy(data, pright.data, sizeof(T) * pright.rows * pright.columns);
		rows = pright.rows;
		columns = pright.columns;
		name_="copy(=)_of_"+pright.name_;
		return *this;
	}
	matrix &operator =(const T &pright) {
		if (!data)
			return *this;
		std::fill(data, data + rows * columns, pright);
	}
	/*
	 Transposes itself
	 */
	void transpose() {
		if (columns == 1 || rows == 1) {
			std::swap(columns, rows);
			return;
		}

		matrix<T> result(columns, rows);
		result.set_name("transpose_of_"+name_);
		result.max_thread_num = max_thread_num;
		short orient = result.rows > result.columns ? PMAT_PARTITION_BY_ROWS
				: PMAT_PARTITION_BY_COLUMNS;
		size_t partition_size = THE_SIZE ( orient , result ) / max_thread_num;
		size_t thread_nums = 0;
		if (partition_size > PMAT_MIN_PARTITION_SIZE)
			thread_nums = max_thread_num;
		else {
			partition_size
					= (orient == PMAT_PARTITION_BY_ROWS) ? MIN(result.rows,PMAT_MIN_PARTITION_SIZE)
							: MIN(result.columns,PMAT_MIN_PARTITION_SIZE);
			thread_nums = (orient == PMAT_PARTITION_BY_ROWS) ? result.rows
					/ partition_size : result.columns / partition_size;
		}
		std::vector<std::thread> threads(thread_nums);

		for (size_t partition = 0; partition < thread_nums; ++partition) {
			//threads[partition]=std::thread([&](){matrix<T>::transpose_block(result,partition,partition_size,orient);});
			threads[partition] = std::thread(&matrix<T>::transpose_block, this,
					std::ref(result), partition, partition_size, orient);
		}
		for (size_t partition = 0; partition < thread_nums; ++partition)
			threads[partition].join();

		*this = std::move(result);
	}
	/*
	 make a copy of this matrix, then transpose, and return it
	 */
	matrix<T> get_transpose() {
		matrix<T> result(*this); // make a copy
		result.transpose();
		return std::move(result);
	}

	// summation functions and operators
	template<class U>
	friend matrix<U> operator +(matrix<U> &&pleft,const matrix<U> &pright)
	{
		size_t total_size=pleft.rows*pleft.columns;
		size_t partition_size= total_size /pleft.max_thread_num;
		size_t thread_nums=0;
		if(partition_size > PMAT_MIN_PARTITION_SIZE )
		thread_nums = pleft.max_thread_num;
		else
		thread_nums=total_size/partition_size;
		std::vector<std::thread> threads(thread_nums);
#ifdef PMAT_DEBUG
		std::cout << "++part size="<<partition_size <<" thread nums="<<thread_nums<<std::endl;
#endif 
		for(size_t partition=0;partition < thread_nums; ++partition )
		{
			threads[partition]=std::thread([&]() {matrix<T>::sum(pleft,pleft,pright,partition,partition_size);});
		}
		for(size_t partition=0;partition <thread_nums; ++partition )
		threads[partition].join();
		return std::move(pleft);
	}
	template<class U>
	friend matrix<U> operator +(const matrix<U> &pleft,matrix<U> &&pright)
	{
		return std::move(pright)+pleft;
	}
	template<class U>
	friend matrix<U> operator +(const matrix<U> &pleft, const matrix<U> &pright) {
		matrix<T> tmp(pleft);
		return std::move(tmp) + pright;
	}
	template<class U>
	friend matrix<U> operator +(matrix<U> &&pleft,matrix<U> &&pright);

	/////multipliation functions and operators //

	template<class U>
	friend matrix<U> operator *(const matrix<U> &pleft, const matrix<U> &pright) {
		if (pleft.columns != pright.rows)
			throw("pmat: Invalid dimension for multiplication");
		matrix<T> result(pleft.rows, pright.columns);
		result.set_max_thread_num(pleft.max_thread_num);
		short orient = result.rows > result.columns ? PMAT_PARTITION_BY_ROWS
				: PMAT_PARTITION_BY_COLUMNS;
		size_t partition_size = THE_SIZE ( orient , result )
				/ pleft.max_thread_num;
		size_t thread_nums = 0;
		if (partition_size > PMAT_MIN_PARTITION_SIZE)
			thread_nums = pleft.max_thread_num;
		else {
			partition_size
					= (orient == PMAT_PARTITION_BY_ROWS) ? MIN(result.rows,PMAT_MIN_PARTITION_SIZE)
							: MIN(result.columns,PMAT_MIN_PARTITION_SIZE);
			thread_nums = (orient == PMAT_PARTITION_BY_ROWS) ? result.rows
					/ partition_size : result.columns / partition_size;
		}
		std::vector<std::thread> threads(partition_size);

		for (size_t partition = 0; partition < partition_size; ++partition) {

threads		[partition]=std::thread([&]() {matrix<T>::multiply(result,pleft,pright,partition,partition_size,orient);});
	}
	for(size_t partition=0;partition < partition_size; ++partition )
	threads[partition].join();
	return std::move(result);
}
template<class U>
friend matrix<U> operator *(const U &pscalar,const matrix<U> &pright)
{
	matrix<U> result(pright);
	size_t total_size=pright.rows*pright.columns;
	size_t partition_size= total_size /pright.max_thread_num;
	size_t thread_nums=0;
	if(partition_size > PMAT_MIN_PARTITION_SIZE )
	thread_nums = pright.max_thread_num;
	else
	thread_nums=total_size/partition_size;
	std::vector<std::thread> threads(thread_nums);
#ifdef PMAT_DEBUG
	std::cout << "**part size="<<partition_size <<" thread nums="<<thread_nums<<std::endl;
#endif 
	for(size_t partition=0;partition < thread_nums; ++partition )
	{
		threads[partition]=std::thread([&]() {matrix<U>::multiply(std::ref(result),pscalar,std::ref(pright),partition,partition_size);});
	}
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
	__CELL_THIS(prow,col)=prow_vector[col];
	for(;col<columns;++col) // if vector data is no enough , fill reminded columns with zero
	__CELL_THIS(prow,col)=0;
}
template<class U>
friend std::ostream &operator<< (std::ostream &pos,const matrix<U> &pmatrix)
{
	for(size_t row=0;row<pmatrix.rows;++row)
	{
		for(size_t col=0;col<pmatrix.columns;++col)
		pos <<std::setw(5)<< __CELL_THAT(pmatrix,row,col) << std::setprecision(3);
		pos<< std::endl;
	}
	return pos;
}

};

#endif /* MATRIX_H_ */
