push 0 
pop [AX] 
call 70 
push 100 
push AX 
ja 0 
draw 0 0 10 
dump
push 1 
pop [AX] 
call 70 
push 200 
push AX 
ja 23 
draw 0 0 10 
dump
push 2 
pop [AX] 
call 70 
push 300 
push AX 
ja 46 
draw 0 0 10 
dump
hlt
push AX 
push 1 
add
pop AX 
ret
