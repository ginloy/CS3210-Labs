/**
 * CS3210 - Blocking communication in MPI.
 */

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <mpi.h>
#include <string>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int numtasks, rank, dest, source, rc, count, tag = 1;
  char inmsg, outmsg = 'x';
  char hostname[256];
  MPI_Status Stat;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  memset(hostname, 0, sizeof(hostname));
  int sc_status = gethostname(hostname, sizeof(hostname) - 1);
  if (sc_status) {
    perror("gethostname");
    return sc_status;
  }

  float msg[10];

  if (rank == 0) {
    dest = 1;
    source = 1;
    rc = MPI_Send(&outmsg, 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
    rc = MPI_Recv(msg, 10, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &Stat);
    for (int i = 0; i < 10; ++i) {
      if (i != 0) {
        printf(" ");
      }
      printf("%f", msg[i]);
    }
    printf("\n");

  } else if (rank == 1) {
    for (int i = 0; i < 10; ++i) {
      msg[i] = i * 1.f;
    }
    dest = 0;
    source = 0;
    rc = MPI_Recv(&inmsg, 1, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);
    rc = MPI_Send(msg, 10, MPI_FLOAT, dest, tag, MPI_COMM_WORLD);
  }

  rc = MPI_Get_count(&Stat, MPI_CHAR, &count);
  printf("Task %d on %s: Received %d char(s) from task %d with tag %d \n", rank,
         hostname, count, Stat.MPI_SOURCE, Stat.MPI_TAG);

  MPI_Finalize();

  return 0;
}
