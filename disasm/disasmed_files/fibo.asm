push 1
push 1
push 12
call 16 
push AX
out 
hlt 
pop DX
pop CX
pop AX
pop BX
push DX
push CX
push 1
jne 40 
ret 
push AX
push AX
push BX
add 
push BX
out 
push CX
push 1
sub 
call 16 
ret 
