WHITE:
push 0
pop [AX]
call INCREMENT_AX:
push 100
push AX
ja WHITE:

draw 0 0 10
dump

BLUE:
push 1
pop [AX]
call INCREMENT_AX:
push 200
push AX
ja BLUE:

draw 0 100 10
dump

RED:
push 2
pop [AX]
call INCREMENT_AX:
push 300
push AX
ja RED:

draw 0 200 10
dump

hlt

INCREMENT_AX:
push AX
push 1
add
pop AX
ret
