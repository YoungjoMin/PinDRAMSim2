#include <iostream>
#include "BloomFilter.h"

void test1() {
  std::cout<< "test1 start!\n";
  BloomFilter<32, 3> filter;
  filter.insert(7);
  filter.insert(4591);
  filter.insert(19040);

  std::cout<< filter.exist(7)<<'\n';
  std::cout<< filter.exist(4591)<<'\n';
  std::cout<< filter.exist(19040)<<'\n';
  std::cout<< filter.exist(50)<<'\n';
  std::cout<< filter.exist(29)<<'\n';
  std::cout<< "test1 end!\n";
}

void test2() {
  std::cout<< "test2 start!\n";
  BloomFilter<256, 3> filter;
  for(int i= 0;i<100;i+=2) {
    filter.insert(i);
  }
  int falsePositiveCnt=0;
  for(int i= 1;i<100;i+=2) {
    falsePositiveCnt += filter.exist(i);
  }

  int falseNegativeCnt=0;
  for(int i= 0;i<100;i+=2) {
    falseNegativeCnt += !filter.exist(i);
  }
  std::cout<<"falsePositiveCnt:"<<falsePositiveCnt<<'\n';
  std::cout<<"falseNegativeCnt:"<<falseNegativeCnt<<'\n';
  std::cout<< "test2 end!\n";
}

int main() {
  test2();
  return 0;
}