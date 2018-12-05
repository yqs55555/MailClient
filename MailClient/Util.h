#pragma once
#include <string>

namespace yqs
{
	/* ���ַ�����Ϊbase64���� */
	int EncodeBase64(const char* pSrc, char* pDst, int nSrcLen);

	std::string DecodeBase64(const char *str, int length);

	std::string GetFileName(const char* path);
}

