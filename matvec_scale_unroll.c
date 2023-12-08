#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>

#define VAL_RANGE 1023

#define UNROLL 1

double mysecond() {
  struct timeval tp;
  struct timezone tzp;
  int i;
  i = gettimeofday(&tp,&tzp);
  return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}

double vec_scale(double *x, double *y, int n) {
  double res = 0.0;
  int i;

#pragma clang loop unroll_count(UNROLL)
  for (i = 0; i < n; i++) {
    x[i] = 2.7 * y[i];
  }
  return x[n - 1];
}

void matrix_vector_scale(double **mat, double *vec, double *result, 
			long long rows, long long cols) { 
  int i;
  for (i = 0; i < rows; i++)
    result[i] = vec_scale(mat[i], vec, cols);
}

void display_matrix(const double **matrix, long long N) {
  int i, j;
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) 
      printf("%3.4f ", matrix[i][j]);
    printf("\n");
  }
}
int main(int argc, char *argv[]) {

  double **matrix;
  double *vec;
  double *result;

  if (argc < 3) {
    printf("usage: \n");
    printf("       ./matvec N n\n");
    exit(0);
  }

  long long N = atoi(argv[1]);
  unsigned reps = atoi(argv[2]);

  int i, j;


  matrix = (double **) malloc(sizeof(double *) * N);
  for (i = 0; i < N; i++)
    matrix[i] = (double *) malloc(sizeof(double) * N);

  vec = (double *) malloc(sizeof(double) * N);
  result = (double *) malloc(sizeof(double) * N);

  for (i = 0; i < N; i++) 
    for (j = 0; j < N; j++) 
      matrix[i][j] = rand() / (double) (RAND_MAX/VAL_RANGE);
     
  for (i = 0; i < N; i++)
    vec[i] = rand() / (double) (RAND_MAX/VAL_RANGE);
  
  double t0 = mysecond();
  for (i = 0; i < reps; i++)
    matrix_vector_scale(matrix, vec, result, N, N);
  t0 = (mysecond() - t0) * 1.e3;
  
  fprintf(stdout, "Result = %3.4f\n", result[N - 1]);
  fprintf(stdout, "Scale Loop Time = %3.2f ms\n", t0);

  return 0;
}
