#include <iostream>
#include <string>
#include <memory>
#include <limits>
#include <stdexcept>
#include <string.h>

#include <openssl/evp.h>
#include <openssl/rand.h>

static const unsigned int KEY_SIZE = 16;
static const unsigned int BLOCK_SIZE = 16;

template <typename T>
struct zallocator
{
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    pointer address (reference v) const {return &v;}
    const_pointer address (const_reference v) const {return &v;}

    pointer allocate (size_type n, const void* hint = 0) {
        if (n > std::numeric_limits<size_type>::max() / sizeof(T))
            throw std::bad_alloc();
        return static_cast<pointer> (::operator new (n * sizeof (value_type)));
    }

    void deallocate(pointer p, size_type n) {
        OPENSSL_cleanse(p, n*sizeof(T));
        ::operator delete(p); 
    }
    
    size_type max_size() const {
        return std::numeric_limits<size_type>::max() / sizeof (T);
    }
    
    template<typename U>
    struct rebind
    {
        typedef zallocator<U> other;
    };

    void construct (pointer ptr, const T& val) {
        new (static_cast<T*>(ptr) ) T (val);
    }

    void destroy(pointer ptr) {
        static_cast<T*>(ptr)->~T();
    }

#if __cpluplus >= 201103L
    template<typename U, typename... Args>
    void construct (U* ptr, Args&&  ... args) {
        ::new (static_cast<void*> (ptr) ) U (std::forward<Args> (args)...);
    }

    template<typename U>
    void destroy(U* ptr) {
        ptr->~U();
    }
#endif
};

typedef unsigned char byte;
typedef std::basic_string<char, std::char_traits<char>, zallocator<char> > secure_string;
using EVP_CIPHER_CTX_free_ptr = std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)>;

void gen_params(byte key[KEY_SIZE], byte iv[BLOCK_SIZE]);
void aes_encrypt(const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE], const secure_string& ptext, secure_string& ctext);
void aes_decrypt(const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE], const secure_string& ctext, secure_string& rtext);

// g++ -Wall -std=c++11 evp-encrypt.cpp -o evp-encrypt.exe -lcrypto
int main(int argc, char* argv[])
{
    // Load the necessary cipher
    EVP_add_cipher(EVP_aes_128_cbc());

    // plaintext, ciphertext, recovered text
	//secure_string ptext = "Now is the time for all good men to come to the aide of their country";
    secure_string ptext = "Now is the time for all good men to come to the aide of their ";
	std::cout << "ptext.length: " << ptext.length() << std::endl;
    secure_string ctext, rtext;

    byte key[KEY_SIZE], iv[BLOCK_SIZE];
    //gen_params(key, iv);
	for(uint32_t i=0;i<KEY_SIZE;i++) key[i] = 'a'+i;
	for(uint32_t i=0;i<BLOCK_SIZE;i++) iv[i] = '0';
	
	std::cout << "key: ";
	for(uint32_t i=0;i<KEY_SIZE;i++) printf("%c",key[i]);
	std::cout << std::endl;
	
	std::cout << "iv: ";
	for(uint32_t i=0;i<BLOCK_SIZE;i++) printf("%c",iv[i]);
	std::cout << std::endl;
	
    aes_encrypt(key, iv, ptext, ctext);
	
	//unsigned char out[1000] = {0};
	secure_string out;
	out.resize(2*ctext.size());
	std::cout << "ctext.length(): " << ctext.length() << std::endl;
	std::cout << "ctext.c_str(): " << ctext.c_str() << std::endl;
	
	int base64_en = EVP_EncodeBlock((unsigned char *)out.c_str(), (const unsigned char*)ctext.c_str(), ctext.length());
	if(base64_en == -1) std::cout << "EVP_EncodeBlock failed" << std::endl;
	out.resize(base64_en);
	std::cout << "base64 encode ret: " <<  base64_en << std::endl;
	std::cout << "base64 encode length: "<< out.size() << " base64 encode strlen: " << strlen(out.c_str())<< std::endl;
	std::cout << "base64 encode content: " << out << std::endl;
	
	#if 1
	//unsigned char recover_data[1000] = {0};
	secure_string recover_data;
	recover_data.resize(2*out.size());
	
	int base64_de = EVP_DecodeBlock((unsigned char *)recover_data.c_str(), (const unsigned char *)out.c_str(), out.size());
	if(base64_de == -1) std::cout << "EVP_DecodeBlock failed" << std::endl;
	recover_data.resize(base64_de/16*16);
	std::cout << "base64 decode ret: " <<  base64_de << std::endl;
	std::cout << "base64 decode length: "<< recover_data.size() << " base64 decode strlen: " << strlen(recover_data.c_str())<< std::endl;
	std::cout << "base64 decode content: " << recover_data << std::endl;

	aes_decrypt(key, iv, recover_data, rtext);
	#else
	unsigned char recover_data[1000] = {0};

	int base64_de = EVP_DecodeBlock(recover_data, (const unsigned char *)out.c_str(), out.size());
	if(base64_de == -1) std::cout << "EVP_DecodeBlock failed" << std::endl;

	std::cout << "base64 decode ret: " <<  base64_de << std::endl;
	std::cout << "base64 decode strlen: " << strlen((const char *)recover_data)<< std::endl;
	std::cout << "base64 decode content: " << recover_data << std::endl;

	aes_decrypt(key, iv, (char *)recover_data, rtext);
	#endif
	
    //aes_decrypt(key, iv, ctext, rtext);
    
    OPENSSL_cleanse(key, KEY_SIZE);
    OPENSSL_cleanse(iv, BLOCK_SIZE);

    std::cout << "Original message:\n" << ptext << std::endl;
    std::cout << "Recovered message:\n" << rtext << std::endl;

    return 0;
}

void gen_params(byte key[KEY_SIZE], byte iv[BLOCK_SIZE])
{
    int rc = RAND_bytes(key, KEY_SIZE);
    if (rc != 1)
      throw std::runtime_error("RAND_bytes key failed");

    rc = RAND_bytes(iv, BLOCK_SIZE);
    if (rc != 1)
      throw std::runtime_error("RAND_bytes for iv failed");
}

void aes_encrypt(const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE], const secure_string& ptext, secure_string& ctext)
{
    EVP_CIPHER_CTX_free_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
    int rc = EVP_EncryptInit_ex(ctx.get(), EVP_aes_128_cbc(), NULL, key, iv);
    if (rc != 1)
      throw std::runtime_error("EVP_EncryptInit_ex failed");

    // Recovered text expands upto BLOCK_SIZE
    ctext.resize(ptext.size()+BLOCK_SIZE);
    int out_len1 = (int)ctext.size();

    rc = EVP_EncryptUpdate(ctx.get(), (byte*)&ctext[0], &out_len1, (const byte*)&ptext[0], (int)ptext.size());
    if (rc != 1)
      throw std::runtime_error("EVP_EncryptUpdate failed");
  
    int out_len2 = (int)ctext.size() - out_len1;
    rc = EVP_EncryptFinal_ex(ctx.get(), (byte*)&ctext[0]+out_len1, &out_len2);
    if (rc != 1)
      throw std::runtime_error("EVP_EncryptFinal_ex failed");

    // Set cipher text size now that we know it
    ctext.resize(out_len1 + out_len2);
}

void aes_decrypt(const byte key[KEY_SIZE], const byte iv[BLOCK_SIZE], const secure_string& ctext, secure_string& rtext)
{
	std::cout << "aes_decrypt ctext.size(): " << ctext.size()<< std::endl;
    EVP_CIPHER_CTX_free_ptr ctx(EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free);
    int rc = EVP_DecryptInit_ex(ctx.get(), EVP_aes_128_cbc(), NULL, key, iv);
    if (rc != 1)
      throw std::runtime_error("EVP_DecryptInit_ex failed");

    // Recovered text contracts upto BLOCK_SIZE
    rtext.resize(ctext.size());
    int out_len1 = (int)rtext.size();

    rc = EVP_DecryptUpdate(ctx.get(), (byte*)&rtext[0], &out_len1, (const byte*)&ctext[0], (int)ctext.size());
    if (rc != 1)
      throw std::runtime_error("EVP_DecryptUpdate failed");
  
    int out_len2 = (int)rtext.size() - out_len1;
    rc = EVP_DecryptFinal_ex(ctx.get(), (byte*)&rtext[0]+out_len1, &out_len2);
	std::cout  << "rc: " << rc << std::endl;
    if (rc != 1)
      throw std::runtime_error("EVP_DecryptFinal_ex failed");

    // Set recovered text size now that we know it
    rtext.resize(out_len1 + out_len2);
}

