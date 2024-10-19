push [1]
push 1 + CX
add
push 2
mul
dump
out
3:
jmp 3:
ja 2
jb 5
je 1
hlt
