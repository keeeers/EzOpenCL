#include "Vector_func.h"
#include <iostream>
#include <vector>
#include <functional>
#include <CL/cl.h>

using namespace std;


void arrayMultiplay_normal(int array_size = 1000) {
	cl_int err;
	//build array A,B,  result container C
	int lenA = array_size;
	int lenB = array_size;
	int lenC = array_size * 2 + 1;
	vector<int> A(lenA);
	vector<int> B(lenB);
	vector<int> C(lenC);

	for (int i = 0; i < lenA; ++i) {
		A[i] = static_cast<int>(i % 10);
	}
	for (int i = 0; i < lenB; ++i) {
		B[i] = static_cast<int>(i % 10);
	}
	for (int i = 0; i < lenA; i++) {
		for (int j = 0; j < lenB; j++) {
			C[i + j] += A[i] * B[j];
		}
		C[i + 1] += C[i] / 10;
		C[i] = C[i] % 10;
	}
	return;
}

