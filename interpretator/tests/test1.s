    addi x10, x0, 15         # x10 is n
fib:
    addi x1, x0, 1
    addi x2, x0, 1
    addi x3, x0, 2
    addi x4, x0, 2
   
    addi x6, x0, 2
    bge x6, x10, cond1

head:
    add x3, x1, x2
    add x1, x0, x2
    add x2, x0, x3
    addi x4, x4, 1
    blt x4, x10, head
    bge x4, x10, end

cond1:
    addi x1, x0, 1
    jal x0, 8
end:
    add x1, x0, x2
    