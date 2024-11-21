push 1
pop AX
push AX + 1
pop AX
push 2
pop [AX + 1]
push [AX]
pop [2 + AX]
hlt
