	org 0x7c00
	cli

	mov ax, 0x201
	mov bx, 0x8000
	mov cx, 2
	mov dx, 0x80
	int 0x13

	mov bp, bx
	mov si, bx
	add si, [bp+28]

load	cmp dword[si], 1
	jne load1

	xor dx, dx
	mov ax, [si+4]
	mov di, 512
	div di
	mov cx, ax
	add cx, 2

	mov ax, [si+16]
	add ax, 511
	div di

	mov ah, 2
	mov bx, [si+8]
	mov dx, 0x80
	int 0x13

load1	add si, [bp+42]
	dec word[bp+44]
	jnz load

	in al, 0x92
	or al, 2
	out 0x92, al

	lgdt [gdt]

	mov eax, cr0
	or eax, 1
	mov cr0, eax

	jmp 8:init

	bits 32
init	mov ax, 16
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov esp, ebp
	jmp [ebp+24]

gdt	dw 23
	dd gdt+6
	dq 0
	dq 0xcf98000000ffff
	dq 0xcf92000000ffff

	times 510-($-$$) db 0
	dw 0xaa55
