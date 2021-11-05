    addi x2, x0, 2          # const
    addi x20, x0, 1000      # address of arr
    add x1, x0, x0         # sum
    add x22, x0, x0         # i
    addi x23, x0, 10         # size

loop1:

    sll x24, x22, x2
    add x24, x24, x20
    sw x22, 0(x24)          # arr[i] = x22
    addi x22, x22, 1
    blt x22, x23, loop1

    add x22, x0, x0         # i = 0

loop2:

    sll x24, x22, x2
    add x24, x24, x20
    lw x25, 0(x24)          # sum += arr[i]
    add x1, x1, x25
    addi x22, x22, 1
    blt x22, x23, loop2

