3:
call INCREMENT_AX:
push AX
push AX
mul
out
push 10
push AX
ja 3:
hlt
INCREMENT_AX:
push 1
push AX
add
pop AX
ret
