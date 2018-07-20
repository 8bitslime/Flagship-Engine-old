#ifdef _WIN32
#include "flagship.h"
#include "console.h"
#include <stdio.h>
#include <stdarg.h>
#include <windows.h>

void fatal_error(const char *format, ...) {
	char buffer[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, 1024, format, args);
	va_end(args);
	
	MessageBoxA(NULL, buffer, "Flagship: Fatal Error!", MB_ICONERROR);
	
	//dump console buffer
	buffer_t *console_buffer = con_buffer();
	if (console_buffer != NULL) {
		FILE *file = fopen("stderr.txt", "w");
		if (file == NULL) {
			goto end;
		}
		
		fwrite(console_buffer->data, sizeof(char), console_buffer->size, file);
		fprintf(file, "\nError message: %s", buffer);
		fclose(file);
	}
	
	end:
	exit(EXIT_FAILURE);
}

#endif
