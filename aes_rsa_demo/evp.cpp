/**
https://www.cnblogs.com/gordon0918/p/5332803.html
第二种方法，使用EVP框架，示例如下
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/aes.h>

int main(void)
{
    char userkey[EVP_MAX_KEY_LENGTH];
    char iv[EVP_MAX_IV_LENGTH];
    unsigned char *date = (unsigned char *)malloc(AES_BLOCK_SIZE*3);
    unsigned char *encrypt = (unsigned char *)malloc(AES_BLOCK_SIZE*6);
    unsigned char *plain = (unsigned char *)malloc(AES_BLOCK_SIZE*6);
    EVP_CIPHER_CTX ctx;
    int ret;
    int tlen = 0;
    int mlen = 0;
    int flen = 0;

    memset((void*)userkey, 'k', EVP_MAX_KEY_LENGTH);
    memset((void*)iv, 'i', EVP_MAX_IV_LENGTH);
    memset((void*)date, 'p', AES_BLOCK_SIZE*3);
    memset((void*)encrypt, 0, AES_BLOCK_SIZE*6);
    memset((void*)plain, 0, AES_BLOCK_SIZE*6);

    /*初始化ctx*/
    EVP_CIPHER_CTX_init(&ctx);

    /*指定加密算法及key和iv(此处IV没有用)*/
    ret = EVP_EncryptInit_ex(&ctx, EVP_aes_128_ecb(), NULL, (const unsigned char*)userkey, (const unsigned char*)iv);
    if(ret != 1) {
        printf("EVP_EncryptInit_ex failed\n");
        exit(-1);
    }
    
    /*禁用padding功能*/
    EVP_CIPHER_CTX_set_padding(&ctx, 0);
    /*进行加密操作*/
    ret = EVP_EncryptUpdate(&ctx, encrypt, &mlen, date, AES_BLOCK_SIZE*3);
    if(ret != 1) {
        printf("EVP_EncryptUpdate failed\n");
        exit(-1);
    }
    /*结束加密操作*/
    ret = EVP_EncryptFinal_ex(&ctx, encrypt+mlen, &flen);
    if(ret != 1) {
        printf("EVP_EncryptFinal_ex failed\n");
        exit(-1);
    }

    tlen = mlen + flen;

    tlen = 0;
    mlen = 0;
    flen = 0;

    EVP_CIPHER_CTX_cleanup(&ctx);
    EVP_CIPHER_CTX_init(&ctx);
     
    ret = EVP_DecryptInit_ex(&ctx, EVP_aes_128_ecb(), NULL, (const unsigned char*)userkey, (const unsigned char*)iv);
    if(ret != 1) {
        printf("EVP_DecryptInit_ex failed\n");
        exit(-1);
    }
    
    EVP_CIPHER_CTX_set_padding(&ctx, 0);
    ret = EVP_DecryptUpdate(&ctx, plain, &mlen, encrypt, AES_BLOCK_SIZE*3);
    if(ret != 1) {
        printf("EVP_DecryptUpdate failed\n");
        exit(-1);
    }

    ret = EVP_DecryptFinal_ex(&ctx, plain+mlen, &flen);
    if(ret != 1) {
        printf("EVP_DecryptFinal_ex failed\n");
        exit(-1);
    }
    /*对比解密后与原数据是否一致*/
    if(!memcmp(plain, date, AES_BLOCK_SIZE*3)) {
        printf("test success\n");    
    } else {
        printf("test failed\n");    
    }

    printf("encrypt: ");
    int i;
    for(i = 0; i < AES_BLOCK_SIZE*3+4; i ++){
        printf("%.2x ", encrypt[i]);    
        if((i+1)%32 == 0){
            printf("\n");
        }
    }
    printf("\n");

    return 0;
}

/**
编译执行结果如下:

xlzh@cmos:~/cmos/openssl-code/aes$ g++ evp.cpp -o evp.out -lssl -lcrypto
xlzh@cmos:~/cmos/openssl-code/aes$ ./evp.out 
test success
encrypt: 08 a9 74 4d b0 66 57 1b 57 fe 60 3d 91 e4 ed 53 08 a9 74 4d b0 66 57 1b 57 fe 60 3d 91 e4 ed 53 
08 a9 74 4d b0 66 57 1b 57 fe 60 3d 91 e4 ed 53 00 00 00 00 
xlzh@cmos:~/cmos/openssl-code/aes$
*/

/**
EVP框架是对openssl提供的所有算法进行了封装，在使用工程中只需要修改少量的代码就可以选择不同的加密算法，在工作中通常采用这种方式。

在上述两个示例中，直接使用API提供的接口，没有使用padding，在EVP中同样需要声明不可以使用padding方式，否则即使要加密的数据长度是AES_BLOCK_SIZE的整数倍，EVP默认也会对原始数据进行追加，导致结果不同，所以在试验中通过EVP_CIPHER_CTX_set_padding(&ctx, 0)函数关闭的EVP的padding功能，同样在解密的时候也需要进行关闭。
*/