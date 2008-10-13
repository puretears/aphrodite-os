
[section .text]
global memcpy
global memset
global strcpy

; memcpy(void *dest, void *src, int size);
memcpy:
	push ebp
	mov ebp, esp
	push edi
	push esi
	push ecx

	mov edi, [ebp + 8]
	mov esi, [ebp + 12]
	mov ecx, [ebp + 16]

	rep movsb
	
	mov eax, [ebp + 8]

	pop ecx
	pop esi
	pop edi
	mov esp, ebp
	pop ebp
	ret

; memset(void *dest, char filler, int size);
memset:
	push ebp
	mov ebp, esp
	push eax
	push ecx
	push edi
	xor eax, eax
	mov edi, [ebp + 8] ; dest
	mov al, [ebp +12] ; filler
	mov ecx, [ebp + 16] ; size
	rep stosb
	pop edi
	pop ecx
	pop eax
	mov esp, ebp
	pop ebp
	ret

; strcpy(char *dest, char *src)
strcpy:
	push ebp
	mov ebp, esp

	mov esi, [ebp + 12] ; src
	mov edi, [ebp + 8] ; dest

	lodsb
	stosb
	cmp al, 0
	jz COPY_OVER
COPY_OVER:
	mov eax, [ebp + 8] ; return the dest address 
	mov esp, ebp
	pop ebp
	ret
