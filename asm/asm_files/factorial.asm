push 4

call FUNC:

push AX
out

hlt

FUNC:
    dump
    pop DX
    pop AX
    push DX
    push AX

    pop AX
    push AX
    push 1

    jne Note:
        push 1
        pop AX

        ret

    Note:
        push AX

        push AX
        push 1
        sub

        call FUNC:

        push AX
        mul

        pop AX

    ret
