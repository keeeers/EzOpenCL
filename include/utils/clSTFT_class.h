#ifndef CLSTFT_CLASS
#define CLSTFT_CLASS

#include <string>
#include <complex>
#include <vector>

class clSTFT_class
{
public:
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
    double SampleFreq;    // 输入数据采样率
    std::vector<std::vector<std::complex<double>>> norm_result; // 串行方法结果

    int createWindow();
    // 构造函数
    clSTFT_class()
        : FreqAxis("LinearAxis"), FreqMin(0.0), FreqMax(0.0),
        FreqResolution(0.0), FreqCount(256), TimeCount(2048),
        RemoveDC(true), WindowType("Gauss"), WindowSize(32),
        WindowData(NULL),DEBUG(false),WindowStep(26),SampleFreq(1000){}

    // 可以添加更多的成员函数，比如设置和获取参数的函数
    int show_arg();

    int norm_STFT(std::complex<double> A[],int n);

    int norm_STFT_clip(std::complex<double> A[], std::vector<std::complex<double>>& B);

    int show_norm_result();

    int clSTFT_norm_FFT(std::complex<double> A[]);

    int clSTFT_clfft(std::complex<double> A[]);
};

#endif // CLSTFT_CLASS


