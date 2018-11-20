#ifndef HUAYUN_TRANSLATION_MD5_H
#define HUAYUN_TRANSLATION_MD5_H

#include <stdio.h>
#include <string.h>

#define S11				7
#define S12				12
#define S13				17
#define S14				22
#define S21				5
#define S22				9
#define S23				14
#define S24				20
#define S31				4
#define S32				11
#define S33				16
#define S34				23
#define S41				6
#define S42				10
#define S43				15
#define S44				21


/* F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;

// a structure for MD5 context
typedef struct md5_ctx
{
  UINT4 m_nState[4];									/* state (ABCD) */
  UINT4 m_nCount[2];									/* number of bits, modulo 2^64 (lsb first) */
  char m_szReserve[2];									//for reserve
  unsigned char m_szBuffer[64];                         /* input buffer */
} MD5_CTX;


class CTranslationMd5
{
	public:
		CTranslationMd5(const char *pszInput);
		~CTranslationMd5();
		int EnCrypt(char *pszOutput, int nBase = 32);
		
	protected:
		unsigned char m_szPadding[64];		//padding
		unsigned char *m_pszInput;		//pointer to input data
		MD5_CTX m_Context;		//MD5 context
		unsigned int m_nInputLen;	//input data length
		void UpdateData(unsigned char *pszOrigData, unsigned int nDataLen);
		void Translate(unsigned char *pszStart);
		void Encode(unsigned char *pszResult, UINT4 *pnGiven, unsigned int nLength);
		void Decode(UINT4 *pnResult, unsigned char *pszGiven, unsigned int nLength);
};

#endif
