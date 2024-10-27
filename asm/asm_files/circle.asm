push 10
pop AX

push -10
pop BX

push 21
pop  SI

COLL:
    push -10
    pop CX

    RAW:
        push BX
        push BX
        mul
        push CX
        push CX
        mul
        add

        push AX
        push AX
        mul

        push AX
        out
        push BX
        out
        push CX
        out

        jb RED:

        push 0
        jmp STEP:

        RED:
            push 2

        STEP:
        push BX
        push 10
        add
        push SI
        mul
        push CX
        push 10
        add
        add
        pop DX

        pop [DX]

        call INCREMENT_CX:
        push CX
        push SI
    jb RAW:

    call INCREMENT_BX:
    push BX
    push SI
jb COLL:


draw 0 0 21
hlt

INCREMENT_BX:
push 1
push BX
add
pop BX
ret

INCREMENT_CX:
push 1
push CX
add
pop CX
ret
