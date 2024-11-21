push 6

call FACTORIAL:

push AX
out

hlt

FACTORIAL:
    pop DX ; ret ip
    pop AX ; factorial_arg
    push DX
    push AX

    push 1

    jne RECURSION_STEP:
        ret
    RECURSION_STEP:
        push AX
        push AX
        push 1
        sub

        call FACTORIAL:

        push AX
        mul

        pop AX

    ret
