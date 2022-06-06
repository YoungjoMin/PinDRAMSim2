#include <stdio.h>
#include <stdlib.h>
#define N 5000

void initMat(long *** arr, int n) {
  (*arr) = (long **)malloc(sizeof(long *) *n);
  for(int i= 0;i<n;i++) {
    (*arr)[i] = (long *)malloc(sizeof(long)*n);
    for(int j= 0;j<n;j++) {
      (*arr)[i][j] = rand();
    }
  }
}

void freeMat(long ** arr, int n) {
  for(int i= 0;i<n;i++) {
    free(arr[i]);
  }
  free(arr);
}

void matMul(long ** dest, long ** A, long ** B, int n) {
  for(int i= 0;i<n;i++) {
    for(int j= 0;j<n;j++) {
      dest[i][j]=0;
      for(int k=0;k<n;k++) {
        dest[i][j] += A[i][k]*B[k][j];
      }
      dest[i][j] %= 1000005;

    }
  }
}

int main() {
  long ** A, **B, **C, **D;
  initMat(&A,N);
  initMat(&B,N);
  initMat(&C,N);
  initMat(&D,N);
  printf("mat Init complete\n");
  while(1){
    for(int i= 0;i<10000;i++) {
      matMul(C,A,B, N);
      matMul(D,C,A, N);
      matMul(A,D,D, N);
      matMul(B,C,C, N);
    }
    printf("mat Mul complete %ld %ld %ld %ld\n", A[1][1], B[11][11], C[111][111], D[1111][1111]);
  }
  freeMat(A,N);
  freeMat(B,N);
  freeMat(C,N);
  freeMat(D,N);
  printf("mat Free complete\n");

  return 0;
}