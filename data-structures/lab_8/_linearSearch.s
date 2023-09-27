; Robert Owens (rao7utn)
; linearSearch.s
; 10/28/2020

	global _linearSearch

	section .text

; Parameter 1 (rdi) Pointer to the memory address of the int array
; Parameter 2 (esi) An integer that represents the size of the array
; Parameter 3 (edx) An integer (target) of what to find in the array
;
; Return val (rax) An unsigned long (index if found, -1 otherwise)
;
; Local var curVal (ecx) Will hold the integer value of array[i] in loop
; Local var curIndex (r8) Will hold the integer index i (is the for loop inteator)
_linearSearch:
	xor rax, rax
	xor r8, r8 ; i = 0
	call searchLoop

searchLoop:
	cmp r8d, esi
	je searchNotFound ; jump to notFound if i == size

	mov ecx, [rdi+4*r8]
	cmp ecx, edx
	je searchFound ; jump to found a[i] == target

	inc r8 ; i++ 
	jmp searchLoop ; back to top of loop

searchFound:
	mov rax, r8
	ret

searchNotFound:
	mov rax, -1
	ret