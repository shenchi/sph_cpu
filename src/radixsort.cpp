#include "radixsort.h"
#include <cstring>

namespace SPH{

	int count[256];
	int offsetTable[256];

	void radix(int byteIndex, int *source, int *dest, int *src_value, int *dest_value, int n)
	{
		memset(count, 0, 256 * sizeof(int));
		int byteOffset = byteIndex * 8;

		for (int i = 0; i < n; i++)
		{
			unsigned char radixValue = (unsigned char)((source[i] >> byteOffset) & 0xff);
			count[radixValue]++;
		}

		offsetTable[0] = 0;
		for (int i = 1; i < 256; i++)
		{
			offsetTable[i] = offsetTable[i - 1] + count[i - 1];
		}

		for (int i = 0; i < n; i++)
		{
			int index = offsetTable[(unsigned char)((source[i] >> byteOffset) & 0xff)]++;
			dest[index] = source[i];
			dest_value[index] = src_value[i];
		}
	}

	void RadixSort(int *key, int *value, int n){

		int * temp_key = new int[n];
		int * temp_value = new int[n];

		radix(0, key, temp_key, value, temp_value, n);
		radix(1, temp_key, key, temp_value, value, n);
		radix(2, key, temp_key, value, temp_value, n);
		radix(3, temp_key, key, temp_value, value, n);
		//radix(4, key, temp_key, value, temp_value, n);
		//radix(5, temp_key, key, temp_value, value, n);
		//radix(6, key, temp_key, value, temp_value, n);
		//radix(7, temp_key, key, temp_value, value, n);

		delete [] temp_key;
		delete [] temp_value;
	}
};