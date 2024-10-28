push 2
push 2
mul
pop [AX]
push 2
push 2
add
pop [1]
push [AX]
push [AX + 1]
mul
push 1 + BX
add
out
HUY:
jb HUY:
hlt

call INPUT:
INPUT:
in
in
in

pop CX
pop BX
pop AX
ret

