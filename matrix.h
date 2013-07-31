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
#define PMAT_MIN_PARTITION_SIZE 30
#define THE_SIZE(PART_BY,MAT) (( PART_BY == PMAT_PARTITION_BY_ROWS ) ? MAT.rows:MAT.columns)
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
	void sum(
		matrix &presult,
		const matrix &pleft,
		const matrix &pright,
		const size_t ppartition_index,
		const size_t ppartition_size,
		const short pparition_by);
public:
	matrix(void);
	matrix(matrix &&pother); // move constructor
	matrix(matrix &pother); // copy constructor
	matrix(size_t prows,size_t pcolumns);
	void setsize(size_t prows,size_t pcolumns);
	void set_max_thread_num(const size_t pthread_num);
	virtual ~matrix(void);

	T& operator()(size_t prow,size_t pcolumn);

	matrix &operator =(matrix &&pright); //
	matrix &operator =(const matrix &pright);
	/***************************** summation functions and operators ******************************************/
	void sum(matrix<T> &presult,const matrix<T> &pleft,const matrix<T> &pright,const size_t ppartition_index,const size_t ppartition_size,const short pparition_by);
	template<class U>
	friend matrix<U> operator +(matrix<U> &&pleft,const matrix<U> &pright);
	template<class U>
	friend matrix<U> operator +(const matrix<U> &pleft,matrix<U> &&pright);
	template<class U>
	friend matrix<U> operator +(const matrix<U> &pleft,const matrix<U> &pright);
	template<class U>
	friend matrix<U> operator +(matrix<U> &&pleft,matrix<U> &&pright);

	/***************************** multipliation functions and operators ******************************************/
	void multiply(matrix &presult,const T &pscalar,const matrix &pright,const size_t ppartition_index,const size_t ppartition_size,const short pparition_by);
	void multiply(matrix &presult,const matrix &pleft,const matrix &pright,const size_t ppartition_index,const size_t ppartition_size,const short pparition_by);

	template<class U>
	friend matrix<U> operator *(const matrix<U> &pleft,const matrix<U> &pright);
	template<class U>
	friend matrix<U> operator *(const double &pleft,const matrix<U> &pright);
	
	void add_row(const size_t prow,const std::vector<T> &prow_vector);
	friend std::ostream &operator<<(std::ostream &pos,const matrix &pmatrix);

};

#endif /* MATRIX_H_ */
