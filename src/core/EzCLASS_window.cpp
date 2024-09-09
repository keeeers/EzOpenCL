#include "EzCLASS_Window.h"



//Public
// Class Initialization
EzCLASS_Window::EzCLASS_Window()
{
}

EzCLASS_Window::~EzCLASS_Window(){

}

int EzCLASS_Window::init(){

}

int EzCLASS_Window::initWindow(){

}

//Private

void EzCLASS_Window::createWindowData(){
    // 分配内存
    if (windowData != NULL) {
        delete[] windowData;
    }
    windowData = new double[windowSize];  

    // 创建高斯函数
    if (windowType == "Gauss") {
        double sigma = 0.5 * (double)windowSize / 2.0;  // 标准差
        double center = (double)windowSize / 2.0;  // 中心点
        for (int n = 0; n < windowSize; n++) {
            double x = (double)n - center;  // 距离中心的距离
            windowData[n] = exp(-0.5 * (x * x) / (sigma * sigma));  // 高斯窗函数
        }
    }

    return ;
}