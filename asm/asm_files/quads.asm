3:
push 1
push AX
add
pop AX
push AX
push AX
mul
out
push 10
push AX
ja 3:
hlt
