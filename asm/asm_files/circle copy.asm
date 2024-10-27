push 5
pop AX

push 0
pop BX

push 0
pop CX

FILL_COLL:
    push 0
    pop CX
    FILL_RAW:
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

        jbe DRAW_RED:
        DRAW_RED:
            push 2

            jmp RAW_STEP:

        ja DRAW_WHITE:
        DRAW_WHITE:
            push 0

            jmp RAW_STEP:

        RAW_STEP:
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

        jb  FILL_RAW:
        jae COLL_STEP:
    COLL_STEP:
        call INCREMENT_BX:

        push BX
        push AX

        jb FILL_COLL:

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
