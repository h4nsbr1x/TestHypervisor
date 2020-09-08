PUBLIC get_cpuid

.code
get_cpuid PROC
    mov eax, 40000001h ; HYPERV_CPUID_INTERFACE
    cpuid
    ret
get_cpuid ENDP

END