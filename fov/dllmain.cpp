// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <fstream>
#include <string>

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

void SetFov(float fov) {
	*(float*)0x0858CE0 = fov;
	*(float*)0x0B6F250 = fov;
}

float g_fFov = 70.0f;
float g_fZoom = 3.0f;
float g_fAimingFov = 70.0f;

DWORD dw_FovJmpBack1 = 0x0522F7E;
void _declspec(naked) MouseSensitivityHook() {
	
	_asm push eax
	_asm mov eax,[g_fFov]
	_asm mov [edi+0x0B4],eax
	_asm pop eax
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

DWORD dw_FovJmpBack4 = 0x052C160;
void _declspec(naked) AimHook() {
	
	_asm fld [g_fFov]
	_asm jmp [dw_FovJmpBack4]


}

DWORD dw_FovJmpBack5 = 0x0521680;
void _declspec(naked) ZoomHook() {
	
	_asm fld [g_fZoom]
	_asm jmp [dw_FovJmpBack5]

}

DWORD dw_FovJmpBack6 = 0x05108B0;
void _declspec(naked) SniperZoomOutHook() {
	
	_asm push eax
	_asm mov eax,[g_fFov]
	_asm mov dword ptr ds:[edi],eax
	_asm pop eax
	_asm jmp [dw_FovJmpBack6]

}

DWORD dw_FovJmpBack7 = 0x050E495;
void _declspec(naked) AimHook2() {
	
	// re-do commands that were overwritten by the jmp
	_asm sub esp,0Ch
	_asm push ebx
	_asm push ebp
	// done!
	_asm pushad
	SetFov(g_fAimingFov);
	_asm popad
	_asm jmp dw_FovJmpBack7

}



void WINAPI Load() {

	std::ifstream ifile("fov.cfg");	
	if (ifile) {
		std::string type = "";
		float value = 0.0f;
		while(ifile >> type >> value) {
			if(type.compare("fov") == 0) {
				g_fFov = value;
			} else if(type.compare("zoom") == 0) {
				g_fZoom = value;
			} else if(type.compare("aiming_fov") == 0) {
				g_fAimingFov = value;
			}
		}
		ifile.close();
	} else {
		std::ofstream ofile("fov.cfg");
		ofile << "fov " << g_fFov << std::endl;
		ofile << "aiming_fov " << g_fAimingFov << std::endl;
		ofile << "zoom " << g_fZoom << " ; default 3.0, leave it as 0.0 unless you're playing lagcomp off, then put it as 3.0 or you will have trouble hitting people when you're using high FOV." << std::endl;
		ofile.close();
	}

	while(*(int*)0xB6F5F0 == 0) { 
		Sleep(5);
	}
	

	// Hook sniper aim
	DWORD oldProt = NULL;
	VirtualProtect((void*)0x0522F74, 10, PAGE_EXECUTE_READWRITE, &oldProt);
	HookInstall(0x0522F74, (DWORD)MouseSensitivityHook, 10);

	// NOP a mov instruction to address 0x0B6F250
	VirtualProtect((void*)0x051D5AB, 10, PAGE_EXECUTE_READWRITE, &oldProt);
	memcpy((void*)0x051D5AB, "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 10);

	VirtualProtect((void*)0x052C159, 6, PAGE_EXECUTE_READWRITE, &oldProt);
	HookInstall(0x052C159, (DWORD)AimHook, 6);

	VirtualProtect((void*)0x052167A, 7, PAGE_EXECUTE_READWRITE, &oldProt);
	HookInstall(0x052167A, (DWORD)ZoomHook, 7);

	VirtualProtect((void*)0x05108AA, 6, PAGE_EXECUTE_READWRITE, &oldProt);
	HookInstall(0x05108AA, (DWORD)SniperZoomOutHook, 6);

	// Make it so when you get in a car FOV is not reset
	VirtualProtect((void*)0x0522F47, 6, PAGE_EXECUTE_READWRITE, &oldProt);
	memcpy((void*)0x0522F47, "\x90\x90\x90\x90\x90\x90", 6);

	// NOP a mov instruction to 0x0B6F250 when entering a car
	VirtualProtect((void*)0x0524BDE, 10, PAGE_EXECUTE_READWRITE, &oldProt);
	memcpy((void*)0x0524BDE, "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 10);

	// RET on a useless call that causes issues with sniper, and when entering a vehicle with a changed FOV.
	VirtualProtect((void*)0x0509BC6, 6, PAGE_EXECUTE_READWRITE, &oldProt);
	memcpy((void*)0x0509BC6, "\xC3\x90\x90\x90\x90\x90", 6);

	// Hook so we can set a different FOV while aiming
	VirtualProtect((void*)0x050E490, 5, PAGE_EXECUTE_READWRITE, &oldProt);
	HookInstall(0x050E490, (DWORD)AimHook2, 5);

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
