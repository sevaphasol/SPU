push 1
pop BX

STEP:
    call INCREMENT_AX:

    push AX
    push BX
    mul
    pop BX

    push AX
    push 3

jb STEP:

push BX

meow BX
hlt

INCREMENT_AX:
push 1
push AX
add
pop AX
ret
