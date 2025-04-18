#include "mm.cpp"

/**
 * Multiplies matrix @a with matrix @b storing
 * the result in matrix @result
 *
 * The multiplication algorithm is the O(n^3)
 * algorithm
 */
void mm(matrix a, matrix b, matrix result) {
  int i, j, k;

  // Parallelize the multiplication
  // Iterations of the outer-most loop are divided
  //   amongst the threads
  // Variables are shared among threads (a, b, result)
  // and each thread has its own private copy (i, j, k)

#pragma acc parallel loop private(i, j, k) collapse(3)
  for (i = 0; i < size; i++)
    for (j = 0; j < size; j++)
      for (k = 0; k < size; k++)
        result.element[i][j] += a.element[i][k] * b.element[k][j];
}

void work() {
  matrix a, b, result;
  long long before, after;

  // Allocate memory for matrices
  allocate_matrix(&a);
  allocate_matrix(&b);
  allocate_matrix(&result);

  // Initialize matrix elements
  init_matrix(a);
  init_matrix(b);
  init_matrix(result);

  // Perform parallel matrix multiplication
  before = wall_clock_time();
  mm(a, b, result);
  after = wall_clock_time();
  fprintf(stderr, "Matrix multiplication took %1.6f ms\n",
          ((float)(after - before)) / 1000000);

  printf("Check value: %16.18f\n", check_matrix(result));

  free_matrix(&a);
  free_matrix(&b);
  free_matrix(&result);
}

int main(int argc, char **argv) {
  srand(0);

  printf("Usage: %s <size>\n", argv[0]);

  if (argc >= 2)
    size = atoi(argv[1]);
  else
    size = 1024;

  printf("Matrix multiplication of size %'d\n", size * size);

  work();

  return 0;
}
