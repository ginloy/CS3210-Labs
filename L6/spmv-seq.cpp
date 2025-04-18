#include "spmv.cpp"

/**
 * Multiplies sparse matrix @a with vector @b
 * storing the result in vector @result
 *
 * Repeats this @repeat times to get a
 * representative runtime
 * 
 */
long long spmv(csr_matrix a, vector b, vector result)
{
    long long before, after;
 
    before = wall_clock_time();
    for (int r = 0; r < repeat; r++) {
        
        // The important bit
        for (size_t i = 0; i < size; i++) {
            size_t row_start = a.row_idx[i];
            size_t row_end   = a.row_idx[i+1];
            float sum = 0;
            for(size_t j = row_start; j < row_end; j++) {
                size_t a_col = a.col_idx[j];
                float a_coef = a.v[j];
                float b_coef = b[a_col];
                sum += a_coef * b_coef;
            }
            result[i] = sum;
        }

    }
    after = wall_clock_time();

    return after - before;
}

void work()
{
    csr_matrix a_c;
    vector b, result;
    long long time;

    allocate_vector(&b);
    allocate_vector(&result);

    init_matrix_csr(&a_c);
    init_vector(&b);
    init_vector(&result);

    printf("Compressed matrix with %lu non-zero elements\n", a_c.nnz);

    time = spmv(a_c, b, result);

    printf("%d multiplications took %1.2f us on average\n", repeat, ((float)time) / 1000 / repeat);

    printf("Check value: %16.18f\n", check_vector(&result));

    if (size <= 8) {
        print_matrix_csr(&a_c);
        printf("            *\n");
        print_vector(&b);
        printf("            =\n");
        print_vector(&result);
    }

    free_csr_matrix(&a_c);
    free_vector(b);
    free_vector(result);
}

int main(int argc, char** argv)
{
    printf("Usage: %s <size> <sparseness> <repeats>\n", argv[0]);

    size = 16384;
    sparseness = 1000;
    repeat = 100;

    if (argc >= 2)
        size = atoi(argv[1]);
    if (argc >= 3)
        sparseness = atoi(argv[2]) + 1;
    if (argc >= 4)
        repeat = atoi(argv[3]);

    printf("Sparse Matrix-Vector Multiplication of size %'ld, sparseness %3.6f%%\n",
        size * size,
        (float)(sparseness - 1)/(float)sparseness * 100.0
    );

    work();

    return 0;
}
