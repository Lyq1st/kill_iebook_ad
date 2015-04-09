#include "stdafx.h"

static const char  page_string [] = "<page lpage=\"0BCEB8E4";
static const char  page_string_end [] = "</page>";
static const char  node_viewframe [] = "viewFrame=\"";
static const char  iebook_caption [] = "<iebook_caption type=\"string\">";
static const char  iebook_caption_end [] = "</iebook_caption>";
static const char  iebook_copyright [] = "<copyright>";
static const char  iebook_copyright_end [] = "</copyright>";
static const char  iebook_link [] = "<iebook_link>";
static const char  iebook_link_end [] = "</iebook_link>";
static const unsigned int page_string_len = sizeof(page_string) - 1;
static const unsigned int page_string_end_len = sizeof(page_string_end) - 1;
static const unsigned int node_viewframeLen = sizeof(node_viewframe) - 1;
static const char clear_value [] = "true";
static const unsigned int clear_value_len = sizeof(clear_value) - 1;
static const unsigned int iebook_caption_len = sizeof(iebook_caption) - 1;
static const unsigned int iebook_copyright_len = sizeof(iebook_copyright) - 1;
static const unsigned int iebook_link_len = sizeof(iebook_link) - 1;
static const byte clear_string [] = "\0\0\0\0";

inline bool  MultiByteToUnicode(const char* strMultiByte, size_t strMultiByteLen, std::wstring& strUnicode, UINT uCodePage)
{
	int nUnicodeCount = MultiByteToWideChar(uCodePage, 0, strMultiByte, (int) strMultiByteLen, NULL, 0);
	if (nUnicodeCount <= 0)
		return false;
	if ((int) strMultiByteLen == -1)
		strUnicode.resize(nUnicodeCount - 1);
	else
		strUnicode.resize(nUnicodeCount);
	return MultiByteToWideChar(uCodePage, 0, strMultiByte, (int) strMultiByteLen, &strUnicode[0], nUnicodeCount) >= 0;
}
inline bool BDMUnicodeToMultiByte(const wchar_t* strUnicode, size_t strUnicodeLen, std::string& strUTF8, UINT uCodePage)
{
	int nUTF8Count = ::WideCharToMultiByte(uCodePage, 0, strUnicode, (int) strUnicodeLen, NULL, 0, NULL, NULL);
	if (nUTF8Count <= 0)
		return false;
	if ((int) strUnicodeLen == -1)
		strUTF8.resize(nUTF8Count - 1);
	else
		strUTF8.resize(nUTF8Count);
	return ::WideCharToMultiByte(uCodePage, 0, strUnicode, (int) strUnicodeLen, &strUTF8[0], nUTF8Count, NULL, NULL) >= 0;
}
inline std::string WtoA(const std::wstring &input)
{
	std::string strDest;
	BDMUnicodeToMultiByte(input.c_str(), input.length(), strDest, CP_UTF8);
	return strDest;
}
char *memstr(char *haystack, char *needle, int size)
{
	char *p;
	char needlesize = strlen(needle);

	for (p = haystack; p <= (haystack - needlesize + size); p++)
	{
		if (memcmp(p, needle, needlesize) == 0)
			return p; /* found */
	}
	return NULL;
}
bool crackFile(std::string st_str_path,std::string title)
{
	HANDLE hFile = NULL;
	hFile = ::CreateFileA(st_str_path.c_str(),
		GENERIC_WRITE | GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		DWORD dwError = GetLastError();
		return false;
	}
	ULARGE_INTEGER liFileSize;
	liFileSize.QuadPart = 0;
	liFileSize.LowPart = ::GetFileSize(hFile, &liFileSize.HighPart);
	byte*  byteOriginBuffer = new byte[liFileSize.LowPart], *byteBuffer = byteOriginBuffer;
	DWORD dwReadSize = 0;
	BOOL bRet = ::ReadFile(hFile, byteBuffer, liFileSize.LowPart, &dwReadSize, NULL);

	if (bRet)
	{
		{
			char *str_pos1 = memstr((char*) byteBuffer, (char*) iebook_caption, liFileSize.LowPart);
			if (NULL != str_pos1)
			{
				char *str_pos2 = memstr((char*) str_pos1, (char*) iebook_caption_end, liFileSize.LowPart - ((int) str_pos1 - (int) byteBuffer));
				if (NULL != str_pos2)
				{
					unsigned int caption_len = (unsigned int) str_pos2 - (unsigned int) str_pos1 - iebook_caption_len;
					printf("caption_len:%d\r\n", caption_len);
					if ("" == title)
						memset(str_pos1 + iebook_caption_len, 0x20, caption_len);
					else
					{
						memset(str_pos1 + iebook_caption_len, 0x20, caption_len);
						//std::string writeString = WtoA(title);
						std::wstring writeString;
						MultiByteToUnicode(title.c_str(), title.size(), writeString, CP_ACP);
						title = WtoA(writeString);
						if ( title.size() <= caption_len)
							memcpy(str_pos1 + iebook_caption_len, title.c_str(),title.size());
						else
							memcpy(str_pos1 + iebook_caption_len, title.c_str(), caption_len);

					}

				}
			}
		}
		{
			char *str_pos1 = memstr((char*) byteBuffer, (char*) iebook_copyright, liFileSize.LowPart);
			if (NULL != str_pos1)
			{
				char *str_pos2 = memstr((char*) str_pos1, (char*) iebook_copyright_end, liFileSize.LowPart - ((int) str_pos1 - (int) byteBuffer));
				if (NULL != str_pos2)
				{
					unsigned int copyright_len = (unsigned int) str_pos2 - (unsigned int) str_pos1 - iebook_copyright_len;
					printf("copyright_len:%d\r\n", copyright_len);
					memset(str_pos1 + iebook_copyright_len, 0x20, copyright_len);

				}
			}
		}
		{
			char *str_pos1 = memstr((char*) byteBuffer, (char*) iebook_link, liFileSize.LowPart);
			if (NULL != str_pos1)
			{
				char *str_pos2 = memstr((char*) str_pos1, (char*) iebook_link_end, liFileSize.LowPart - ((int) str_pos1 - (int) byteBuffer));
				if (NULL != str_pos2)
				{
					unsigned int link_len = (unsigned int) str_pos2 - (unsigned int) str_pos1 - iebook_link_len;
					printf("link_len:%d\r\n", link_len);
					memset(str_pos1 + iebook_link_len, 0x20, link_len);
				}
			}
		}
		{
			char *str_pos1 = memstr((char*) byteBuffer, (char*) page_string, liFileSize.LowPart);
			if (NULL != str_pos1)
			{
				char *str_pos2 = memstr((char*) str_pos1, (char*) page_string_end, liFileSize.LowPart - ((int) str_pos1 - (int) byteBuffer));
				if (NULL != str_pos2)
				{
					unsigned int page_len = (unsigned int) str_pos2 - (unsigned int) str_pos1 + page_string_end_len;
					printf("page_len:%d\r\n", page_len);
					memset(str_pos1, 0x20, page_len);
				}
			}
		}
		CloseHandle(hFile);

		st_str_path += "_crack.exe";
		hFile = ::CreateFileA(st_str_path.c_str(),
			GENERIC_WRITE | GENERIC_READ,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			DWORD dwError = GetLastError();
			goto END;
		}
		else
		{
			DWORD dwWirteSize = 0;
			BOOL bRet = ::WriteFile(hFile, byteBuffer, liFileSize.LowPart, &dwWirteSize, NULL);
			if (bRet && liFileSize.LowPart == dwWirteSize)
			{
				printf("crack succeed\r\n");
			}
			CloseHandle(hFile);
		}
END:
		if (NULL != byteOriginBuffer)
		{
			delete [] byteOriginBuffer;
			byteOriginBuffer = NULL;
		}
		return true;
	}
	return true;
}
int main(int argc, char * argv []){

	
	if (2 == argc)
	{
		crackFile(argv[1],"");
	}
	else if (3 == argc)
	{
		crackFile(argv[1], argv[2]);
	}
	return -1;
}
