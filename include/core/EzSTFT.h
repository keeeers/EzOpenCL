#ifndef EZCLASS_EZSTFT_H
#define EZCLASS_EZSTFT_H

#include "EzCLASS_data.h"
#include "EzCLASS_Window.h"
#include "EzCLASS_fft.h"

#include <string>


class EzCLASS_data;
class EzCLASS_window;
class EzCLASS_fft;


class EzCLASS_EzSTFT {
    /*
    * EzCLASS_EzSTFT contain all var and class needed in STFT
    * it include class:EzCLASS_data, EzCLASS_windw, EzCLASS_fft
    * pubilc args:
    *   FreqAxis, FreqMin, FreqMax, FreqRes, FreqCount;
    *   TimeCount, RemoveDC,
    *   DEBUG;
    * EzCLASS_EzSTFT 类包含了所有STFT所需的变量和类，
    * 其中包含三个类成员：EzCLASS_data、EzCLASS_windw、EzCLASS_fft
    * 公共通用参数：
    *   FreqAxis：频率轴类型，线性或对数
    *   FreqMin：频率最小值
    *   FreqMax：频率最大值
    *   FreqRes：频率分辨率
    *   FreqCount：频率数量
    *   TimeCount：时间数量
    *   RemoveDC：是否移除DC分量
    *   DEBUG：调试模式
    */
public:
    // commom args
    std::string FreqAxis; // liner or logarithmic
    double FreqMin;       // min value of frequency
    double FreqMax;       // max value of frequency
    double FreqRes;       // frequency resolution
    int FreqCount;        // frequency count
    int TimeCount;        // Times count
    bool RemoveDC;        // need to remove DC component
    int DEBUG;            // debug mode

    //  constructor class 
    EzCLASS_data* input_data; // input data class
	EzCLASS_window* Window_class; // window class
	EzCLASS_fft* Fft_class; // fft class

    // Init
    EzCLASS_EzSTFT();

    int Init_components_class();

private:
    // Init components class
    int _Init_input_data();
    int _Init_window_class();
    int _Init_fft_class();
	// Utils
	
	// __Debug
	
	// __Watcher

};


#endif