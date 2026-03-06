#ifndef SYSCALLOF_H
# define SYSCALLOF_H

#include "asmdef.h"

typedef unsigned long syssz_t;

enum arch {
	X64_86,
	ARM,
	AARCH64,
	X86,
};

struct syscall_info {
	struct syscall *systabl;
	syssz_t sysmax;
	syssz_t syscnt;
	enum arch sysarch;
	int next;
};

int getsys_ent (struct syscall_info si,
                qword target);
void print_help ();
struct syscall_info get_sysinfo (char *arg);
int parse_sysnums (struct syscall_info si,
                   unsigned long *args,
                   int argc,
                   char **argv);
void print_syscalls (struct syscall_info si,
                     unsigned long sz,
		     unsigned long *args);

#endif /* SYSCALLOF_H */
