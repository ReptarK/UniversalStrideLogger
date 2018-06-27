#include "stdafx.h"
#include "Log.h"

char Log::dlldir[320];
char Log::strbuff[260];
std::ofstream Log::ofile;

void Log::Initialize(HMODULE hModule)
{
	GetModuleFileNameA(hModule, dlldir, 320);
	dlldir[strlen(dlldir) - 3] = 0;
	strcat(dlldir, "txt");

	ofile.open(dlldir, std::ios::app);
	add_log("\nCSGO Log");
}

void Log::add_log(const char * fmt, ...)
{
	if (ofile)
	{
		if (!fmt) { return; }

		va_list va_alist;
		char logbuf[256] = { 0 };

		va_start(va_alist, fmt);
		_vsnprintf(logbuf + strlen(logbuf),
			sizeof(logbuf) - strlen(logbuf), fmt, va_alist);
		va_end(va_alist);

		ofile << logbuf << std::endl;
	}
}
