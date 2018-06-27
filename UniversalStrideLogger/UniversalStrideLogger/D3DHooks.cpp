#include "stdafx.h"
#include "D3DHooks.h"
#include "EventManager.h"
#include "PatternScanning.h"

namespace Functions
{
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		EventHandler<EventIndex::OnReset, EventDefines::OnReset, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*>::GetInstance()->Trigger(device, pPresentationParameters);
		auto hr = D3DHooks::Get().originalReset(device, pPresentationParameters);
		/**/
		return hr;
	}

	long __stdcall hkPresent(LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect,
		HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
	{
		EventHandler<EventIndex::OnDrawingPresent, EventDefines::OnDrawingPresent, LPDIRECT3DDEVICE9, const RECT*, const RECT*, HWND, const RGNDATA*>
			::GetInstance()->Trigger(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
		return D3DHooks::Get().originalPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	}

	long __stdcall hkEndScene(IDirect3DDevice9* device)
	{
		EventHandler<EventIndex::OnDrawingEndScene, EventDefines::OnDrawingEndScene, IDirect3DDevice9*>::GetInstance()->Trigger(device);
		return D3DHooks::Get().originalEndScene(device);
	}
	long __stdcall hkDrawIndexPrimitive(IDirect3DDevice9 * device, D3DPRIMITIVETYPE Type, INT Base, UINT Min, UINT Num, UINT Start, UINT Prim)
	{
		EventHandler<EventIndex::OnDrawIndexPrimitive, EventDefines::OnDrawIndexPrimitive, IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT>
			::GetInstance()->Trigger(device, Type, Base, Min, Num, Start, Prim);
		return D3DHooks::Get().originalDrawIndexPrimitive(device, Type, Base, Min, Num, Start, Prim);
	}
}

LRESULT CALLBACK D3D9MsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return DefWindowProc(hwnd, uMsg, wParam, lParam); }

void D3DHooks::Initialize(HMODULE Handle)
{
	HMODULE dDll_ = NULL;
	while (!dDll_)
	{
		dDll_ = GetModuleHandleA("d3d9.dll");
		Sleep(100);
	}
	CloseHandle(dDll_);

	IDirect3D9* d3d = NULL;
	IDirect3DDevice9* d3ddev = NULL;

	std::cout << "[GetModuleHandleA(NULL)] = " << GetModuleHandleA(NULL) << std::endl;
	WNDCLASSEXA wc = { sizeof(WNDCLASSEX),CS_CLASSDC,D3D9MsgProc,0L,0L,GetModuleHandleA(NULL),NULL,NULL,NULL,NULL,"DX",NULL };
	RegisterClassExA(&wc);
	std::cout << "[WNDCLASSEXA] = " << &wc << std::endl;
	HWND tmpWnd = CreateWindowA("DX", NULL, WS_OVERLAPPEDWINDOW, 100, 100, 300, 300, GetDesktopWindow(), NULL, wc.hInstance, NULL);
	if (tmpWnd == NULL)
	{
		std::cout << "[DirectX] Failed to create temp window" << std::endl;
		return;
	}

	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d == NULL)
	{
		DestroyWindow(tmpWnd);
		std::cout << "[DirectX] Failed to create temp Direct3D interface" << std::endl;
		return;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = tmpWnd;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	HRESULT result = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, tmpWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
	if (result != D3D_OK)
	{
		d3d->Release();
		DestroyWindow(tmpWnd);
		std::cout << "[DirectX] Failed to create temp Direct3D device" << std::endl;
		return;
	}
	std::cout << "[d3ddev] = " << d3ddev << std::endl;
	#if defined _M_X64
		DWORD64* dVtable_ = (DWORD64*)d3ddev;
		dVtable_ = (DWORD64*)dVtable_[0];
	#elif defined _M_IX86
		DWORD* dVtable_ = (DWORD*)d3ddev;
		dVtable_ = (DWORD*)dVtable_[0]; // == *d3ddev
	#endif

		D3DVTable = dVtable_;
		DestroyWindow(tmpWnd);

	std::cout << "Table at : ";
	printf("0x%08x\n", D3DVTable);

	MH_CreateHook((PBYTE)D3DVTable[vTableIndex::Reset], &Functions::hkReset, reinterpret_cast<LPVOID*>(&originalReset));
	MH_EnableHook((PBYTE)D3DVTable[vTableIndex::Reset]);

	MH_CreateHook((PBYTE)D3DVTable[vTableIndex::EndScene], &Functions::hkEndScene, reinterpret_cast<LPVOID*>(&originalEndScene));
	MH_EnableHook((PBYTE)D3DVTable[vTableIndex::EndScene]);

	MH_CreateHook((PBYTE)D3DVTable[vTableIndex::DrawIndexPrimitive], &Functions::hkDrawIndexPrimitive, reinterpret_cast<LPVOID*>(&originalDrawIndexPrimitive));
	MH_EnableHook((PBYTE)D3DVTable[vTableIndex::DrawIndexPrimitive]);

	std::cout << "originalReset at : ";
	printf("0x%08x\n", originalReset);
	std::cout << "originalEndScene at : ";
	printf("0x%08x\n", originalEndScene);
	std::cout << "originalDrawIndexPrimitive at : ";
	printf("0x%08x\n", originalDrawIndexPrimitive);
}

void D3DHooks::Shutdown()
{
	if (originalReset)
		MH_DisableHook((PBYTE)D3DVTable[vTableIndex::Reset]);

	if (originalEndScene)
		MH_DisableHook((PBYTE)D3DVTable[vTableIndex::EndScene]);

	if (originalDrawIndexPrimitive)
		MH_DisableHook((PBYTE)D3DVTable[vTableIndex::DrawIndexPrimitive]);
}
