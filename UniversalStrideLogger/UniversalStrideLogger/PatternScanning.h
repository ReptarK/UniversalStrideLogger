#pragma once
class PatternScanning
{
public:
	static bool bCompare(const BYTE* pData, const BYTE* bMask, const char* szMask);
	static DWORD_VERSION FindPattern(DWORD_VERSION dwAddress, DWORD_VERSION dwLen, BYTE *bMask, const char * szMask);
};

