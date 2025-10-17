.data

.text
.globl main
main:
    # Allocate stack space
    addi $sp, $sp, -400

    # Declared i at offset 0
    # Print integer
    move $a0, $t-1
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall

    # Exit program
    addi $sp, $sp, 400
    li $v0, 10
    syscall
