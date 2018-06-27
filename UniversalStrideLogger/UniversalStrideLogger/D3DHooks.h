#pragma once
#include "Singleton.h"

namespace Functions
{
	// Defines
	using Reset = long(__stdcall *)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
	using Present = long(__stdcall *)(LPDIRECT3DDEVICE9, const RECT*, const RECT*, HWND, const RGNDATA*);
	using EndScene = long(__stdcall *)(IDirect3DDevice9*);
	using DrawIndexPrimitive = long(__stdcall *)(IDirect3DDevice9*, D3DPRIMITIVETYPE Type, INT Base, UINT Min, UINT Num, UINT Start, UINT Prim);

	// Functions
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters);
	long __stdcall hkPresent(LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
	long __stdcall hkEndScene(IDirect3DDevice9* device);
	long __stdcall hkDrawIndexPrimitive(IDirect3DDevice9*, D3DPRIMITIVETYPE Type, INT Base, UINT Min, UINT Num, UINT Start, UINT Prim);
}

namespace vTableIndex
{
	constexpr auto Reset = 16;
	constexpr auto Present = 17;
	constexpr auto EndScene = 42;
	constexpr auto DrawIndexPrimitive = 82;
}

class D3DHooks
	: public Singleton<D3DHooks>
{
	friend class Singleton<D3DHooks>;

	D3DHooks() {}
	~D3DHooks() {}

public:
	void Initialize(HMODULE = 0);
	void Shutdown();

	LPDIRECT3DDEVICE9 GetDevice() { return _device; }
	void SetDevice(LPDIRECT3DDEVICE9 device) { _device = device; }

	DWORD_VERSION* D3DVTable;

	Functions::Reset originalReset;
	Functions::Present originalPresent;
	Functions::EndScene originalEndScene;
	Functions::DrawIndexPrimitive originalDrawIndexPrimitive;

private:
	LPDIRECT3DDEVICE9 _device = 0;
};

