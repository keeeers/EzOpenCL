#include "clSTFT_class.h"

#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex>
#include <vector>

int clSTFT_class::createWindow() {
    if (WindowData == NULL) {
        WindowData = new double[WindowSize];  // 分配内存
    }

    WindowStep = WindowSize;

    if (WindowType == "Gauss") {
        double sigma = 0.5 * (double)WindowSize / 2.0;  // 标准差
        double center = (double)WindowSize / 2.0;  // 中心点
        for (int n = 0; n < WindowSize; n++) {
            double x = (double)n - center;  // 距离中心的距离
            WindowData[n] = exp(-0.5 * (x * x) / (sigma * sigma));  // 高斯窗函数
        }
    }

    if (DEBUG) {
        for (int i = 0; i < WindowSize; i++) {
            printf("%lf \n", WindowData[i]);
        }
    }

    return 1;
}

clSTFT_class::clSTFT_class()
    : FreqAxis("LinearAxis"), FreqMin(0.0), FreqMax(0.0),
    FreqResolution(0.0), FreqCount(256), TimeCount(2048),
    RemoveDC(true), WindowType("Gauss"), WindowSize(32),
    WindowData(NULL), DEBUG(false), WindowStep(26), SampleFreq(1000),
    class_fft(NULL){
        class_fft = new clFFT_class(this);
    }

int clSTFT_class::Init_Freq() {
    FreqMin = 0.0;
    FreqMax = SampleFreq / 2;
    FreqResolution = SampleFreq / WindowSize;
    FreqCount = WindowSize;
    return 1;
}

int clSTFT_class::show_arg() {
    /*
    std::string FreqAxis; // 线性或对数尺度
    double FreqMin;       // 频率最小值
    double FreqMax;       // 频率最大值
    double FreqResolution;// 频率分辨率
    int FreqCount;        // 频率方向上的网格点数
    int TimeCount;        // 时间方向上的网格点数
    bool RemoveDC;        // 是否去除直流分量
    std::string WindowType;// 窗函数类型
    int WindowSize;       // 窗函数长度
    double* WindowData;   // 窗函数数组
    int WindowStep;       // 窗口移动步长
    int DEBUG;            // debug观察
    double SampleFreq;    // 输入数据采样率*/
    printf("Freq Min:%10.2lf ; Max:%10.2lf ; Fs:%10.2lf\n", FreqMin, FreqMax, FreqResolution);
    printf("Window Type: %s ; Size:%5.d ; Step:%5.d\n", WindowType.c_str(), WindowSize, WindowStep);
    printf("TimeCount:%10.d ; FreqCount:%10.d\n", TimeCount, FreqCount);
    printf("DEBUG:%s\n", DEBUG ? "True" : "False");
    printf("SampleFreq:%10.2lf\n", SampleFreq);
    return 1;
}

int clSTFT_class::norm_STFT(std::complex<double> A[],int len) {
    //擦除数据
    norm_result.erase(norm_result.begin(), norm_result.end());
    //分布计算各段FFT
    for (int i = 0; i + WindowSize < len; i += WindowStep) {
        std::vector<std::complex<double>> B;
        B.clear();
        B.resize(0);
        norm_STFT_clip(&(A[i]), B);
        /*for (int j = 0; j < WindowSize; j++) {
            printf("%5.2lf ", std::abs(B[j]));
        }
        printf("\n");*/
        norm_result.push_back(B);
    }
    return 1;
}

int clSTFT_class::norm_STFT_clip(std::complex<double> A[],std::vector<std::complex<double>>& B) {
    int n = WindowSize ;
    for (int i = 0; i < n; i++) {
        B.push_back(std::complex<double>(A[i].real() * WindowData[i], A[i].imag() * WindowData[i]));
    }
    
    /*const double freqB = 120;
    for (int i = 0; i < n; i++) {
        B.push_back(std::complex<double>(sin(freqB * i / SampleFreq * 2 * acos(-1)), 0));
    }*/
    clSTFT_norm_FFT(&B[0]);
    return 1;
}

int clSTFT_class::show_norm_result() {
    const size_t freqlen = WindowSize;
    const size_t timelen = norm_result.size();

    printf("_________________norm_result_START_______________\n");
    for (int i = 0; i < timelen; i++) {
        std::vector<std::complex<double>> B = norm_result[i];
        for (int j = 0; j < freqlen / 2; j++) {
            printf("%10.3lf %10.3lf\n", B[j].real(), B[j].imag());
        }
        if (i >= 0) {
            break;
        }
    }
    printf("_________________norm_result_END_______________\n");


    //&&&&&&&&&&&&&&&&&&&&&&&&&& debug return
    return 1;

    // figuer max,min
    double mint=std::abs(norm_result[0][freqlen/2]), maxt = mint;
    for (int i = 0; i < timelen; i++) {
        std::vector<std::complex<double>> B = norm_result[i];
        for (int j = 2; j < freqlen/2; j++) {
            double abs_value = std::abs(B[j]);
            mint = std::min(mint, abs_value);
            maxt = std::max(maxt, abs_value);
        }
    }
    printf("maxt : %7.3lf  mint : %7.3lf\n", maxt, mint);
    if (maxt == 0 || maxt == mint) {
        printf("ERROR in clSTFT_class show_norm_result! Error maxt value!");
        return 0;
    }
    // normlize value 2 color_map
    std::vector<std::vector<double>> color_map(timelen, std::vector<double>(freqlen));
    for (int i = 0; i < timelen; i++) {
        for (int j = 0; j < freqlen; j++) {
            double normalized_value = (std::abs(norm_result[i][j]) - mint) / (maxt - mint);
            color_map[i][j] = normalized_value;
            //printf("%8.5lf", color_map[i][j]);
        }
        //printf("\n");
    }
    // 遍历 `color_map` 的每一个子向量
    for (int i = 0; i < color_map.size(); i++) {
        for (int j = WindowSize / 2; j < WindowSize; j++) {
            double normalized_value = color_map[i][j];
            // 根据归一化值设置不同的颜色
            if (normalized_value < 0.25) {
                std::cout << "\033[31m"; // 设置红色
                printf(" ");
            }
            else if (normalized_value < 0.5) {
                std::cout << "\033[33m"; // 设置黄色
                printf("^");
            }
            else if (normalized_value < 0.75) {
                std::cout << "\033[32m"; // 设置绿色
                printf("$");
            }
            else {
                std::cout << "\033[34m"; // 设置蓝色
                printf("@");
            }
            //printf("#");
            std::cout << "\033[0m"; // 重置颜色
        }
        printf("\n");
    }

    return 1;
}

int clSTFT_class::clSTFT_norm_FFT(std::complex<double> A[]) {
    int n = WindowSize ;
    // bit reversal
    int* pos = (int*)malloc(sizeof(int) * n+10);
    const double pi = acos(-1);
    pos[0] = 0;//第0个元素倒位序已知
    for (int i = 1; i < n; i++) {
        pos[i] = pos[i / 2] / 2 + (i % 2) * n / 2; //求倒位序
    }
    // cout << endl;
    for (int i = 0; i < n; i++) {
        if (i < pos[i]) {
            swap(A[i], A[pos[i]]); //根据倒位序原址交换
        }
    }
    free(pos);
    // butterfly diagram
    for (int l = 2; l <= n; l *= 2)
    {
        std::complex<double> W, Wk;
        for (int i = 0; i < n; i += l) {
            Wk = 1;
            W.real(cos(pi * 2 / l));
            W.imag(-sin(pi * 2 / l));
            for (int j = 0; j < l / 2; j++) {
                std::complex<double> x = A[i + j], y = A[i + j + l / 2] * Wk;//使用额外变量
                //printf("%d %d:%.1lf %.1lf %.1lf %.1lf\n", i, j, x.real(), x.imag(), y.real(), y.imag());
                A[i + j] = x + y;
                A[i + j + l / 2] = x - y;
                Wk *= W;
            }
        }
    }
    if (DEBUG) {
        for (int i = 0; i < n; i++) {
            double freq = (double)i * SampleFreq / (double)n;
            printf("%5.2lf : %5.2lf ::%5.2lf %5.2lf\n", freq, std::abs(A[i]), A[i].real(), A[i].imag());
        }
    }
    return 1;
}

int clSTFT_class::clSTFT_clfft(std::complex<double> A[]) {
    
    return 1;
}