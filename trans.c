/* 
 * Author: Griffin Bishop
 * Username: grbishop
 *
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    // May not modify array A. Only modify array B.
    int row, column, tmp;
    int diag_flag = 0;
    int diag_temp = 0;
    int blocksize = 8;
    if ((M == 32) && (N == 32)){
	blocksize = 8;
    } else if ((M == 64) && (N == 64)){
	blocksize = 4;
    } else {
	blocksize = 16;
    }

    int blocki, blockj;

    // Organize into blocks
    for (blockj = 0; blockj < M; blockj += blocksize){
	for (blocki = 0; blocki < N; blocki += blocksize){

	    for (row = blocki; (row < blocki + blocksize) && (row < N) ; ++row){
		for (column = blockj; (column < blockj + blocksize) && (column < M); ++column){
		    if (row == column){ // If it is a diagonal, 
					// save it to delay its computation to be more local.
 			diag_flag = 1;
			diag_temp = A[row][column];
			continue;
		    }
		    tmp = A[row][column];
		    B[column][row] = tmp;
		}
		if (diag_flag) { // If there was a diagonal, write it now.
		    B[row][row] = diag_temp; 
		    diag_flag = 0;
		}
	    }
	}
    }
}

// Do not grade this.
/*void transpose_submit2(int M, int N, int A[N][M], int B[M][N]){
    // May not modify array A. Only modify array B.
    //int row, column, tmp;
    int i;//, j;
    int x, y;
    int blocksize;
    if ((M == 32) && (N == 32)){
	blocksize = 8;
    } else if ((M == 64) && (N == 64)){
	blocksize = 4;
    } else {
	blocksize = 16;
    }

    blocksize = 4;


    int blocki, blockj;
    //set_matrix(M, N, A);
    //print_matrix(M, N, A); 

    // Organize into blocks
    for (blockj = 0; blockj < M; blockj += blocksize){
	for (blocki = 0; blocki < N; blocki += blocksize){
	    // Zig zag Accessing
	    x = 0;
	    y = 0;
	    B[blocki+x][blockj+y] = A[blockj+y][blocki+x];
	    printf("X: %d, Y: %d.\n", x, y);
	    for (i = blockj; i < blockj + blocksize; i++){
	    //while(x != blocksize ){
		//if (x == blocksize){
		    
		//}
		// Access the current coords, 
		
		// Then, update coordinates:
		
		// Shift x's over to y's
		while (x > 0 && y < blocksize){	    
		    x--;
		    y++;
		    if (y < blocksize && x < blocksize){
		    B[blocki+x][blockj+y] = A[blockj+y][blocki+x];
		    printf("X: %d, Y: %d.\n", x, y);
	  	    }
		}
		y++;
		if (y < blocksize && x < blocksize){
		    B[blocki+x][blockj+y] = A[blockj+y][blocki+x];
		    printf("X: %d, Y: %d.\n", x, y);
		}

		// Shift y's over to x's
		while (y > 0 && x < blocksize){ 
		    y--;
		    x++;
		    if (x < blocksize && y < blocksize){
		        B[blocki+x][blockj+y] = A[blockj+y][blocki+x];
		        printf("X: %d, Y: %d.\n", x, y);
		    }		    
		}
		x++;
		if (x < blocksize && y < blocksize){
		    B[blocki+x][blockj+y] = A[blockj+y][blocki+x];
		    printf("X: %d, Y: %d.\n", x, y);  
	 	}   
	    }

	}
    }
}*/

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    // Register your solution function //
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    // Register any additional transpose functions //
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

