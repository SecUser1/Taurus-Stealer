#pragma once
class Antis
{
private:
	ULONG get_idt_base();
	ULONG get_gdt_base();
	void __cpuid(int CPUInfo[4], int InfoType);
public:
	bool IsCIS();
	bool IsVM();
};

