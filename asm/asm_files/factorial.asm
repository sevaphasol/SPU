push 1
pop BX

STEP:
    call INCREMENT_AX:

    push AX
    push BX
    mul
    pop BX

    push AX
    push 5

jb STEP:

push BX
out

hlt

INCREMENT_AX:
push 1
push AX
add
pop AX
ret
