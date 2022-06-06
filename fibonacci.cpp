/*
 * Copyright (C) 2009-2021 Intel Corporation.
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>

static unsigned long Fibonacci(unsigned long);

int main(int argc, char** argv)
{
    if (argc > 2)
    {
        std::cerr << "Usage: fibonacci [num]" << std::endl;
        return 1;
    }

    unsigned long num = 1000;
    if (argc == 2)
    {
        std::istringstream is;
        is.str(argv[1]);
        is >> num;
        if (!is)
        {
            std::cerr << "Illegal number '" << argv[1] << "'" << std::endl;
            return 1;
        }
    }
    unsigned long sum = 0;
    for(unsigned long i = 0;i <= num;i++) {
      unsigned long fib = Fibonacci(num);
      sum += fib;
    }
    std::cout << "Entry number summing fibos until " << num << "  is " << sum << std::endl;
    return 0;
}

static unsigned long Fibonacci(unsigned long num)
{
  unsigned long a=0, b=1,n=0;

    while(n<num) {
      a = a+b;
      a^=b^=a^=b; //swap(a,b);
      n++;
    }
    return b;
}
