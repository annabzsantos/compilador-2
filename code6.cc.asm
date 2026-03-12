# UFMT - Compiladores
# Prof. Ivairton
# Montagem: $ spim -file <arquivo>.asm
#        ou: $ mars <arquivo>.asm

.data
newline: .asciiz "\n"
x: .word 0
y: .word 0

.text
.globl main
main:
# Numero imediato 1
li   $t0, 1
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Atribuicao -> 'x'
lw   $t0, 0($sp)
addi $sp, $sp, 4
la   $t1, x
sw   $t0, 0($t1)
# Carrega global 'x'
la   $t1, x
lw   $t0, 0($t1)
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Atribuicao -> 'y'
lw   $t0, 0($sp)
addi $sp, $sp, 4
la   $t1, y
sw   $t0, 0($t1)

# Encerra programa (syscall exit)
li   $v0, 10
syscall

# Prologo de func_soma
func_soma:
addi $sp, $sp, -8
sw   $ra, 4($sp)
sw   $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -64
sw   $a0, -4($fp)
# Numero imediato 1
li   $t0, 1
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Atribuicao -> 'a'
lw   $t0, 0($sp)
addi $sp, $sp, 4
sw   $t0, -4($fp)
# Numero imediato 1
li   $t0, 1
addi $sp, $sp, -4
sw   $t0, 0($sp)
# return -> $v0
lw   $v0, 0($sp)
addi $sp, $sp, 4

# Epilogo da funcao
move $sp, $fp
lw   $fp, 0($sp)
lw   $ra, 4($sp)
addi $sp, $sp, 8
jr   $ra
