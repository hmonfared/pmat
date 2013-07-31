pmat( Parallel MATrix )
====

pmat is header only high performance matrix operation library based on c++11.
It uses c++11 threading, rvalue reference and moveing features from c++11. Also It is optimized to devide data
into smaller parts to be processed in the best way. For example for multplying A(2*5000) * B(5000*1000), data
partitioning will done by deviding '1000' in to 10( for example) threads instead of trying to devide data 
by rows of A 

Supported operations:
=====================
Add : C=A+B
Multiply : C=A*B   &  A=2*B
Scalar Value Assignment: A=2 ( fills all cells of A with 2)
