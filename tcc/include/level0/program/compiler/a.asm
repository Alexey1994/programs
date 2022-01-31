use32

;000009BE  0FB6450C          movzx eax,byte [ebp+0xc]
;000009C2  0FB75508          movzx edx,word [ebp+0x8]
;000009C6  EE                out dx,al
mov edx, 0x3D4
mov al, 0x0A
out dx, al

mov edx, 0x3D5
mov al, 0x20
out dx, al

jnz 0
cmp eax, 1234

mov eax, edx

je a
xor eax, eax
jmp b
a: mov eax, 1
b:

cmp eax, ecx
add [esp + 4], eax
mov [ebp + ebx + 1234], ecx
mov eax, [ebp + ebx + 1234]

mov [ecx], eax
mov ebx, [esp]
mov eax, [ebp + ebx + 0]
mov ecx, [ebp + ebx + 0]
mov [ebp + ebx + 0], eax
mov [ebp + ebx + 0], dword 12345
add [esp], dword 4


mov eax, ebp
add eax, [esp]
mov eax, [eax]
sub [esp], dword 4

add ebp, 0xffff
sub ebp, 0xffff
mov dword [ebp + 0], 1

add esp, 1
sub esp, 1
mov dword [esp], 0
add dword [esp], 1
sub dword [esp], 1
mov eax, [ebp + 1]
mov eax, [esp + 1]

push ebp
pop ebp
pop eax
ret
push 1234
mov [esp], ebp
add ebp, eax
mov [esp], dword 0
call 12345
call eax
add ebp, [esp + 8]
mov eax, [eax]

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

jmp 25

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

not eax
neg eax

or eax, ecx
and eax, ecx
xor eax, ecx

shl eax, cl  ;eax = eax << cl
shr eax, cl  ;eax = eax >> cl

add eax, ecx ;eax = eax + ecx
sub eax, ecx ;eax = eax - ecx
mul ecx      ;eax = eax * ecx
div ecx      ;eax = eax / ecx, edx = eax % ecx