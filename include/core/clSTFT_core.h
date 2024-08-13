#ifndef CLSTFT_CORE
#define CLSTFT_CORE

#include "clSTFT_class.h"

#include <vector>
#include <complex>

class Data_real {
public:
    std::vector<std::complex<double>> array;  // 使用 std::vector 来存储数据
    int len;                 // 记录数据长度
    int maxPrintLen;
    Data_real() : len(0), maxPrintLen(10){}  // 构造函数

    ~Data_real() {}  // 析构函数

    void add(std::complex<double> value) {
        array.push_back(value);
        len++;
    }

    void show() {
        printf("Data_real array[%d]:\n", len);
        for (int i = 0; i < len && i < maxPrintLen; i++) {
            printf("%lf %lf\n", array[i].real(), array[i].imag());
        }
        printf("\n");
    }
};

void clSTFT_test();


void clSTFT_readFile(const char* ,Data_real* );

#endif