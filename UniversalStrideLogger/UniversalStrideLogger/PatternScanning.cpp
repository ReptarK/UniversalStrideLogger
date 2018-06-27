#pragma once
#include "stdafx.h"
#include "PatternScanning.h"

bool PatternScanning::bCompare(const BYTE * pData, const BYTE * bMask, const char * szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
		if (*szMask == 'x' && *pData != *bMask)
			return false;

	return (*szMask) == NULL;
}

DWORD_VERSION PatternScanning::FindPattern(DWORD_VERSION dwAddress, DWORD_VERSION dwLen, BYTE * bMask, const char * szMask)
{
	for (DWORD_VERSION i = 0; i < dwLen; i++)
		if (bCompare((BYTE*)(dwAddress + i), bMask, szMask))
			return (DWORD_VERSION)(dwAddress + i);

	return 0;
}

