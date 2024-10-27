push 1
pop AX
push 1
pop BX
push 2
pop CX

STEP:
    call INCREMENT_CX:

    push AX
    push BX
    add
    pop DX

    push BX
    pop AX

    push DX
    pop BX

    push BX
    out

    push CX
    push 50
jb STEP:

hlt

INCREMENT_CX:
push 1
push CX
add
pop CX
ret
