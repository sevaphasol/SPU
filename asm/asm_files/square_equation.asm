call INPUT:

call DISCRIMINANT:

push DX
push 0

ja TWO_ROOTS:
je ONE_ROOT:

hlt

TWO_ROOTS:

push DX
sqrt
pop DX

call FIRST_ROOT:
call SECOND_ROOT:

hlt

ONE_ROOT:

call FIRST_ROOT:

hlt

INPUT:
in
in
in

pop CX
pop BX
pop AX
ret

DISCRIMINANT:
push BX
push BX
mul
push 4
push AX
mul
push CX
mul
sub
pop DX
ret

FIRST_ROOT:
push -1
push BX
mul
push DX
add
push 2
push AX
mul
div
out
ret

SECOND_ROOT:
push -1
push BX
mul
push -1
push DX
mul
add
push 2
push AX
mul
div
out
ret
