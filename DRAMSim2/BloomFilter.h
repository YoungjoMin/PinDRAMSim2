#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H
#include <bitset>
#include <iostream>

/*
The particular hash functions used to index the Bloom filter are an implementation choice. However,
the effectiveness of our mechanism is largely insensitive to
the choice of hash function, since weak cells are already distributed randomly throughout DRAM [8]. The results presented in Section 6 use a hash function based on the xorshift
pseudo-random number generator [29], which in our evaluation is comparable in effectiveness to H3 hash functions that
can be easily implemented in hardware
*/
// Bloom Filter per rank, Paper uses xorshift hash.

// m must be power of 2


template <int mask>
class XorShift32  {
private:    
  uint32_t xorshift32(uint32_t x, int a, int b, int c) {
    x ^= x << a;
    x ^= x >> b;
    x ^= x << c;
    return x&mask;
  }
public:
  uint32_t operator() (uint32_t x , int idx) {
    const static int as[] = {1,2,3,3,5,5,7,9,13,1,2,3,3,5,6,7,10,13};
    const static int bs[] = {3,5,1,23,9,27,13,21,3,5,5,3,25,9,1,17,9,5};
    const static int cs[] = {10,15,14,25,28,28,25,16,27,16,21,26,24,31,11,21,21,19};
    return xorshift32(x, as[idx], bs[idx], cs[idx]);
  }
};

template <int m, int k>
class BloomFilter {
private:
  std::bitset<m> mask;
  XorShift32<m-1> hashs;
public:
  BloomFilter() {}
  void insert(int x) {
    for(int i=0;i<k;i++) {
      mask.set(hashs(x,i));
    }
  }
  bool exist(int x) {
    for(int i=0;i<k;i++) {
      if(!mask.test(hashs(x,i))) return false;
    }
    return true;
  }
};


#endif