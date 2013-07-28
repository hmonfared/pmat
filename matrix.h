#ifndef _PMAPT_MATRX_H_
#define _PMAPT_MATRX_H_
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
	matrix<T> &operator =(matrix<T> &&pright); //
	matrix<T> &operator =(const matrix<T> &pright);
};

#endif //_PMAPT_MATRX_H_
