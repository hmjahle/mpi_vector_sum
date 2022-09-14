#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

#define N 10

void Read_vector(
    double local_x[], 
    int local_n,
    int n,
    char vec_name[],
    int my_rank,
    MPI_Comm comm
);

void Print_vector(
    double local_z[],
    int local_n,
    int n,
    char title[],
    int my_rank,
    MPI_Comm comm
);

void Parallel_vector_sum(
    double local_u[],
    double local_v[],
    double local_z[],
    int local_n
);

int main() {
    int my_rank, comm_sz;
    int n = N, local_n;  // Size of u and v
    char u_name[] = "Vector U";
    char v_name[] = "Vector V";

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    local_n = n / comm_sz;

    double local_u[local_n], local_v[local_n];

    // Read u vector
    Read_vector(
        local_u, local_n, n, u_name, my_rank, MPI_COMM_WORLD 
    );
    // Read v vectpr
    Read_vector(
        local_v, local_n, n, v_name, my_rank, MPI_COMM_WORLD 
    );

    // Calculate result
    double local_result[local_n];
    char result_name[] = "Vector U + Vector V";
    Parallel_vector_sum(local_u, local_v, local_result, local_n);

    // Print result
    Print_vector(
        local_result, local_n, n, result_name, my_rank, MPI_COMM_WORLD
    );

    MPI_Finalize();
    return 0;
}

void Read_vector(
    double local_x[], 
    int local_n,
    int n,
    char vec_name[],
    int my_rank,
    MPI_Comm comm
) {
    double* a = NULL;

    if (my_rank == 0) {
        a = malloc(n * sizeof(double));
        printf("Enter the vector: %s\n", vec_name);
        for (int i = 0; i < n; i++) {
            scanf("%lf", &a[i]);
        }
        // Note that the send coult is equal to local_n not n
        MPI_Scatter(a, local_n, MPI_DOUBLE, local_x, local_n, MPI_DOUBLE, 0, comm);
        free(a);  // We are now finished with the memory required for a
    } else {
        MPI_Scatter(a, local_n, MPI_DOUBLE, local_x, local_n, MPI_DOUBLE, 0, comm);
    }
}

void Print_vector(
    double local_z[],
    int local_n,
    int n,
    char title[],
    int my_rank,
    MPI_Comm comm
) {
    double* z = NULL;

    if (my_rank == 0) {
        z = malloc(n * sizeof(double)); // Allocate memory to store the whole vector again

        // receive count = local_n since it is the count received from each process! 
        MPI_Gather(
            local_z, local_n, MPI_DOUBLE, z, local_n, MPI_DOUBLE, 0, comm
        );
        printf("%s\n", title);

        // This is the global i (i.e., the index of the whole vector)
        for (int i = 0; i < n; i++) {
            printf("%.2f ", z[i]);
        }
        printf("\n");
        free(z);
    } else {
        MPI_Gather(
            local_z, local_n, MPI_DOUBLE, z, local_n, MPI_DOUBLE, 0, comm
        );
    }
}

void Parallel_vector_sum(
    double local_u[],   /* in */
    double local_v[],   /* in */
    double local_z[],   /* in */
    int local_n         /* in */
) {
    for (int local_i = 0; local_i < local_n; local_i++) {
        local_z[local_i] = local_u[local_i] + local_v[local_i];
    }
}