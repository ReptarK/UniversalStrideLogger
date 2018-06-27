#pragma once

class Log
{
public:
	static void Initialize(HMODULE);
	static void __cdecl add_log(const char *fmt, ...);

private:
	static char dlldir[320];
	static char strbuff[260];
	static std::ofstream ofile;

};

