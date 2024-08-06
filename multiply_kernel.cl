__kernel void multiply_arrays(
			__global const int* A,
			__global const int* B,
			__global const int* len_A,
			__global const int* len_B,
			__global       int* C) 
{	
	__local int Alen;
	__local int Blen;
	Alen = *len_A;
	Blen = *len_B;
	int gid = get_global_id(0);
	if (gid >= Alen){
		return ;
	}
	for (int i=0;i < Blen; ++i){
		atomic_add(&C[gid+i],A[gid]*B[i]);
	}
}