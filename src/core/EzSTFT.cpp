#include "EzSTFT.h"


// Class EzCLASS_EzSTFT ;

// Pubilc 

// __Init 初始化
EzCLASS_EzSTFT::EzCLASS_EzSTFT() :
	FreqAxis("Linear"), FreqMin(0), FreqMax(0),
	FreqRes(1), FreqCount(1024), TimeCount(1000),
	RemoveDC(false), DEBUG(false) {};

int EzCLASS_EzSTFT::Init_components_class(){
	// 初始化成员类
	if (!_Init_input_data()){
		if (DEBUG) printf("EzCLASS_EzSTFT::_Init_input_data() failed.\n");
		return -1;
	}
	if (!_Init_window_class()){
		if (DEBUG) printf("EzCLASS_EzSTFT::_Init_window_class() failed.\n");
		return -1;
	}
	if (!_Init_fft_class()){
		if (DEBUG) printf("EzCLASS_EzSTFT::_Init_fft_class() failed.\n");
		return -1;
	}
	if (DEBUG){
		printf("EzCLASS_EzSTFT::Init_components_class() SUCCESS.\n");
	}
	return 0;
}

// Private

// Init components class 申请成员类
int EzCLASS_EzSTFT::_Init_input_data(){
	input_data = new EzCLASS_data;
	return 0;
}

int EzCLASS_EzSTFT::_Init_window_class(){
	Window_class = new EzCLASS_window;
	return 0;
}


int EzCLASS_EzSTFT::_Init_fft_class(){
	Fft_class = new EzCLASS_fft;
	return 0;
}

// Utils

// __Debug

// __Watcher