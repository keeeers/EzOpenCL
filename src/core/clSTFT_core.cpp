#include "clSTFT_class.h"
#include "clSTFT_core.h"
#include "time_chrono.h"

#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex>


void clSTFT_test() {
    //开启观测模式
    TimeKeeper timedog;
    clSTFT_class stft;
    stft.DEBUG = false;
    Data_real input_data;
    const char* filename;
    filename = "./data/100.txt";
    timedog.Init();
    clSTFT_readFile(filename, &input_data);
    //input_data.show();
    timedog.dur_withPre_Init("readFile");
    //创建窗函数数组
    stft.WindowSize = 64;
    stft.WindowType = "Gauss";
    stft.createWindow();
    stft.WindowStep = 100;
    timedog.dur_withPre_Init("create Window");

    //input_data.show();
    std::vector<std::complex<double>> B;
    stft.norm_STFT_clip(&input_data.array[100],B);
    timedog.dur_withPre_Init("norm_STFT_clip");
    stft.norm_STFT(&input_data.array[0],input_data.len);
    timedog.dur_withPre_Init("norm_STFT");
    stft.show_arg();
    //stft.norm_STFT(&input_data.array[0], input_data.len/100);
    //stft.show_norm_result();
}



void clSTFT_readFile(const char* filename,Data_real* Data) {
    FILE* fp = fopen(filename, "r");  // 打开文件以文本模式读取
    if (fp == NULL) {
        fprintf(stderr, "Failed to open file '%s'\n", filename);
        exit(EXIT_FAILURE);  // 如果打开失败则退出程序
    }

    char line[256];  // 定义一个足够大的缓冲区来存储一行数据
    int number;  // 用于存储读取到的数字

    // 循环读取每一行直到 EOF
    int min=2100000000, max=-2100000000;
    while (fgets(line, sizeof(line), fp)) {
        // 尝试从这一行读取一个整数
        if (sscanf(line, "%d", &number) == 1) {
            Data -> add(std::complex<double>(((double)number),0.0));
            //min = std::min(number, min);
            //max = std::max(number, max);
        } 
        else {
            // 如果读取失败，可能是这一行为空或者格式不正确
            printf("Failed to read a number from line: '%s'\n", line);
        }
    }
    /*for (int i = 0; i < Data->len; i++) {
        Data->array[i] = std::complex<double>(((Data->array[i].real()-min)/(max-min)), 0.0);
    }*/
    fclose(fp);  // 关闭文件
}