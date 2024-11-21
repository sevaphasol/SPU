push 5

call FACTORIAL:

push AX
out

hlt

FACTORIAL:
    pop DX

    push 1

    ja STEP:
        push 1
        pop AX

        push DX
        ret

    STEP:
        push 1
        sub

        call FACTORIAL:

        push AX
        push 1
        add
        push AX
        mul
        pop AX

    push DX
    ret
