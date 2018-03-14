#include <stdio.h>
#include "scinfo.h"

static bool create_file_to_write(char* file_name, char* content, size_t file_length)
{
	FILE* fp = NULL;

	try
	{
		fp = fopen(file_name, "wb+x");
		if (fp == NULL)
			return false;
		
		fwrite(content, 1, file_length, fp);
		fclose(fp);
	}
	catch (...)
	{
		fclose(fp);
		return false;
	}

	return true;
}

int main()
{
	uint32_t data_len = 0;
	void *data = generate_sinf(12345, "test", 1994, data_len); // need that to compare with the new code
	create_file_to_write("test_file", (char *)data, data_len);
	return 0;
}
