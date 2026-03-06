#include "asmdef.h"

const struct syscall ssyscalls_x86_64[] = {
	SYSCALLS_X86_64
};

const struct syscall ssyscalls_arm[] = {
	SYSCALLS_ARM
};

const struct syscall ssyscalls_aarch64[] = {
	SYSCALLS_AARCH64
};

const struct syscall ssyscalls_x86[] = {
	SYSCALLS_X86
};

#undef SYSCALLS_X86_64
#undef SYSCALLS_ARM
#undef SYSCALLS_AARCH64
#undef SYSCALLS_X86
