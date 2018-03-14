//make:    gcc -o qianming qianming.c qianming_test.c -Wall -g -lssl -lcrypto

#ifndef _QIANMING_H_
#define _QIANMING_H_
#include <openssl/evp.h>
#include <stdbool.h>
#define CANNOT_OPEN_KEYFILE -2
#define READ_KEY_FROM_KEYFILE_FAIL -3
#define EVP_PKEY_NEW_FAIL          -4
#define SIGN_SUCCESS               1
#define SIGN_FAIL				   -1

#define VERIFY_SUCCESS               1
#define VERIFY_FAIL				   -1

#define PUBLIC_KEY_FILE "./publickey"
#define PRIVATE_KEY_FILE "./privatekey"
#define RSA_PRIKEY_PSW  "123456"

bool GeneratePKey(const char * publickeyFile,const char * privatekeyFile,char * password,int password_len);
//读取密钥
// 打开公钥文件，返回EVP_PKEY结构的指针  
EVP_PKEY* open_public_key(const char *keyfile) ;

// 打开私钥文件，返回EVP_PKEY结构的指针     
EVP_PKEY* open_private_key(const char *keyfile, const unsigned char *passwd) ;
// 使用密钥加密，这种封装格式只适用公钥加密，私钥解密，这里key必须是公钥  
int rsa_key_encrypt(EVP_PKEY *key, const unsigned char *orig_data, size_t orig_data_len,   
                        unsigned char *enc_data, size_t *enc_data_len) ;
// 使用密钥解密，这种封装格式只适用公钥加密，私钥解密，这里key必须是私钥  
int rsa_key_decrypt(EVP_PKEY *key, const unsigned char *enc_data, size_t enc_data_len,   
                        unsigned char *orig_data, size_t *orig_data_len);

//私钥签名，公钥验签
int private_sign(const unsigned char *in_str,unsigned int in_str_len,unsigned char *outret,unsigned int *outlen,const char*keyfile,const char * passwd);
int public_verify(const unsigned char *in_str, unsigned int in_len,unsigned char *outret, unsigned int outlen,const char*keyfile);
//for charge 
int charge_verify(const unsigned char *text_str, unsigned int text_len,unsigned char *bin_str, unsigned int bin_len);

//base64
char * base64_encode( const unsigned char * bindata, char * base64, int binlength );
unsigned int base64_decode( const char * base64, unsigned char * bindata );


#endif //_QIANMING_H_
