#include <iostream>
#include <Windows.h>
#include <capstone/capstone.h>

extern "C" int get_cpuid();
extern "C" int test_protected_opcodes();

int GetInstruction(csh* pHandle, cs_insn** pInsn, const uint8_t* address) {
    // shamelessly taken from the capstone tutorial
    size_t count;

    if (cs_open(CS_ARCH_X86, CS_MODE_64, pHandle) != CS_ERR_OK) {
        return -1;
    }
    count = cs_disasm(*pHandle, (const uint8_t*)address, 0x10, (uint64_t)address, 1, pInsn);
    if (count > 0) {
        return 0;
    }
    return -1;
}

const char access_violation[] = "EXCEPTION_ACCESS_VIOLATION";
const char illegal_instruction[] = "EXCEPTION_ILLEGAL_INSTRUCTION";

const char* GetFaultName(int faultCode) {
    switch (faultCode) {
        case EXCEPTION_ILLEGAL_INSTRUCTION:
            return illegal_instruction;
        case EXCEPTION_ACCESS_VIOLATION:
            return access_violation;
        default:
            return NULL;
    }
}

int HandleException(int exceptionCode, LPEXCEPTION_POINTERS pExceptionPointers) {
    csh handle;
    cs_insn* insn;

    if (GetInstruction(&handle, &insn, (const uint8_t*)pExceptionPointers->ContextRecord->Rip) == 0) {
        const char* faultName = GetFaultName(exceptionCode);
        if (faultName) {
            std::cout << insn[0].mnemonic << " threw exception " << faultName << std::endl;
        }
        else {
            std::cout << insn[0].mnemonic << " threw exception " << std::hex << exceptionCode << std::endl;
        }
        pExceptionPointers->ContextRecord->Rip += insn[0].size;
        cs_close(&handle);
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    cs_close(&handle);
    return EXCEPTION_CONTINUE_SEARCH;
}

int main()
{
    int cpuid_string[2];

    cpuid_string[0] = get_cpuid();
    cpuid_string[1] = 0;
    if (cpuid_string[0] < 0x100) {
        std::cout << "Hypervisor code: " << std::hex << cpuid_string[0] << std::endl;
    }
    else {
        std::cout << "Hypervisor code: " << ((char*)cpuid_string) << std::endl;
    }

    // test vmcall

    __try {
        test_protected_opcodes();
    }
    __except (HandleException(GetExceptionCode(), GetExceptionInformation())) {
        ;
    }

    std::cout << "Exiting gracefully" << std::endl;

    return 0;
}

