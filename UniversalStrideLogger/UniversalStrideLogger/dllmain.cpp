// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "EventManager.h"
#include "D3DHooks.h"
#include "Chams.h"
#include "Log.h"

#define _DEBUG

template <EventIndex uniqueEventNumber, typename T, typename ... TArgs>
EventHandler<uniqueEventNumber, T, TArgs...>* EventHandler<uniqueEventNumber, T, TArgs...>::instance = nullptr;

HMODULE Handle;

///Game Address
#define ADDRESS_CGAME 0x1446CC4F0
#define ADDRESS_GRAPHICS 0x1446CD9C8

///Offsets
#define OFFSET_PLAYERBASE 0x300
#define OFFSET_TYPE 0x3F8
#define OFFSET_POSITION 0x420
#define OFFSET_VELOCITY 0x430
#define OFFSET_YAW 0x530 
#define OFFSET_PITCH 0x534
#define OFFSET_ACTOR 0x5C0
#define OFFSET_NAME 0x7C0
#define OFFSET_NEXTEOBJECT 0x748
#define OFFSET_STANCE 0x954

struct Vector3
{
	float x;
	float y;
	float z;
};

struct CActor;
struct CPlayerBase;

class CPlayer
{
public:
	char pad_0x0000[0x300]; //0x0000
	CPlayerBase* m_pPlayerBase; //0x0300 
	char pad_0x0308[0xF0]; //0x0308
	__int64 m_iType; //0x03F8 
	char pad_0x0400[0x20]; //0x0400
	Vector3 m_vPosition; //0x0420 
	char pad_0x042C[0x4]; //0x042C
	Vector3 m_vVelocity; //0x0430 
	char pad_0x043C[0xF4]; //0x043C
	float m_fYaw; //0x0530 
	float m_fPitch; //0x0534 
	char pad_0x0538[0x88]; //0x0538
	CActor* m_pActor; //0x05C0 
	char pad_0x05C8[0x1F8]; //0x05C8
	char* m_cName; //0x07C0 
	char pad_0x07C8[0x18C]; //0x07C8
	__int32 m_iStance; //0x0954 
	char pad_0x0958[0x3AE8]; //0x0958

}; //Size=0x4440
void Test()
{
	DWORD64 pGame = *reinterpret_cast<DWORD64*>(ADDRESS_CGAME);
}

void InitializeConsole()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
}

bool InitializeD3D()
{
	if (MH_Initialize() != MH_OK) {
		std::cout << "Failed to Initialize D3D ..." << std::endl;
		return false;
	}
	EventHandler<EventIndex::OnDrawingEndScene, EventDefines::OnDrawingEndScene, IDirect3DDevice9*>::GetInstance()->Add(nullptr);
	EventHandler<EventIndex::OnDrawingEndScene, EventDefines::OnDrawingEndScene, IDirect3DDevice9*>::GetInstance()->Remove(nullptr);
	EventHandler<EventIndex::OnReset, EventDefines::OnReset, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*>::GetInstance()->Add(nullptr);
	EventHandler<EventIndex::OnReset, EventDefines::OnReset, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*>::GetInstance()->Remove(nullptr);
	EventHandler<EventIndex::OnDrawingPresent, EventDefines::OnDrawingPresent, LPDIRECT3DDEVICE9, const RECT*, const RECT*, HWND, const RGNDATA*>::GetInstance()->Add(nullptr);
	EventHandler<EventIndex::OnDrawingPresent, EventDefines::OnDrawingPresent, LPDIRECT3DDEVICE9, const RECT*, const RECT*, HWND, const RGNDATA*>::GetInstance()->Remove(nullptr);
	EventHandler<EventIndex::OnDrawIndexPrimitive, EventDefines::OnDrawIndexPrimitive, IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT>::GetInstance()->Add(nullptr);
	EventHandler<EventIndex::OnDrawIndexPrimitive, EventDefines::OnDrawIndexPrimitive, IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT>::GetInstance()->Remove(nullptr);
	D3DHooks::Get().Initialize(Handle);
	return true;
}

DWORD WINAPI MainThread(LPVOID base)
{
	InitializeConsole();
	MessageBoxA(NULL, "Injected!", "Info", 0);
	//while (!GetAsyncKeyState(VK_F2)) { Sleep(150); std::cout << "F2 "; }
	Beep(523, 250); Beep(523, 250);

	InitializeD3D();
	Chams::Get().Initialize();

	while (!GetAsyncKeyState(VK_END)) {
		Sleep(100);
	}

	FreeConsole();
	Beep(523, 250);
	FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
	return false;
}

BOOL WINAPI OnDllDetach()
{
	MH_Uninitialize();
	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	switch (reason) {
	case DLL_PROCESS_ATTACH:
		Handle = hModule;
		//Log::Initialize(hModule);
		DisableThreadLibraryCalls(hModule);
		CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
		return TRUE;
	case DLL_PROCESS_DETACH:
		if (lpReserved == nullptr)
			return OnDllDetach();
		return TRUE;
	default:
		return TRUE;
	}
	return TRUE;
}

