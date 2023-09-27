; Robert Owens (rao7utn)
; 11/04/2020
; binarySearch.s

	global binarySearch
	section .text

; Parameter 1 (rdi) memeory address of the start of the array
; Parameter 2 (esi) lowest index in partition of search
; Parameter 3 (edx) highest index in partition of search
; Parameter 4 (ecx) target (value)

; local var (r8d) stores the pivot (curIndex)
; local var (r9d) int value of half the a.length()/2

; Return (rax) the index of the array with the target

binarySearch:
	; Load in the middle array memeory address (a[a.lenght()/2])
	xor rax, rax
	xor r8, r8
	xor r9, r9
	xor r10, r10
	xor r11, r11
	mov r9d, edx
	shr r9d, 1

	mov r8d, r9d 

	; enter loop
	jmp iterate

iterate:
	cmp edx, esi
	jl notFound

	mov r11, r8
	shl r11, 2
	add r11, rdi
	mov r10, [r11]

	cmp ecx, r10d
	je found
	jl lowerHalf ; target < a[i] look at lower half
	jg upperHalf ; target > a[i] look upper half

; add 1 to pivot then make that the lower bound and recurse
upperHalf:
	inc r8d
	mov esi, r8d
	shr r9d, 1
	jmp iterate

; subtract 1 from privot then
; make the pivot the upperbound
lowerHalf:
	dec r8d
	mov edx, r8d
	shr r9d, 1
	jmp iterate

found:
	mov eax, r8d
	ret

notFound:
	mov eax, -1
	ret