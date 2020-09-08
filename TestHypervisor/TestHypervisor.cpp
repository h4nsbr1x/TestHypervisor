#include <iostream>

extern "C" int get_cpuid();

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
}

