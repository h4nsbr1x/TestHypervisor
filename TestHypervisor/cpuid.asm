PUBLIC get_cpuid
PUBLIC test_protected_opcodes

.code
get_cpuid PROC
    mov eax, 40000001h ; HYPERV_CPUID_INTERFACE
    cpuid
    ret
get_cpuid ENDP

test_protected_opcodes PROC
    invept rax, oword ptr [rsi]
    invvpid rax, oword ptr [rcx]
    vmcall
    vmclear qword ptr [rcx]
    vmfunc
    vmlaunch
    vmptrld qword ptr [rcx]
    vmptrst qword ptr [rcx]
    vmread qword ptr [rcx], rax
    vmresume
    vmwrite rax, qword ptr [rcx]
    vmxoff
    vmxon qword ptr [rcx]
    ret
test_protected_opcodes ENDP

END