#include <stdio.h>
#include <stdlib.h>
#define N 3000

long ** A;
long ** B;
long ** C;

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
    }
  }
}

int main() {
  initMat(&A,N);
  initMat(&B,N);
  initMat(&C,N);
  printf("mat Init complete\n");
  matMul(C,A,B, N);
  printf("mat Mul complete\n");
  freeMat(A,N);
  freeMat(B,N);
  freeMat(C,N);
  printf("mat Free complete\n");

  return 0;
}