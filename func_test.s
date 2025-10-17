.data

.text
.globl main
main:
    # Allocate stack space
    addi $sp, $sp, -400

inc:
    addi $sp, $sp, -8
    sw $ra, 4($sp)
    sw $fp, 0($sp)
    addi $fp, $sp, 8
    li $t0, 6
    move $v0, $t0
    jr $ra
    lw $fp, 0($sp)
    lw $ra, 4($sp)
    addi $sp, $sp, 8
    jr $ra
    # Declared main at offset 0
    li $t0, 0
    sw $t0, 0($sp)
    sw $t-1, 0($sp)
    lw $t0, 0($sp)
    # Print integer
    move $a0, $t0
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
