#pragma once

/**
 *
 * Sparse Matrix-Vector Product (SpMV)
 *
 * TKR
 *
 **/
#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <xmmintrin.h>
#include "timer.hpp"

#define SEED 69420

size_t size;

int sparseness, repeat;

typedef float* vector;

typedef struct
{
    float** element;
} matrix;

typedef struct
{
    float* v;
    size_t* col_idx;
    size_t* row_idx;
    size_t nnz;
} csr_matrix;


void allocate_vector(vector* v)
{
    // allocate array for all the rows
    *v = (float*)malloc(sizeof(float) * size);
    if (v == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
}


void allocate_matrix(matrix* m)
{
    // allocate array for all the rows
    m->element = (float**)malloc(sizeof(float*) * size);
    if (m->element == NULL) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }

    // allocate an array for each row of the matrix
    for (size_t i = 0; i < size; i++) {
        m->element[i] = (float*)malloc(sizeof(float) * size);
        if (m->element[i] == NULL) {
            fprintf(stderr, "Out of memory\n");
            exit(1);
        }
    }
}

void free_vector(vector v)
{
    free(v);
}

void free_matrix(matrix* m)
{
    for (size_t i = 0; i < size; i++) {
        free(m->element[i]);
    }
    free(m->element);
}

void free_csr_matrix(csr_matrix* m)
{
    free(m->v);
    free(m->row_idx);
    free(m->col_idx);
}

void init_vector(vector* v)
{
    srand(SEED);
    for (size_t i = 0; i < size; i++)
        (*v)[i] = (float)rand()/(float)(RAND_MAX/9);
}

void init_matrix_sparse(matrix* m)
{
    srand(SEED);
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            if (rand() % sparseness)
                m->element[i][j] = 0;
            else
                m->element[i][j] = (float)rand()/(float)(RAND_MAX/8) + 1;
        }
    }
}

void compress(matrix* m, csr_matrix* c)
{
    size_t nnz = 0;
    for (size_t i = 0; i < size; i++)
        for (size_t j = 0; j < size; j++)
            if (m->element[i][j] != 0.0)
                nnz++;

    c->v       = (float*)malloc(sizeof(float) * nnz);
    c->col_idx = (size_t*)malloc(sizeof(size_t) * nnz);
    c->row_idx = (size_t*)malloc(sizeof(size_t) * (size + 1));
    c->nnz     = nnz;

    nnz = 0;
    c->row_idx[0] = 0;
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            if (m->element[i][j] != 0.0) {
                c->v[nnz] = m->element[i][j];
                c->col_idx[nnz] = j;
                nnz++;
            }
        }
        c->row_idx[i+1] = nnz;
    }
}

void init_matrix_csr(csr_matrix* c)
{
    srand(SEED);
    size_t nnz = 0;
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            if (rand() % sparseness == 0) {
                rand();
                nnz++;
            }
        }
    }
    srand(SEED);

    c->v       = (float*)calloc(nnz, sizeof(float));
    c->col_idx = (size_t*)malloc(sizeof(size_t) * nnz);
    c->row_idx = (size_t*)malloc(sizeof(size_t) * (size + 1));
    c->nnz     = nnz;
    
    nnz = 0;
    c->row_idx[0] = 0;
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            if (rand() % sparseness == 0) {
                c->v[nnz] = (float)rand()/(float)(RAND_MAX/8) + 1;
                c->col_idx[nnz] = j;
                nnz++;
            }
        }
        c->row_idx[i+1] = nnz;
    }
}

double check_vector(vector* v)
{
    double t = 0.0;

    for (size_t i = 0; i < size; i++)
        t += (*v)[i];

    return t;
}

void print_vector(vector* m)
{
    printf("          ");
    for (size_t i = 0; i < size; i++) {
        printf("%6.2f  ", (*m)[i]);
    }
    printf("\n");
}

void print_matrix(matrix* m)
{
    for (size_t i = 0; i < size; i++) {
        printf("row %4lu: ", i);
        for (size_t j = 0; j < size; j++)
            printf("%6.2f  ", m->element[i][j]);
        printf("\n");
    }
}

void print_matrix_csr(csr_matrix* m)
{
    for (size_t i = 0; i < size; i++) {
        printf("row %4lu: ", i);
        size_t row_start = m->row_idx[i];
        size_t row_end   = m->row_idx[i+1];
        for (size_t col = 0; col < size; col++) {
            if (row_start < row_end && col == m->col_idx[row_start]) {
                printf("%6.2f  ", m->v[row_start]);
                row_start++; 
            } else
                printf("  0.00  ");
        }
        printf("\n");
    }
}
