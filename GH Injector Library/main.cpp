#include "pch.h"

#include "Tools.h"

BOOL WINAPI DllMain(HINSTANCE hDll, DWORD dwReason, void * pReserved)
{
	UNREFERENCED_PARAMETER(pReserved);
	
	if (dwReason == DLL_PROCESS_ATTACH)
	{

#ifdef DEBUG_INFO
		AllocConsole();

		FILE * pFile = nullptr;
		freopen_s(&pFile, "CONOUT$", "w", stdout);
#endif

		LOG("GH Injector V%ls attached at %p\n", GH_INJ_VERSIONW, hDll);

		DisableThreadLibraryCalls(hDll);

		g_hInjMod = hDll;

		char	szRootPathA[MAX_PATH]{ 0 };
		wchar_t szRootPathW[MAX_PATH]{ 0 };

		if (!GetOwnModulePathA(szRootPathA, sizeof(szRootPathA) / sizeof(szRootPathA[0])))
		{
			LOG("Couldn't resolve own module path (ansi)\n");

			return FALSE;
		}
		
		if (!GetOwnModulePathW(szRootPathW, sizeof(szRootPathW) / sizeof(szRootPathW[0])))
		{
			LOG("Couldn't resolve own module path (unicode)\n");

			return FALSE;
		}

		size_t buffer_size = MAX_PATH;
		char * szWindowsDir = new char[buffer_size];

		if (_dupenv_s(&szWindowsDir, &buffer_size, "WINDIR"))
		{
			LOG("Couldn't resolve %%WINDIR%%\n");

			delete[] szWindowsDir;

			return FALSE;
		}

		g_RootPathA = szRootPathA;
		g_RootPathW = szRootPathW;

		LOG("Rootpath is %s\n", szRootPathA);

#ifdef _WIN64
		std::string szNtDllWOW64 = szWindowsDir;
		szNtDllWOW64 += "\\SysWOW64\\ntdll.dll";

		sym_ntdll_wow64_ret	= std::async(std::launch::async, &SYMBOL_PARSER::Initialize, &sym_ntdll_wow64, szNtDllWOW64, g_RootPathA, nullptr, false);
#endif

		std::string szNtDllNative = szWindowsDir;
		szNtDllNative += "\\System32\\ntdll.dll";

		sym_ntdll_native_ret = std::async(std::launch::async, &SYMBOL_PARSER::Initialize, &sym_ntdll_native, szNtDllNative, g_RootPathA, nullptr, false);

		delete[] szWindowsDir;
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		LOG("Process detaching\n");
	}
	
	return TRUE;
}