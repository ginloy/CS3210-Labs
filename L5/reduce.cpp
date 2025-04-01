/**
 * CS3210 - Collective communication in MPI.
 */

#include <cstdio>
#include <cstdlib>
#include <mpi.h>

int main(int argc, char *argv[]) {
  int numtasks, rank, sendcount, recvcount, source;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

  source = 0;
  sendcount = 1;
  recvcount = 1;

  // Generate a random value for each process
  srand(rank);
  int localval = rand() % 10;
  printf("Rank %d generated value %d\n", rank, localval);

  int sum = 0;
  // TODO: Exercise 4
  MPI_Allreduce(&localval, &sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  printf("Rank %d, Total: %d\n", rank, sum);

  MPI_Finalize();

  return 0;
}
