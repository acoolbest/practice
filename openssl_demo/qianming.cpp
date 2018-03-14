#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <string.h>
#include "qianming.h"

bool GeneratePKey(const char * publickeyFile,const char * privatekeyFile,char * password,int password_len)
{
	RSA* rsa=RSA_generate_key(1024,RSA_3,NULL,NULL);
	while(RSA_check_key(rsa)!=1)
		rsa=RSA_generate_key(1024,RSA_3,NULL,NULL);
	
    //Begin to generate a public key file
    BIO *bp = BIO_new(BIO_s_file());  
    if(NULL == bp)  
    {  
            printf("generate_key bio file new error!\n");  
    	    RSA_free(rsa);  
            return false;  
    }  
      
    if(BIO_write_filename(bp, (void *)publickeyFile) <= 0)  
    {  
            printf("BIO_write_filename error!\n");  
   	    BIO_free_all(bp);  
            RSA_free(rsa);  
            return false;
    }  
      
    if(PEM_write_bio_RSAPublicKey(bp, rsa) != 1)  
    {  
        printf("PEM_write_bio_RSAPublicKey error!\n");  
        BIO_free_all(bp);  
        RSA_free(rsa);  
        return false;  
    }  

    //A public key file generated successfully
	//Release resources
    printf("Create public key ok!\n");  
    BIO_free_all(bp);  
      
    //Begin to generate the private key file
    bp = BIO_new_file(privatekeyFile, "w+");  
    if(NULL == bp)  
    {  
            printf("generate_key bio file new error2!\n");  
            return false;  
    }  
      
    if(PEM_write_bio_RSAPrivateKey(bp, rsa,  
            EVP_des_ede3_ofb(), (unsigned char *)password,   
            password_len, NULL, NULL) != 1)  
    {  
            printf("PEM_write_bio_RSAPublicKey error!\n");  
    	    BIO_free_all(bp);  
    	    RSA_free(rsa);  
            return false;  
    }
	
    //A public key file generated successfully
    //Release resources
    printf("Create private key ok!\n");  
    BIO_free_all(bp);  
    RSA_free(rsa);  
 
	return true;	
}

//读取密钥
// 打开公钥文件，返回EVP_PKEY结构的指针  
EVP_PKEY* open_public_key(const char *keyfile)  
{  
        EVP_PKEY* key = NULL;  
        RSA *rsa = NULL;  
      
        OpenSSL_add_all_algorithms();  
        BIO *bp = NULL;  
        bp = BIO_new_file(keyfile, "rb");   
        if(NULL == bp)  
        {  
            printf("open_public_key bio file new error!\n");  
            return NULL;  
        }  
      
        rsa = PEM_read_bio_RSAPublicKey(bp, NULL, NULL, NULL);  
        if(rsa == NULL)  
        {  
            printf("open_public_key failed to PEM_read_bio_RSAPublicKey!\n");  
            BIO_free(bp);  
            return NULL;  
        }  
      
        printf("open_public_key success to PEM_read_bio_RSAPublicKey!\n");  
        key = EVP_PKEY_new();  
        if(NULL == key)  
        {  
            printf("open_public_key EVP_PKEY_new failed\n");  
            BIO_free(bp);  
            RSA_free(rsa);  
            return NULL;  
        }  
        BIO_free(bp);  
        RSA_free(rsa);  
        EVP_PKEY_assign_RSA(key, rsa);  
        return key;  
}  

// 打开私钥文件，返回EVP_PKEY结构的指针     
EVP_PKEY* open_private_key(const char *keyfile, const unsigned char *passwd)  
{  
        EVP_PKEY* key = NULL;  
        RSA *rsa = NULL;  
        OpenSSL_add_all_algorithms();  
        BIO *bp = NULL;  
        bp = BIO_new_file(keyfile, "rb");   
        if(NULL == bp)  
        {  
            printf("open_private_key bio file new error!\n");  
            return NULL;  
        }  
      
        rsa = PEM_read_bio_RSAPrivateKey(bp, &rsa, NULL, (void *)passwd);  
        if(rsa == NULL)  
        {  
            printf("open_private_key failed to PEM_read_bio_RSAPrivateKey!\n");  
            BIO_free(bp);  
            return NULL;  
        }  
      
        printf("open_private_key success to PEM_read_bio_RSAPrivateKey!\n");  
        key = EVP_PKEY_new();  
        if(NULL == key)  
        {  
            printf("open_private_key EVP_PKEY_new failed\n");  
            BIO_free(bp);  
            RSA_free(rsa);  
            return NULL;  
        }  
        BIO_free(bp);  
        RSA_free(rsa);  
        EVP_PKEY_assign_RSA(key, rsa);  
        return key;  
}  
// 使用密钥加密，这种封装格式只适用公钥加密，私钥解密，这里key必须是公钥  
int rsa_key_encrypt(EVP_PKEY *key, const unsigned char *orig_data, size_t orig_data_len,   
                        unsigned char *enc_data, size_t *enc_data_len)  
{  
        EVP_PKEY_CTX *ctx = NULL;  
        OpenSSL_add_all_ciphers();  
      
        ctx = EVP_PKEY_CTX_new(key, NULL);  
        if(NULL == ctx)  
        {  
            printf("ras_pubkey_encryptfailed to open ctx.\n");  
            EVP_PKEY_free(key);  
            return -1;  
        }  
      
        if(EVP_PKEY_encrypt_init(ctx) <= 0)  
        {  
            printf("ras_pubkey_encryptfailed to EVP_PKEY_encrypt_init.\n");  
            EVP_PKEY_CTX_free(ctx);  
            EVP_PKEY_free(key);  
            return -1;  
        }  
      
        if(EVP_PKEY_encrypt(ctx,  
            enc_data,  
            enc_data_len,  
            orig_data,  
            orig_data_len) <= 0)  
        {  
            printf("ras_pubkey_encryptfailed to EVP_PKEY_encrypt.\n");  
            EVP_PKEY_CTX_free(ctx);  
            EVP_PKEY_free(key);  
      
            return -1;  
        }  
      
        EVP_PKEY_CTX_free(ctx);  
        EVP_PKEY_free(key);  
      
        return 0;  
}  

// 使用密钥解密，这种封装格式只适用公钥加密，私钥解密，这里key必须是私钥  
int rsa_key_decrypt(EVP_PKEY *key, const unsigned char *enc_data, size_t enc_data_len,   
                        unsigned char *orig_data, size_t *orig_data_len)  
{  
        EVP_PKEY_CTX *ctx = NULL;  
        OpenSSL_add_all_ciphers();  
      
        ctx = EVP_PKEY_CTX_new(key, NULL);  
        if(NULL == ctx)  
        {  
            printf("ras_prikey_decryptfailed to open ctx.\n");  
            EVP_PKEY_free(key);  
            return -1;  
        }  
      
        if(EVP_PKEY_decrypt_init(ctx) <= 0)  
        {  
            printf("ras_prikey_decryptfailed to EVP_PKEY_decrypt_init.\n");  
            EVP_PKEY_CTX_free(ctx);  
            EVP_PKEY_free(key);  
            return -1;  
        }  
      
        if(EVP_PKEY_decrypt(ctx,  
            orig_data,  
            orig_data_len,  
            enc_data,  
            enc_data_len) <= 0)  
        {  
            printf("ras_prikey_decryptfailed to EVP_PKEY_decrypt.\n");  
            EVP_PKEY_CTX_free(ctx);  
            EVP_PKEY_free(key);  
            return -1;  
        }  
      
        EVP_PKEY_CTX_free(ctx);  
        EVP_PKEY_free(key);  
        return 0;  
}  


//私钥签名，公钥验签
int private_sign(const unsigned char *in_str,unsigned int in_str_len,unsigned char *outret,unsigned int *outlen,const char*keyfile,const char * passwd)
{
	OpenSSL_add_all_algorithms();

	BIO *bp = BIO_new_file(keyfile, "rb");
	if(NULL == bp)
	{
		//printf("open_private_key bio file new error!\n");  
		return CANNOT_OPEN_KEYFILE;  
	}  

	RSA *rsa= NULL;
	rsa= PEM_read_bio_RSAPrivateKey(bp, &rsa, NULL, (void *)passwd);  
	if(rsa == NULL)  
	{  
		//printf("open_private_key failed to PEM_read_bio_RSAPrivateKey!\n");  
		BIO_free(bp);
		return READ_KEY_FROM_KEYFILE_FAIL;  
	}  
	
	//printf("open_private_key success to PEM_read_bio_RSAPrivateKey!\n");  
	int result = RSA_sign(NID_sha1,in_str,in_str_len,outret,outlen,rsa);

	BIO_free(bp);  
	RSA_free(rsa);  
	//EVP_PKEY_free(key);  
	return result != SIGN_SUCCESS ? SIGN_FAIL : result;
}
int public_verify(const unsigned char *in_str, unsigned int in_len,unsigned char *outret, unsigned int outlen,const char*keyfile)
{
	RSA *rsa = NULL;  

	OpenSSL_add_all_algorithms();  
	BIO *bp = BIO_new_file(keyfile, "rb");
	if(NULL == bp)  
	{  
		//printf("open_public_key bio file new error!\n");  
		return CANNOT_OPEN_KEYFILE;  
	}  

	rsa = PEM_read_bio_RSAPublicKey(bp, NULL, NULL, NULL);  
	if(rsa == NULL)  
	{  
		//printf("open_public_key failed to PEM_read_bio_RSAPublicKey!\n");  
		BIO_free(bp);
		return READ_KEY_FROM_KEYFILE_FAIL;
	}

	int result = RSA_verify(NID_sha1,in_str,in_len,outret,outlen,rsa);
	
	BIO_free(bp);
	RSA_free(rsa);
    return result != VERIFY_SUCCESS ? VERIFY_FAIL : result;
}
int charge_verify(const unsigned char *text_str, unsigned int text_len,unsigned char *bin_str, unsigned int bin_len)
{
	return public_verify(text_str,text_len,bin_str,bin_len,(const char*)PUBLIC_KEY_FILE);
}

//base64 encode,decode
const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char * base64_encode( const unsigned char * bindata, char * base64, int binlength )
{
    int i, j;
    unsigned char current;

    for ( i = 0, j = 0 ; i < binlength ; i += 3 )
    {
        current = (bindata[i] >> 2) ;
        current &= (unsigned char)0x3F;
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)(bindata[i] << 4 ) ) & ( (unsigned char)0x30 ) ;
        if ( i + 1 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+1] >> 4) ) & ( (unsigned char) 0x0F );
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)(bindata[i+1] << 2) ) & ( (unsigned char)0x3C ) ;
        if ( i + 2 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+2] >> 6) ) & ( (unsigned char) 0x03 );
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)bindata[i+2] ) & ( (unsigned char)0x3F ) ;
        base64[j++] = base64char[(int)current];
    }
    base64[j] = '\0';
    return base64;
}

unsigned int base64_decode( const char * base64, unsigned char * bindata )
{
    unsigned int i, j;
    unsigned char k;
    unsigned char temp[4];
    for ( i = 0, j = 0; base64[i] != '\0' ; i += 4 )
    {
        memset( temp, 0xFF, sizeof(temp) );
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i] )
                temp[0]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+1] )
                temp[1]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+2] )
                temp[2]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+3] )
                temp[3]= k;
        }

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2))&0xFC)) |
                ((unsigned char)((unsigned char)(temp[1]>>4)&0x03));
        if ( base64[i+2] == '=' )
            break;

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4))&0xF0)) |
                ((unsigned char)((unsigned char)(temp[2]>>2)&0x0F));
        if ( base64[i+3] == '=' )
            break;

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6))&0xF0)) |
                ((unsigned char)(temp[3]&0x3F));
    }
    return j;
}


