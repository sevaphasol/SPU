push 2 
push 2 
mul
pop [AX] 
push 2 
push 2 
add
pop [1] 
push [AX] 
push [1 + AX] 
mul
push 1 + BX 
add
out
jb 34 
hlt
push 1 
push 1 
add
pop [1 + CX] 
ret
call 37 
