#include "file.h"
#include <cstdio>
#include <corecrt_malloc.h>
#include <iostream>
#include <stdio.h>


void readCLs(char* Filename, char* program_buffer) {
	/* Read program file and place content into buffer */
	FILE* program_handle;
	size_t program_size;
	program_handle = fopen(Filename, "rb");
	if (program_handle == NULL) {
		perror("Couldn't find the program file");
		return ;
	}
	fseek(program_handle, 0, SEEK_END);
	program_size = ftell(program_handle);
	rewind(program_handle);
	std::cout << "readfile" << std::endl;
	if (program_buffer) {
		std::cout << "writefile" << std::endl;
		program_buffer = (char*)malloc(program_size + 1);
		program_buffer[program_size] = '\0';
		fread(program_buffer, sizeof(char), program_size, program_handle);
		std::cout << "writefile finished" << std::endl;
	}
	fclose(program_handle);
}