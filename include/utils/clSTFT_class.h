#ifndef CLSTFT_CLASS
#define CLSTFT_CLASS

#include <string>
#include <complex>
#include <vector>

class clSTFT_class
{
public:
    std::string FreqAxis; // ���Ի�����߶�
    double FreqMin;       // Ƶ����Сֵ
    double FreqMax;       // Ƶ�����ֵ
    double FreqResolution;// Ƶ�ʷֱ���
    int FreqCount;        // Ƶ�ʷ����ϵ��������
    int TimeCount;        // ʱ�䷽���ϵ��������
    bool RemoveDC;        // �Ƿ�ȥ��ֱ������
    std::string WindowType;// ����������
    int WindowSize;       // ����������
    double* WindowData;   // ����������
    int WindowStep;       // �����ƶ�����
    int DEBUG;            // debug�۲�
    double SampleFreq;    // �������ݲ�����
    std::vector<std::vector<std::complex<double>>> norm_result; // ���з������

    int createWindow();
    // ���캯��
    clSTFT_class()
        : FreqAxis("LinearAxis"), FreqMin(0.0), FreqMax(0.0),
        FreqResolution(0.0), FreqCount(256), TimeCount(2048),
        RemoveDC(true), WindowType("Gauss"), WindowSize(32),
        WindowData(NULL),DEBUG(false),WindowStep(26),SampleFreq(1000){}

    // ������Ӹ���ĳ�Ա�������������úͻ�ȡ�����ĺ���
    int show_arg();

    int norm_STFT(std::complex<double> A[],int n);

    int norm_STFT_clip(std::complex<double> A[], std::vector<std::complex<double>>& B);

    int show_norm_result();

    int clSTFT_norm_FFT(std::complex<double> A[]);

    int clSTFT_clfft(std::complex<double> A[]);
};

#endif // CLSTFT_CLASS


