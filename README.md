pmat
====

pmat is header only high performance matrix operation library based on c++11.
It uses c++11 threading and rvalue references features from c++11. Also It is optimized to devide data
into smaller parts to be processed in the best way. For example for multplying A(2*5000) * B(5000*1000), data
partitioning will done by deviding '1000' in to 10( for example) threads instead of trying to devide data 
by rows of A (2)
