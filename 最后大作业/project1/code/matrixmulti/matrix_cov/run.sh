 mpicc matrixcov.c -o matrixcov -lm
 mpiexec -n 16 ./matrixcov