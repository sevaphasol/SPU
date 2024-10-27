push 1
pop BX

call FACTORIAL:

push BX
out

hlt

FACTORIAL:
call INCREMENT_AX:
push AX
push BX
mul
pop BX

push AX
push 30
jb STEP:
pop [0]
pop [0]
ret
STEP:
pop [0]
pop [0]
call FACTORIAL:
ret

INCREMENT_AX:
push 1
push AX
add
pop AX
ret
