push 5

call FACTORIAL:

push AX
out

hlt

FACTORIAL:
    pop  DX
    pop  AX
    push DX

    push AX
    push 1

    jne STEP:
        ret

    STEP:
        push AX
        push AX
        push 1
        sub

        call FACTORIAL:

        push AX
        mul
        pop AX

    ret

