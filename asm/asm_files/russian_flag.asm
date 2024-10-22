WHITE:
push 0
pop [AX]
push AX
push 1
add
pop AX
push 100
push AX
ja WHITE:

draw 0 10
dump

BLUE:
push 1
pop [AX]
push AX
push 1
add
pop AX
push 200
push AX
ja BLUE:

draw 100 10
dump

RED:
push 2
pop [AX]
push AX
push 1
add
pop AX
push 300
push AX
ja RED:

draw 200 10
dump

hlt
