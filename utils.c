//Copyright <2022> <Jesse Cox>
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this 
//software and associated documentation files (the "Software"), to deal in the Software 
//without restriction, including without limitation the rights to use, copy, modify, 
//merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
//permit persons to whom the Software is furnished to do so, subject to the following:

//The above copyright notice and this permission notice shall be included in all copies
//or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
//INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
//PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
//HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
//CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
//OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>

// Function to swap two numbers
void swap(char *x, char *y) {
	char t = *x; *x = *y; *y = t;
}
 
// Function to reverse `buffer[i�j]`
char* reverse(char *buffer, int i, int j)
{
	while (i < j) {
		swap(&buffer[i++], &buffer[j--]);
	}
 
	return buffer;
}
 
// Iterative function to implement `itoa()` function in C
char* itoa(int value, char* buffer, int base)
{
	// invalid input
	if (base < 2 || base > 32) {
		return buffer;
	}
 
	// consider the absolute value of the number
	int n = abs(value);
 
	int i = 0;
	while (n)
	{
		int r = n % base;
 
		if (r >= 10) {
			buffer[i++] = 65 + (r - 10);
		}
		else {
			buffer[i++] = 48 + r;
		}
 
		n = n / base;
	}
 
	// if the number is 0
	if (i == 0) {
		buffer[i++] = '0';
	}
 
	// If the base is 10 and the value is negative, the resulting string
	// is preceded with a minus sign (-)
	// With any other base, value is always considered unsigned
	if (value < 0 && base == 10) {
		buffer[i++] = '-';
	}
 
	buffer[i] = '\0'; // null terminate string
 
	// reverse the string and return it
	return reverse(buffer, 0, i - 1);
}

char *return_default_labe(int mp, char * dir, int cp)
{
	char *out;
	asprintf(&out, "m%i%s%i", (mp + 1),dir, (cp + 1));
	return out;
}

uint32_t get_time_millis(void)
{
	uint32_t uiNow;
	struct timeval tvNow;
	gettimeofday(&tvNow, NULL);
	uiNow = tvNow.tv_usec;
	return uiNow;
}

unsigned long current_timestamp() {
	struct timeval te; 
	gettimeofday(&te, NULL); // get current time
	long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
	// printf("milliseconds: %lld\n", milliseconds);
	return milliseconds;
}