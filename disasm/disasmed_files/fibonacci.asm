push 1 
pop AX 
push 1 
pop BX 
push 2 
pop CX 
call 55 
push AX 
push BX 
add
pop DX 
push BX 
pop AX 
push DX 
pop BX 
push BX 
out
push CX 
push 10 
jb 18 
hlt
push 1 
push CX 
add
pop CX 
ret
