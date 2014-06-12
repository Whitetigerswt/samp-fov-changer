// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <fstream>

#include "multiplayer_hooksystem.h"

void WINAPI Load();

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if ( CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)Load, NULL, 0, 0) == NULL ) {
			ExitProcess(GetLastError());
			return FALSE; 
		}
		break;
	}
	return TRUE;
}

float g_fFov = 70.0f;

DWORD dw_FovJmpBack1 = 0x0522F7E;
void _declspec(naked) MouseSensitivityHook() {
	
	_asm pushad
	_asm mov eax,[g_fFov]
	_asm mov [edi+0x0B4],eax
	_asm popad
	_asm jmp [dw_FovJmpBack1]

}

DWORD dw_FovJmpBack2 = 0x522F38;
float default_fov = 70.0f;
void _declspec(naked) MouseSensitivityHook_2() {
	
	_asm fadd dword ptr [default_fov]
	_asm jmp [dw_FovJmpBack2]

}

DWORD dw_FovJmpBack3 = 0x0523F67;
void _declspec(naked) MouseSensitivityHook_3() {
	
	_asm fld dword ptr [default_fov]
	_asm jmp [dw_FovJmpBack3]

}

void SetFov(float fov) {
	*(float*)0x0B6F250 = fov;
}


void WINAPI Load() {

	std::ifstream ifile("fov.cfg");	
	if (ifile) {
		if(ifile >> g_fFov) {

		}
		ifile.close();
	} else {
		std::ofstream ofile("fov.cfg");
		ofile << "70.0" << std::endl;
		ofile.close();

		g_fFov = 70.0f;
	}

	// Hook sniper aim
	DWORD oldProt = NULL;
	VirtualProtect((void*)0x0522F74, 10, PAGE_EXECUTE_READWRITE, &oldProt);
	HookInstall(0x0522F74, (DWORD)MouseSensitivityHook, 10);

	while(*(int*)0xB6F5F0 == 0) { 
		Sleep(5);
	}
	

	// NOP a mov instruction to address 0x0B6F250
	VirtualProtect((void*)0x051D5AB, 10, PAGE_EXECUTE_READWRITE, &oldProt);
	memcpy((void*)0x051D5AB, "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 10);

	// Patch so you won't zoom in when aiming
	VirtualProtect((void*)0x05216BE, 6, PAGE_EXECUTE_READWRITE, &oldProt);
	memcpy((void*)0x05216BE, "\x90\x90\x90\x90\x90\x90", 6);


	//--------------

	// MOUSE SENSITIVITY PATCH
	// changing fov messes with mouse sensitivity pretty badly, so these next few lines fix that.

	VirtualProtect((void*)0x0522F32, 6, PAGE_EXECUTE_READWRITE, &oldProt);
	HookInstall(0x0522F32, (DWORD)MouseSensitivityHook_2, 6);

	VirtualProtect((void*)0x0523F61, 6, PAGE_EXECUTE_READWRITE, &oldProt);
	HookInstall(0x0523F61, (DWORD)MouseSensitivityHook_3, 6);

	VirtualProtect((void*)0x0522F51, 6, PAGE_EXECUTE_READWRITE, &oldProt);
	memcpy((void*)0x0522F51, "\x90\x90\x90\x90\x90\x90", 6);

	VirtualProtect((void*)0x0522F6A, 6, PAGE_EXECUTE_READWRITE, &oldProt);
	memcpy((void*)0x0522F6A, "\x90\x90\x90\x90\x90\x90", 6);

	SetFov(g_fFov);


}
