 mpicc Matrix_multi.c -o Matrix_multi -lm
 mpiexec -n 16 ./Matrix_multi