#include <stdio.h>
#include <seccomp.h>
#include <unistd.h>
void addRule(scmp_filter_ctx ctx, int sysnum) {
    int re = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, sysnum, 0);
    if(re){
        exit(1);
    }
}
void dropSyscalls(void) {
    scmp_filter_ctx  ctx;
    ctx = seccomp_init(SCMP_ACT_KILL);
    if(!ctx){
        exit(1);
    }
    seccomp_arch_add(ctx, SCMP_ARCH_X86_64);
    addRule(ctx, 0);
    addRule(ctx, 2);
    addRule(ctx, 60);
    seccomp_load(ctx);
    return;
}

void foo() {
    char a[0x20];
    read(STDIN_FILENO,a,0x1000);
}

int main(){
    long long int * addr;
    setbuf(stdin, 0);
    setbuf(stdout, 0);
    setbuf(stderr, 0);
    puts("I will give you a chance");
    scanf("%lld", &addr);
    printf("%lld\n", *addr);

    dropSyscalls();
    foo();
    asm("mov rax, 60");
    asm("mov rdi, 1");
    asm("syscall");
}
