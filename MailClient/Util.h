#pragma once
#include <string>

namespace yqs
{
	/* ½«×Ö·û´®±äÎªbase64±àÂë */
	int EncodeBase64(const char* pSrc, char* pDst, int nSrcLen);

	std::string DecodeBase64(const char *str, int length);

	std::string GetFileName(const char* path);
}

