// Matrix multiplication using threads

#include "smash.h"
#include <stdio.h>
#include <sys/time.h>

// size of square matrix
#define SIZE 30 

// square matrix; result should be Z = X*Y
int X[SIZE][SIZE];
int Y[SIZE][SIZE];
int Z[SIZE][SIZE];

// init all matrices
// randomally init X and Y 
// set Z elts to 0
void matrix_init() {
   int i,j;
   
  for (i = 0 ; i < SIZE; i++)
    for (j = 0 ; j < SIZE; j++) {
       X[j][i] = rand() % 1000;
       Y[j][i] = rand() % 1000;
       Z[j][i] = 0;
    }
}

// elt multiplication 
// 1 thread per elt in Z matrix
// yield for fairness after every mult
void elt_mult(cthread * c, int i, int j) {
   int k; 

   for (k = 0 ; k < SIZE; k++) {
       Z[i][j] += (X[i][k] * Y[k][j]);
       cthread_yield();
   }
}

// show matrix M
void matrix_show(int M[SIZE][SIZE]) {
  int i,j;

  for (i = 0 ; i < SIZE; i++) {
    for (j = 0 ; j < SIZE; j++) {
       fprintf(stderr,"%d\t",M[i][j]);
    }
    fprintf(stderr,"\n");
  }
  fprintf(stderr,"\n");
}


long calctime(struct timeval * t2, struct timeval *t1) {
   int temp = t2->tv_sec - t1->tv_sec;
   int temp2 = t2->tv_usec - t1->tv_usec;
   printf("%d seconds and %d microseconds\n",temp,temp2);
   return ((temp * 1000000) + temp2);
}


// main cthread function
void cthread_main() {
   int i,j;
   struct timeval t1,t2;
   cthread * c[SIZE][SIZE];
   
   srand(time(0));
   matrix_init();
     

   for (i = 0 ; i < SIZE; i++)
     for (j = 0 ; j < SIZE; j++)  
       c[i][j] = cthread_init(elt_mult,4096,2,i,j);
   
  gettimeofday(&t1,NULL);
   for (i = 0 ; i < SIZE; i++)
     for (j = 0 ; j < SIZE; j++)  
       cthread_run(c[i][j]);
   
  for (i = 0 ; i < SIZE; i++)
     for (j = 0 ; j < SIZE; j++)  
       cthread_join(c[i][j]);
  gettimeofday(&t2,NULL);
   
  matrix_show(X); 
  matrix_show(Y); 
  matrix_show(Z); 
  fprintf(stderr,"matrix multiply took %d microseconds\n", calctime(&t2,&t1));
}



