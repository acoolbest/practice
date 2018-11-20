#include "TranslationMd5.h"


/****************************************************
* Date:	2007/02/13
* This member function is used to construct a CTranslationMD5 type of  object
* Parameters:
*		pszInput [in]:		Pointer to input data
* No Return Values
*****************************************************/
CTranslationMd5::CTranslationMd5(const char *pszInput)
{
	memset(&m_Context, 0, sizeof(m_Context));
	m_Context.m_nState[0] = 0x67452301;
	m_Context.m_nState[1] = 0xefcdab89;
	m_Context.m_nState[2] = 0x98badcfe;
	m_Context.m_nState[3] = 0x10325476;

	memset(m_szPadding, 0, sizeof(m_szPadding));
	m_szPadding[0] = 0x80;
	
	m_pszInput = (unsigned char*)pszInput;
	m_nInputLen = (unsigned int)strlen(pszInput);
}


/*********************************************
* Date:	2007/02/13
* This member function is used to deconstruct a CTranslateMD5 type of object
* No Parameters and Return Values
**********************************************/
CTranslationMd5::~CTranslationMd5()
{
	m_pszInput = NULL;
	memset(&m_Context, 0 ,sizeof(m_Context));
}


/********************************************************
* Date:	2007/02/13
* This member function is used to encode data
* Parameters:
*		pszResult [out]:		Pointer to encrypted data,
*		pnGiven [in]:				Pointer to original data
*		nLength [in]:				Original data length
* No Return Values
*********************************************************/
void CTranslationMd5::Encode(unsigned char *pszResult, UINT4 *pnGiven, unsigned int nLength)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < nLength; i++, j += 4)
	{
		pszResult[j] = (unsigned char)(pnGiven[i] & 0xff);
		pszResult[j+1] = (unsigned char)((pnGiven[i] >> 8) & 0xff);
		pszResult[j+2] = (unsigned char)((pnGiven[i] >> 16) & 0xff);
		pszResult[j+3] = (unsigned char)((pnGiven[i] >> 24) & 0xff);
	}
}


/********************************************************
* Date:	2007/02/13
* This member function is used to decode data
* Parameters:
*		pnResult [out]:				Pointer to decrypted data
*		pszGiven [in]:				Pointer to original data
*		nLength [in]:					Original data length
* No Return Values
*********************************************************/
void CTranslationMd5::Decode(UINT4 *pnResult, unsigned char *pszGiven, unsigned int nLength)
{
  unsigned int i, j;

  for (i = 0, j = 0; j < nLength; i++, j += 4)
  {
		pnResult[i] = ((UINT4)pszGiven[j]) | (((UINT4)pszGiven[j+1]) << 8) |
		(((UINT4)pszGiven[j+2]) << 16) | (((UINT4)pszGiven[j+3]) << 24);
  }
}


/*************************************************
* Date:	2007/02/13
* This member function is used to translate(both encoding and decoding) data
* Parameters:
*		pszStart[in]:		Pointer to original data
* No Return Values
**************************************************/
void CTranslationMd5::Translate(unsigned char *pszStart)
{
	UINT4 a = m_Context.m_nState[0], b = m_Context.m_nState[1], c = m_Context.m_nState[2], d = m_Context.m_nState[3], x[16];

	Decode (x, pszStart, 64);

	/* Round 1 */
	FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
	FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
	FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
	FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
	FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
	FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
	FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
	FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
	FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
	FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
	FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	/* Round 2 */
	GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
	GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
	GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
	GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
	GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
	GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
	GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
	GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
	GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
	GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
	GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
	GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
	HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
	HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
	HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
	HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
	HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
	HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
	HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
	HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
	HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

	/* Round 4 */
	II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
	II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
	II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
	II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
	II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
	II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
	II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
	II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
	II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
	II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

	m_Context.m_nState[0] += a;
	m_Context.m_nState[1] += b;
	m_Context.m_nState[2] += c;
	m_Context.m_nState[3] += d;

	/* Zeroize sensitive information.
	*/
	memset ((POINTER)x, 0, sizeof (x));
}


/**********************************************************
* Date:	2007/02/13
* This member function is used to update translated result
* Parameters:
*		pszOrigData [in]:		Pointer to original data
*		nDataLen [in]:			original data length
* No Return Values
***********************************************************/
void CTranslationMd5::UpdateData(unsigned char *pszOrigData, unsigned int nDataLen)
{
	unsigned int nStart, nIndex, nPartLen;
	nIndex = (unsigned int)((m_Context.m_nCount[0] >> 3) & 0x3F);
	
	m_Context.m_nCount[0] += (UINT4)nDataLen << 3;
	if(m_Context.m_nCount[0] < (UINT4)nDataLen << 3)
	{
		m_Context.m_nCount[1]++;
	}

	m_Context.m_nCount[1] += (UINT4)nDataLen >> 29;
	nPartLen = 64 -nIndex;

	//Translate as many times as possible
	if(nDataLen >= nPartLen)
	{
		memcpy((POINTER)(m_Context.m_szBuffer+nIndex), (POINTER)pszOrigData, nPartLen);
		Translate(m_Context.m_szBuffer);
			
		for(nStart = nPartLen; nStart+63 < nDataLen; nStart += 64)
		{
			Translate(pszOrigData+nStart);
		}
		nIndex = 0;
	}
	else
	{
		nStart = 0;
	}

	//Buffering remaining input
	memcpy(m_Context.m_szBuffer+nIndex, pszOrigData+nStart, nDataLen-nStart);
}


/******************************************************
* Date:	2007/02/13
* This member function is used to encrypt data with MD5 algorithm
* Parameters:
*		pszOutput [out]:		Pointer to the final enrypted data
*		nBase [in]:					Base for encrypting. It is optinal and 32 by default
*
* Return Values:
*		Always 1
******************************************************/
int CTranslationMd5::EnCrypt(char *pszOutput, int nBase)
{
	UpdateData(m_pszInput, m_nInputLen);

	unsigned char szBits[8];
	unsigned int nIndex, nPadLen;
	unsigned char *pszOrigOutput = new unsigned char[16];
	memset(pszOrigOutput, 0, 16);

	//Save number of bits 
	Encode (szBits, m_Context.m_nCount, 8);

	//Pad out to 56 mod 64.
	nIndex = (unsigned int)((m_Context.m_nCount[0] >> 3) & 0x3f);
	nPadLen = (nIndex < 56) ? (56 - nIndex) : (120 - nIndex);
	UpdateData(m_szPadding, nPadLen);
	UpdateData( szBits, 8);
	Encode(pszOrigOutput, m_Context.m_nState, 16);

	if(nBase == 16)
	{
		memcpy(pszOrigOutput, pszOrigOutput+4, 8);
		memset(pszOrigOutput+8, 0, 8);
	}

	nBase /= 2;
	for(int i = 0; i < nBase; ++i)
	{
		sprintf(pszOutput+2*i, "%02x", pszOrigOutput[i]);
	}
	
	delete []pszOrigOutput;
	pszOrigOutput = NULL;

	return 1;
}