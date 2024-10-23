SQUARE:
push 1
dump
pop [AX]
push 225
push AX
push 1
push AX
add
pop AX
ja SQUARE:
draw 0 0 15
hlt
