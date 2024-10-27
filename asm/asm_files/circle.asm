push 5
pop AX

push 0
pop BX

push 15
pop  DX

COLL:
    push 0
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

        jb RED:

        push 0

        jmp STEP:

        RED:
            push 2
        STEP:

        push BX
        push AX
        mul
        push CX
        add
        pop DX

        pop [DX]

        call INCREMENT_CX:
        push CX
        push AX
    jb RAW:

    call INCREMENT_BX:
    push BX
    push AX
jb COLL:


draw 0 0 5
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
