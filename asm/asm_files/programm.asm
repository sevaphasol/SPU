push [1]
push 1 + CX
add
push 2
mul
dump
out
NEXT:
jmp NEXT:
ja 2
jb NEXT:
je 1
hlt
