; Robert Owens (rao7utn)
; mathlib.s
; 10/26/2020

	global product

	section .text

product:
	; Standard prologue: we do not have to create any local
	; variables (those values will be kept in registers), and 
	; we are not using any callee-saved registers.

	; rsi is the counter (2nd param)
	; rdi is the number to coninually add with (1st param)

	; Subroutine body:
	xor rax, rax	; zero out the return register
	xor r10, r10 	; zero out the counter i
prodStart:
	cmp r10, rsi 	; does i == n?
	je prodDone	; if yes, we are done with the loop
	add rax, rdi 	; add first param to itself
	inc r10 		; incriment counter i by 1
	jmp prodStart 	; this itteration of the loop is finished
prodDone:
	; Standard epilogue: the return value is already in rax, we
	; do not have any callee-saved registers to restore, and we do not
	; have any local variables to deallocate, so all we do is return
	ret

	global power

	section .text

; rsi will hold the power (2nd param)
; rdi will hold the base (1st param)
; No backing up required only the power
; 	is modified and it is not used after
; 	the recursive call 
power:
	; check base case power == 0
	cmp rsi, 0
	je powBase

	; Recursive case (power -1)
	dec rsi
	call power
	mov rsi, rax
	call product ; ((((1*rdi)*rdi)*rdi)...)
	ret

; x^0 = 1 always
powBase:
	mov rax, 1
	ret