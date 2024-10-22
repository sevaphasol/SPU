WHITE:
push 0
pop [AX]
call INCREMENT_AX:











push 100
push AX
ja WHITE:
draw 0 10
dump
BLUE:
push 0
pop [AX]
call INCREMENT_AX:
push 100
push AX
ja BLUE:
draw 0 10
dump
hlt
INCREMENT_AX:
push AX
push 1
add
pop AX
ret
