STEP:
push 64
pop [AX]
call INCREMENT_AX:
push 100
push AX
jae STEP:

draw 0 0 10 10
dump

hlt

INCREMENT_AX:
push AX
push 1
add
pop AX
ret
