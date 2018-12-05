#pragma once
#include "Util.h"

namespace yqs
{
	/* ���ַ�����Ϊbase64���� */
	const char Base64Table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

	int EncodeBase64(const char* pSrc, char* pDst, int nSrcLen)
	{
		if (nullptr == pSrc || 0 == nSrcLen)
			return -1;

		unsigned char c1, c2, c3;
		int nDstLen = 0;
		int nDiv = nSrcLen / 3;
		int nMod = nSrcLen % 3;

		for (int i = 0; i < nDiv; i++)
		{
			c1 = *pSrc++;
			c2 = *pSrc++;
			c3 = *pSrc++;

			*pDst++ = Base64Table[c1 >> 2];
			*pDst++ = Base64Table[((c1 << 4) | c2 >> 4) & 0x3f];
			*pDst++ = Base64Table[((c2 << 2) | c3 >> 6) & 0x3f];
			*pDst++ = Base64Table[c3 & 0x3f];
			nDstLen += 4;
		}

		if (1 == nMod)
		{
			*pDst++ = Base64Table[(*pSrc) >> 2];
			*pDst++ = Base64Table[((*pSrc) << 4) & 0x3f];
			*pDst++ = '=';
			*pDst++ = '=';
			nDstLen += 4;
		}
		else if (2 == nMod)
		{
			c1 = *pSrc++;
			c2 = *pSrc++;

			*pDst++ = Base64Table[c1 >> 2];
			*pDst++ = Base64Table[((c1 << 4) | c2 >> 4) & 0x3f];
			*pDst++ = Base64Table[c2 >> 2];
			*pDst++ = '=';
			nDstLen += 4;
		}

		*pDst = '\0';
		return nDstLen;
	}

	std::string DecodeBase64(const char *str, int length) {
		//�����
		static const char DecodeTable[] =
		{
			-2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -2, -1, -2, -2,
			-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
			-1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
			52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -2, -2, -2,
			-2,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
			15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2,
			-2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
			41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2, -2, -2,
			-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
			-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
			-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
			-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
			-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
			-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
			-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
			-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2
		};
		int bin = 0, i = 0, pos = 0;
		std::string _decode_result;
		const char *current = str;
		char ch;
		while ((ch = *current++) != '\0' && length-- > 0)
		{
			if (ch == '=') { // ��ǰһ���ַ��ǡ�=����
							 /*
							 ��˵��һ������ڽ���ʱ��4���ַ�Ϊһ�����һ���ַ�ƥ�䡣
							 ����������
							 1�����ĳһ��ƥ��ĵڶ����ǡ�=���ҵ������ַ����ǡ�=����˵������������ַ������Ϸ���ֱ�ӷ��ؿ�
							 2�������ǰ��=�����ǵڶ����ַ����Һ�����ַ�ֻ�����հ׷�����˵�������������Ϸ������Լ�����
							 */
				if (*current != '=' && (i % 4) == 1) {
					return nullptr;
				}
				continue;
			}
			ch = DecodeTable[ch];
			//�������Ҫ�������������в��Ϸ����ַ�
			if (ch < 0) { /* a space or some other separator character, we simply skip over */
				continue;
			}
			switch (i % 4)
			{
			case 0:
				bin = ch << 2;
				break;
			case 1:
				bin |= ch >> 4;
				_decode_result += bin;
				bin = (ch & 0x0f) << 4;
				break;
			case 2:
				bin |= ch >> 2;
				_decode_result += bin;
				bin = (ch & 0x03) << 6;
				break;
			case 3:
				bin |= ch;
				_decode_result += bin;
				break;
			}
			i++;
		}
		return _decode_result;
	}
	std::string GetFileName(const char *path)
	{
		std::string str(path);
		int mark = str.find_last_of("//");
		str = str.substr(mark + 1, str.size() - mark - 1);
		return str;
	}
}

