RED:
push 0
pop [AX]
call INCREMENT_AX:
push 50
push AX
ja RED:

GREEN:
push 1
pop [AX]
call INCREMENT_AX:
push 75
push AX
ja GREEN:

BLUE:
push 2
pop [AX]
call INCREMENT_AX:
push 100
push AX
ja BLUE:

draw 2 0 10
dump

hlt

INCREMENT_AX:
push AX
push 1
add
pop AX
ret
