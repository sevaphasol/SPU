push 1
push 1

push 12

call FIBO:

push AX
out

hlt

FIBO:
    pop DX ; pop return_ip
    pop CX ; pop counter
    pop AX ; pop F(n-1)
    pop BX ; pop F(n-2)

    push DX ; push return ip

    push CX
    push 1

    jne STEP:
        ret
    STEP:
        push AX ; push F(n-1)
        push AX
        push BX
        add     ; push F(n) = F(n-1) + F(n-2)

        push BX ; print F(n-2)
        out

        push CX
        push 1
        sub     ; counter--

        call FIBO:

    ret
