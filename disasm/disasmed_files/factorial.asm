push 1 
pop BX 
call 13 
push BX 
out
hlt
call 49 
push AX 
push BX 
mul
pop BX 
push AX 
push 5 
jb 40 
pop [0] 
pop [0] 
ret
pop [0] 
pop [0] 
call 13 
ret
push 1 
push AX 
add
pop AX 
ret
