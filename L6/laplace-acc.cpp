#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "timer.hpp"

#ifndef WIDTH
#define WIDTH      1024
#endif
#ifndef HEIGHT
#define HEIGHT     1024
#endif
#ifndef TEMP_TOLERENCE
#define TEMP_TOLERANCE 0.01
#endif

double temp[HEIGHT+2][WIDTH+2];
double temp_prev[HEIGHT+2][WIDTH+2];

void initialize();

void track_progress(int iter);

int main(int argc, char *argv[]) {
    int itr = 0;
    double worst_dt = 100;
    long long start, stop;


    #pragma acc data create(temp_prev) create(temp)
    {

    initialize();
    start = wall_clock_time();
    while ( worst_dt > TEMP_TOLERANCE ) {

        #pragma acc parallel loop
        for(int i = 1; i <= HEIGHT; i++) {
            for(int j = 1; j <= WIDTH; j++) {
                temp[i][j] = 0.25 * (temp_prev[i+1][j]
                        + temp_prev[i-1][j]
                        + temp_prev[i][j+1]
                        + temp_prev[i][j-1]);
            }
        }

        worst_dt = 0.0;

        #pragma acc parallel loop reduction(max:worst_dt)
        for(int i = 1; i <= HEIGHT; i++){
            for(int j = 1; j <= WIDTH; j++){
                double dt = fabs(temp[i][j] - temp_prev[i][j]);
                temp_prev[i][j] = temp[i][j];
                worst_dt = fmax(dt, worst_dt);
            }
        }

        if((itr % 100) == 0) {
            #pragma acc update host(temp[HEIGHT])
            track_progress(itr);
        }

        itr++;
    }
    }
    
    stop = wall_clock_time();

    printf("\nMax error at iteration %d was %f\n", itr-1, worst_dt);
    printf("Total time was %1.6f ms.\n", (float)(stop - start) / 1000000);
}

void initialize(){
    #pragma acc parallel loop present(temp_prev)
    for(int i = 0; i <= HEIGHT+1; i++){
        for (int j = 0; j <= WIDTH+1; j++){
            temp_prev[i][j] = 0.0;
        }
    }
    #pragma acc parallel loop present(temp_prev)
    for(int i = 0; i <= HEIGHT+1; i++) {
        temp_prev[i][0] = 0.0;
        temp_prev[i][WIDTH+1] = (100.0/HEIGHT)*i;
    }
    #pragma acc parallel loop present(temp_prev)
    for(int j = 0; j <= WIDTH+1; j++) {
        temp_prev[0][j] = 0.0;
        temp_prev[HEIGHT+1][j] = (100.0/WIDTH)*j;
    }
}
void track_progress(int itr) {
    printf("---------- Iteration number: %d ------------\n",
            itr);
    for(int i = 0; i <= HEIGHT; i += (HEIGHT/8)) {
        printf("[%d,%d]: %5.2f  ", i, HEIGHT, temp[HEIGHT][i]);
    }
    printf("\n");
}
