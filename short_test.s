.data

.text
.globl main
main:
    # Allocate stack space
    addi $sp, $sp, -400

    # Declared a at offset 0
    # Declared b at offset 4
    li $t0, 7
    li $t1, 3
    li $t2, 2
    mult $t1, $t2
    mflo $t1
    add $t0, $t0, $t1
    sw $t0, 0($sp)
    lw $t0, 0($sp)
    li $t1, 5
    li $t2, 1
    sub $t1, $t1, $t2
    div $t0, $t1
    mflo $t0
    sw $t0, 4($sp)
    lw $t0, 0($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
    lw $t0, 4($sp)
    # Print integer
    move $a0, $t0
    li $v0, 1
    syscall
    # Print newline
    li $v0, 11
    li $a0, 10
    syscall
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
