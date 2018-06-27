#include "stdafx.h"
#include "Chams.h"
#include "EventManager.h"
#include "D3DHooks.h"
#include "Log.h"

#define PLAYER1(NumVertices, PrimitiveCount, Stride)\
(NumVertices == 2924 && PrimitiveCount == 3540 && Stride == 32) ||\
(NumVertices == 4084 && PrimitiveCount == 4697 && Stride == 32) || (NumVertices == 4422 && PrimitiveCount == 6319 && Stride == 32) ||\
(NumVertices == 2052 && PrimitiveCount == 2966 && Stride == 32) || (NumVertices == 64 && PrimitiveCount == 92 && Stride == 32)

#define PLAYER2(NumVertices, PrimitiveCount, Stride)\
(NumVertices == 136 && PrimitiveCount == 204 && Stride == 32) || (NumVertices == 1967 && PrimitiveCount == 2720 && Stride == 32) ||\
(NumVertices == 5299 && PrimitiveCount == 7167 && Stride == 32) || (NumVertices == 2482 && PrimitiveCount == 3414 && Stride == 32) ||\
(NumVertices == 2118 && PrimitiveCount == 3354 && Stride == 32)

typedef struct _STRIDELOG {
	INT Base; UINT Min;
	UINT Num; UINT Start;
	UINT Prim;
}STRIDELOG, *PSTRIDELOG;

STRIDELOG StrideLog;
std::vector<STRIDELOG> STRIDE;
std::vector<DWORD_VERSION> BASETEX;
D3DPRESENT_PARAMETERS PrP = { NULL };
LPDIRECT3DBASETEXTURE9 BTEX = NULL;
LPDIRECT3DTEXTURE9 Green = NULL;
LPDIRECT3DTEXTURE9 Yellow = NULL;
LPDIRECT3DTEXTURE9 Red = NULL;
LPDIRECT3DTEXTURE9 pTx = NULL;
DWORD_VERSION dCrDev = NULL;
LPDIRECT3D9 pDx = NULL;
LPD3DXFONT pFont = NULL;
D3DVIEWPORT9 Vpt;
D3DLOCKED_RECT d3dlr;
PDWORD_VERSION VTab = NULL;
DWORD_VERSION Old = NULL;
std::ofstream ofile;
char strbuff[260];
UINT iStride = 0;
UINT iBaseTex = 0;
bool Found = false;
bool Startlog = false;

void DrawCsgoChams(LPDIRECT3DDEVICE9 device, D3DPRIMITIVETYPE Type,
	INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT StartIndex, UINT PrimCount, UINT Stride)
{
	if (PLAYER1(NumVertices, PrimCount, Stride)) {
		device->SetTexture(0, Green);
		device->SetRenderState(D3DRS_ZENABLE, FALSE);
		D3DHooks::Get().originalDrawIndexPrimitive(device, Type, BaseVertexIndex, MinVertexIndex, NumVertices, StartIndex, PrimCount);
		device->SetRenderState(D3DRS_ZENABLE, TRUE);
	}

	if (PLAYER2(NumVertices, PrimCount, Stride)) {
		device->SetTexture(0, Green);
		device->SetRenderState(D3DRS_ZENABLE, FALSE);
		D3DHooks::Get().originalDrawIndexPrimitive(device, Type, BaseVertexIndex, MinVertexIndex, NumVertices, StartIndex, PrimCount);
		device->SetRenderState(D3DRS_ZENABLE, TRUE);
	}
}

void OnDrawIndexPrimitive(LPDIRECT3DDEVICE9 device, D3DPRIMITIVETYPE Type,
	INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT StartIndex, UINT PrimCount)
{
	LPDIRECT3DVERTEXBUFFER9 Stream_Data;
	UINT Offset = 0;
	UINT Stride = 0;

	if (device->GetStreamSource
	(0, &Stream_Data, &Offset, &Stride) == S_OK)
		Stream_Data->Release();

	if (Stride == iStride)
	{
		device->GetTexture(0, &BTEX);
		Found = false;

		for (UINT i = 0; i < BASETEX.size(); i++)
			if (BASETEX[i] == (DWORD_VERSION)BTEX)
			{
				Found = true;
				break;
			}

		if (Found == false)
			BASETEX.push_back
			((DWORD_VERSION)BTEX);

		if (BASETEX[iBaseTex] == (DWORD_VERSION)BTEX && Green)
		{
			device->SetTexture(0, Green);
			device->SetRenderState(D3DRS_ZENABLE, FALSE);
			D3DHooks::Get().originalDrawIndexPrimitive(device, Type, BaseVertexIndex, MinVertexIndex, NumVertices, StartIndex, PrimCount);
			device->SetRenderState(D3DRS_ZENABLE, TRUE);

			if (Startlog == true)
			{
				Found = false;
				for (UINT i = 0; i < STRIDE.size(); i++)
					if (STRIDE[i].Base == BaseVertexIndex &&
						STRIDE[i].Min == MinVertexIndex &&
						STRIDE[i].Num == NumVertices &&
						STRIDE[i].Start == StartIndex &&
						STRIDE[i].Prim == PrimCount)
					{
						Found = true;
						break;
					}

				if (Found == false)
				{
					printf("(NumVertices == %i && PrimCount == %i && Stride == %i\n)",
						NumVertices, PrimCount, Stride);
					StrideLog.Base = BaseVertexIndex;
					StrideLog.Min = MinVertexIndex;
					StrideLog.Num = NumVertices;
					StrideLog.Start = StartIndex;
					StrideLog.Prim = PrimCount;

					Log::add_log("(NumVertices == %i && PrimitiveCount == %i && Stride == %i)",
						NumVertices, PrimCount, Stride);

					STRIDE.push_back(StrideLog);
				}
			}
		}
	}

	DrawCsgoChams(device, Type, BaseVertexIndex, MinVertexIndex, NumVertices, StartIndex, PrimCount, Stride);
}

void OnEndScene(LPDIRECT3DDEVICE9 device)
{
	device->GetViewport(&Vpt);

	RECT FRect = { 0,0,
		Vpt.Width / 5,Vpt.Height };

	if (Green == NULL)
		if (device->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT, &Green, NULL) == S_OK)
			if (device->CreateTexture(8, 8, 1, 0, D3DFMT_A8R8G8B8,
				D3DPOOL_SYSTEMMEM, &pTx, NULL) == S_OK)
				if (pTx->LockRect(0, &d3dlr, 0, D3DLOCK_DONOTWAIT |
					D3DLOCK_NOSYSLOCK) == S_OK)
				{
					for (UINT xy = 0; xy < 8 * 8; xy++)
						((PDWORD)d3dlr.pBits)[xy] = 0xFFBF57C3;

					pTx->UnlockRect(0);
					device->UpdateTexture(pTx, Green);
					pTx->Release();
				}

	if (pFont == NULL)
		D3DXCreateFontA(device, 16, 0, 700, 0, 0, 1, 0,
			0, DEFAULT_PITCH | FF_DONTCARE, "Calibri", &pFont);

	sprintf(strbuff, "Num of Textures: %i\nStride: %i\nBase Tex Num: %i\n\nSTRIDE LOGGER V1\n\n" \
		"Log Enable: %i\n\nVK_RIGHT: Stride++\nVK_LEFT: Stride--\nVK_UP: BaseTexNum++" \
		"\nVK_DOWN: BaseTexNum--\nVK_NUMPAD0: Log On/Off", \
		BASETEX.size(), iStride, iBaseTex + 1, Startlog);

	if (pFont)
		pFont->DrawTextA(0, strbuff, -1, &FRect,
			DT_CENTER | DT_NOCLIP, 0xFF00FF00);

	if (GetAsyncKeyState(VK_RIGHT) & 1)
	{
		iStride++; BASETEX.clear(); iBaseTex = 0;
	}

	if (GetAsyncKeyState(VK_LEFT) & 1)
		if (iStride > 0)
		{
			iStride--; BASETEX.clear(); iBaseTex = 0;
		};

	if (GetAsyncKeyState(VK_UP) & 1)
		if (iBaseTex < BASETEX.size() - 1)iBaseTex++;

	if (GetAsyncKeyState(VK_DOWN) & 1)
		if (iBaseTex > 0)
			iBaseTex--;

	if (GetAsyncKeyState(VK_NUMPAD0) & 1)
	{
		Startlog = !Startlog; STRIDE.clear();
	}
}

void OnReset(LPDIRECT3DDEVICE9 pDev, D3DPRESENT_PARAMETERS* PresP)
{
	if (pFont) { pFont->Release(); pFont = NULL; }
	if (Green) { Green->Release(); Green = NULL; }
}

void Chams::Initialize()
{
	EventHandler<EventIndex::OnReset, EventDefines::OnReset, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*>::GetInstance()->Add(OnReset);
	EventHandler<EventIndex::OnDrawingEndScene, EventDefines::OnDrawingEndScene, IDirect3DDevice9*>::GetInstance()->Add(OnEndScene);
	EventHandler<EventIndex::OnDrawIndexPrimitive, EventDefines::OnDrawIndexPrimitive, IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT>::GetInstance()->Add(OnDrawIndexPrimitive);
}
