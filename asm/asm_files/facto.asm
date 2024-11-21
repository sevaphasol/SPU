push 6

call FACTORIAL:

push AX
out

hlt

FACTORIAL:
    pop DX ; change places of ret ip and factorial arg
    pop AX
    push DX
    push AX

    push 1

    jne RECURSION_STEP:
        ret ; AX = 1 break recurion

    RECURSION_STEP:
        push AX ; save AX
        push AX
        push 1
        sub     ; push decremented AX for next recursion step

        call FACTORIAL:

        push AX
        mul     ; in stack we have |AX + 1||AX|

        pop AX

    ret
