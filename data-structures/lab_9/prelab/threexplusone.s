; Robert Owens (rao7utn)
; threexplusone.s
; 11/01/2020
; University of Virginia
; CS 2150 Pre-Lab 9
; Fall 2020

; OPTOMIZATIONS
; lea for multiplication
; right arethmetic shift for division
; memory accessed only once (in odd so not even every time)


	global threexplusone

	section .text

; Parameter 1 (edi): x the number to apply the function onto
; local var temp (esi): for minipulating x safely

; Return (eax) the number of steps before an input reached one

threexplusone:
	xor rax, rax

	cmp edi, 1
	je done

	mov sil, dil
	and sil, 1
	cmp sil, 1
	je odd
	jmp even
	

; when input == 1. return rax (the number of steps)
done:
	ret

; when x is an even number compute x/2 and recurse
even:
	shr edi, 1
	call threexplusone
	inc eax
	ret

; when x is an odd number compute 3x+1 and recurse
odd:
	lea edi, [3*edi]
	inc edi
	call threexplusone
	inc eax
	ret