#include <stdio.h>
#include <fstream>
#include <string.h>
#include "evp_encrypt.h"

using namespace std;

#define			CRG_FAIL 							0
#define			CRG_SUCCESS 						1
#define			AES_KEY_IV_CIPHETREXT				"aes.key.iv"
#define			PRINT_POINT_STR						" ----------> "
#define			PRINT_ERR_FILE_STR					"[ERROR FILE]\n"
#define			PRINT_FAIL_STR						"[FAIL]\n"


// g++ -Wall -std=c++11 evp_encrypt.cpp -o evp-encrypt.exe -lcrypto
static uint8_t g_local_key[KEY_SIZE] = {0};
static uint8_t g_local_iv[BLOCK_SIZE] = {0};
static uint8_t g_key[10*KEY_SIZE] = {0};
static uint8_t g_iv[100*BLOCK_SIZE] = {0};

void gen_params(byte key[KEY_SIZE], byte iv[BLOCK_SIZE])
{
    int rc = RAND_bytes(key, KEY_SIZE);
    if (rc != 1)
      throw std::runtime_error("RAND_bytes key failed");

    rc = RAND_bytes(iv, BLOCK_SIZE);
    if (rc != 1)
      throw std::runtime_error("RAND_bytes for iv failed");
}

static uint32_t aes_encrypt(const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE], const secure_string& ptext, secure_string& ctext)
{
    EVP_CIPHER_CTX_free_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
    int rc = EVP_EncryptInit_ex(ctx.get(), EVP_aes_128_cbc(), NULL, key, iv);
	if (rc != 1)
    {
		//throw std::runtime_error("EVP_EncryptInit_ex failed");
		printf("EVP_EncryptInit_ex failed\n");
		return CRG_FAIL;
	}

    // Recovered text expands upto BLOCK_SIZE
    ctext.resize(ptext.size()+BLOCK_SIZE);
    int out_len1 = (int)ctext.size();

    rc = EVP_EncryptUpdate(ctx.get(), (byte*)&ctext[0], &out_len1, (const byte*)&ptext[0], (int)ptext.size());
  	if (rc != 1)
    {
		//throw std::runtime_error("EVP_EncryptUpdate failed");
		printf("EVP_EncryptUpdate failed\n");
		return CRG_FAIL;
	}
	
    int out_len2 = (int)ctext.size() - out_len1;
    rc = EVP_EncryptFinal_ex(ctx.get(), (byte*)&ctext[0]+out_len1, &out_len2);
  	if (rc != 1)
    {
		//throw std::runtime_error("EVP_EncryptFinal_ex failed");
		printf("EVP_EncryptFinal_ex failed\n");
		return CRG_FAIL;
	}
    // Set cipher text size now that we know it
    ctext.resize(out_len1 + out_len2);
	return CRG_SUCCESS;
}

static uint32_t aes_decrypt(const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE], const secure_string& ctext, secure_string& rtext)
{
    EVP_CIPHER_CTX_free_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
    int rc = EVP_DecryptInit_ex(ctx.get(), EVP_aes_128_cbc(), NULL, key, iv);
	if (rc != 1)
    {
		//throw std::runtime_error("EVP_DecryptInit_ex failed");
		printf("EVP_DecryptInit_ex failed\n");
		return CRG_FAIL;
	}

    // Recovered text contracts upto BLOCK_SIZE
    rtext.resize(ctext.size());
    int out_len1 = (int)rtext.size();

    rc = EVP_DecryptUpdate(ctx.get(), (byte*)&rtext[0], &out_len1, (const byte*)&ctext[0], (int)ctext.size());
	if (rc != 1)
    {
		//throw std::runtime_error("EVP_DecryptUpdate failed");
		printf("EVP_DecryptUpdate failed\n");
		return CRG_FAIL;
	}

    int out_len2 = (int)rtext.size() - out_len1;
    rc = EVP_DecryptFinal_ex(ctx.get(), (byte*)&rtext[0]+out_len1, &out_len2);
    if (rc != 1)
    {
		//throw std::runtime_error("EVP_DecryptFinal_ex failed");
		printf("EVP_DecryptFinal_ex failed\n");
		return CRG_FAIL;
	}
    // Set recovered text size now that we know it
    rtext.resize(out_len1 + out_len2);
	return CRG_SUCCESS;
}


uint32_t aes_cbc_encrypt(const secure_string& ptext, secure_string& ctext)
{
	return aes_encrypt(g_key, g_iv, ptext, ctext);
}

uint32_t aes_cbc_decrypt(const secure_string& ctext, secure_string& rtext)
{
	return aes_decrypt(g_key, g_iv, ctext, rtext);
}

static uint32_t read_aes_key_iv(string read_path, uint8_t *key, uint8_t *iv, bool is_decrypt = true)
{
	uint32_t ret = CRG_FAIL;

	secure_string strline = "";
	secure_string recover_key = "";
	secure_string recover_iv = "";
	#if 0
	char base64_decode_key[4*KEY_SIZE] = {0};
	char base64_decode_iv[4*BLOCK_SIZE] = {0};
	#else
	secure_string base64_decode_key = "";
	secure_string base64_decode_iv = "";
	#endif
	
	ifstream ifs(read_path);
	if (ifs.is_open())
	{
		for(int i=0;i<2;i++)
		{
			if(ifs.good())
			{
				getline(ifs,strline);
				cout << "length: " << strline.length() << ", content: " << strline << endl;
				if(i==0){
					if(is_decrypt){
						
						#if 1
						base64_decode_key.resize(2*strline.length());
						int base64_de = EVP_DecodeBlock((unsigned char *)base64_decode_key.c_str(), (const unsigned char *)strline.c_str(), strline.length());
						if(base64_de == -1) std::cout << "EVP_DecodeBlock failed" << std::endl;
						base64_decode_key.resize(base64_de/16*16);
						#else
						cout << "EVP_DecodeBlock length: " <<EVP_DecodeBlock((unsigned char*)base64_decode_key, (const unsigned char*)strline.c_str(), strline.length()) << endl;//base64 decode
						#endif
						if(aes_decrypt(g_local_key, g_local_iv, base64_decode_key, recover_key) != CRG_SUCCESS){
							ret = CRG_FAIL;
							break;
						}
					}else{
						recover_key = strline;
					}
					
				}
				else if(i==1){
					if(is_decrypt){
						#if 1
						base64_decode_iv.resize(2*strline.length());
						int base64_de = EVP_DecodeBlock((unsigned char *)base64_decode_iv.c_str(), (const unsigned char *)strline.c_str(), strline.length());
						if(base64_de == -1) std::cout << "EVP_DecodeBlock failed" << std::endl;
						base64_decode_iv.resize(base64_de/16*16);
						#else
						cout << "EVP_DecodeBlock length: " << EVP_DecodeBlock((unsigned char*)base64_decode_iv, (const unsigned char*)strline.c_str(), strline.length()) << endl;//base64 decode
						#endif
						if(aes_decrypt(g_local_key, g_local_iv, base64_decode_iv, recover_iv) != CRG_SUCCESS){
							ret = CRG_FAIL;
							break;
						}
					}else{
						recover_iv = strline;
					}
				}
				else break;
				ret = CRG_SUCCESS;
			}
		}
		if(ret == CRG_SUCCESS)
		{
			memcpy(g_key, recover_key.c_str(), recover_key.length());
			g_key[recover_key.length()] = '\0';
			memcpy(g_iv, recover_iv.c_str(), recover_iv.length());
			g_iv[recover_iv.length()] = '\0';
		}
	}
	if(ifs.is_open()) ifs.close();
	return ret;
}

static uint32_t save_aes_key_iv(string save_path, const uint8_t *key, const uint8_t *iv, bool encrypt = true)
{
	secure_string cipher_key = "";
	secure_string cipher_iv = "";
	#if 0
	char base64_encode_key[4*KEY_SIZE] = {0};
	char base64_encode_iv[4*BLOCK_SIZE] = {0};
	#endif
	string base64_encode_key = "";
	string base64_encode_iv = "";
	if(encrypt)
	{
		if(aes_encrypt(g_local_key, g_local_iv, (const char *)key, cipher_key) == CRG_SUCCESS
		&& aes_encrypt(g_local_key, g_local_iv, (const char *)iv, cipher_iv) == CRG_SUCCESS)
		{
			ofstream ofs(save_path);
			if (ofs.is_open())
			{
				base64_encode_key.resize(2*cipher_key.length());
				int base64_en = EVP_EncodeBlock((unsigned char*)base64_encode_key.c_str(), (const unsigned char*)cipher_key.c_str(), cipher_key.length());
				base64_encode_key.resize(base64_en);
				ofs << base64_encode_key;
				ofs << endl;
				
				base64_encode_iv.resize(2*cipher_iv.length());
				base64_en = EVP_EncodeBlock((unsigned char*)base64_encode_iv.c_str(), (const unsigned char*)cipher_iv.c_str(), cipher_iv.length());
				base64_encode_iv.resize(base64_en);
				ofs << base64_encode_iv;
				ofs.close();
				return CRG_SUCCESS;
			}
		}
	}
	else
	{
		ofstream ofs(save_path);
		if (ofs.is_open())
		{
			ofs << key;
			ofs << endl;
			ofs << iv;
			ofs.close();
			return CRG_SUCCESS;
		}
	}
	
	return CRG_FAIL;
}

uint32_t init_aes_encrypt_env()
{
	uint32_t ret = CRG_FAIL;
	// Load the necessary cipher
    EVP_add_cipher(EVP_aes_128_cbc());

	//if(read_aes_key_iv(g_key, g_iv) != CRG_SUCCESS)
	{
		printf("Read AES nessary date %s%s\n", PRINT_POINT_STR, PRINT_FAIL_STR);
	}
	//else
	{
		//if(post_safe_bind(g_key, g_iv, 1) == CRG_SUCCESS)
		{
			//ret = save_aes_key_iv(g_key, g_iv);
		}
	}
	return ret;
}

void destory_aes_encrypt_env()
{
	OPENSSL_cleanse(g_key, KEY_SIZE);
    OPENSSL_cleanse(g_iv, BLOCK_SIZE);
}

int main(int argc, char * argv[])
{
	EVP_add_cipher(EVP_aes_128_cbc());
	#if 0
	for(int i=0;i<KEY_SIZE;i++) g_local_key[i] = 'a'+i;
	for(int i=0;i<BLOCK_SIZE;i++) g_local_iv[i] = '0';
	#else
	memcpy(g_local_key, "abcdefghijklmnop", KEY_SIZE);
	memcpy(g_local_iv, "0000000000000000", BLOCK_SIZE);
	#endif
	for(int i=0;i<KEY_SIZE;i++) printf("%c",g_local_key[i]);
	cout << endl;
	for(int i=0;i<BLOCK_SIZE;i++) printf("%c",g_local_iv[i]);
	cout << endl;
	if(argc > 1)
	{
		if(atoi(argv[1]) == 1)
		{
			//明文转密文
			if(read_aes_key_iv("key_iv.txt", g_key, g_iv, false) == CRG_SUCCESS){
				save_aes_key_iv("aes.key.iv", g_key, g_iv);
			}else{
				printf("read_aes_key_iv key_iv.txt failed\n");
			}
		}
		else if(atoi(argv[1]) == 2)
		{
			//密文转明文
			if(read_aes_key_iv("aes.key.iv", g_key, g_iv) == CRG_SUCCESS){
				save_aes_key_iv("recover.txt", g_key, g_iv,false);
			}else{
				printf("read_aes_key_iv aes.key.iv failed\n");
			}
		}
	}
	else
	{
		if(read_aes_key_iv("key_iv.txt", g_key, g_iv, false) == CRG_SUCCESS){
			save_aes_key_iv("aes.key.iv", g_key, g_iv);
		}else{
			printf("read_aes_key_iv key_iv.txt failed\n");
		}
		printf("save aes.key.iv done!\n");
		if(read_aes_key_iv("aes.key.iv", g_key, g_iv) == CRG_SUCCESS){
			save_aes_key_iv("recover.txt", g_key, g_iv,false);
		}else{
			printf("read_aes_key_iv aes.key.iv failed\n");
		}
		printf("save recover.txt done!\n");
	}
	destory_aes_encrypt_env();
	return 0;
}