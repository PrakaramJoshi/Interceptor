IF _M_X64
;External function used to Find out the name of th function given its address
extern on_enter:Proc
extern on_exit:Proc

.code
;--------------------------------------------------------------------
; _penter procedure
;--------------------------------------------------------------------
	PUSHREGS	macro
		push rax      ;save current rax
		push rbx      ;save current rbx
		push rcx      ;save current rcx
		push rdx      ;save current rdx
		push rbp      ;save current rbp
		push rdi      ;save current rdi
		push rsi      ;save current rsi
		push r8         ;save current r8
		push r9         ;save current r9
		push r10      ;save current r10
		push r11      ;save current r11
		push r12      ;save current r12
		push r13      ;save current r13
		push r14      ;save current r14
		push r15      ;save current r15
	endm

	POPREGS	macro
	   pop r15         ;restore current r15
	   pop r14         ;restore current r14
	   pop r13         ;restore current r13
	   pop r12         ;restore current r12
	   pop r11         ;restore current r11
	   pop r10         ;restore current r10
	   pop r9         ;restore current r9
	   pop r8         ;restore current r8
	   pop rsi         ;restore current rsi
	   pop rdi         ;restore current rdi
	   pop rbp         ;restore current rbp
	   pop rdx         ;restore current rdx
	   pop rcx         ;restore current rcx
	   pop rbx         ;restore current rbx
	   pop rax         ;restore current rax
	endm

_penter proc

	; Store the volatile registers
	PUSHREGS
	; reserve space for 4 registers [ rcx,rdx,r8 and r9 ] 32 bytes
	sub  rsp,20h 

	; Get the return address of the function
	mov  rcx,rsp
	mov  rcx,qword ptr[rcx+98h]
	sub  rcx,5
	
	;call the function to get the name of the callee and caller	
	call on_enter

	;Release the space reserved for the registersk by adding 32 bytes
	add  rsp,20h 

	;Restore the registers back by poping out
	POPREGS

	ret

_penter endp

;--------------------------------------------------------------------
; _pexit procedure
;--------------------------------------------------------------------
_pexit proc
	
	; Store the volatile registers
	PUSHREGS
	; reserve space for 4 registers [ rcx,rdx,r8 and r9 ] 32 bytes
	sub  rsp,20h 

	; Get the return address of the function
	mov  rcx,rsp
	mov  rcx,qword ptr[rcx+98h]
	sub  rcx,5
	
	;call the function to get the name of the callee and caller	
	call on_exit

	;Release the space reserved for the registersk by adding 32 bytes
	add  rsp,20h 

	;Restore the registers back by poping out
	POPREGS

	ret

	
_pexit endp

ENDIF
end