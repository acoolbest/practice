#include <iostream>
#include <openssl/evp.h>
#include <string.h>
//g++ key.cpp -lcrypto
using namespace std;

int main()
{
	// base64编码
	unsigned char out[1000] = {0};
	const unsigned char enc_data[] = "hello world";
	
	cout << EVP_EncodeBlock(out, enc_data, strlen((const char *)enc_data)) << endl;
	cout << out << endl;
 
	// base64解码
	unsigned char recover_data[1000] = {0};

	cout << EVP_DecodeBlock(recover_data, out, strlen((const char *)out)) << endl;
	cout << recover_data << endl;

	return 0;
}