push 10
pop AX

push -10
pop BX

push BX
push AX
add
pop DI

push 21
pop  SI

COLL:
    push -10
    pop CX

    RAW:
        push BX
        push BX
        mul
        push 4
        mul
        push CX
        push CX
        mul
        add

        push AX
        push AX
        mul

        jbe DOT:

        push 32
        jmp STEP:

        DOT:
            push 42

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

draw 0 DI SI SI

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
