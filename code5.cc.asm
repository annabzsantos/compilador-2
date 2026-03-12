# UFMT - Compiladores
# Prof. Ivairton
# Montagem: $ spim -file <arquivo>.asm
#        ou: $ mars <arquivo>.asm

.data
newline: .asciiz "\n"
x: .word 0

.text
.globl main
main:
# Numero imediato 10
li   $t0, 10
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Argumento 1
lw   $t0, 0($sp)
addi $sp, $sp, 4
move $a0, $t0
# Numero imediato 20
li   $t0, 20
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Argumento 2
lw   $t0, 0($sp)
addi $sp, $sp, 4
move $a1, $t0
# Numero imediato 30
li   $t0, 30
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Argumento 3
lw   $t0, 0($sp)
addi $sp, $sp, 4
move $a2, $t0
# Numero imediato 40
li   $t0, 40
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Argumento 4
lw   $t0, 0($sp)
addi $sp, $sp, 4
move $a3, $t0
# Numero imediato 50
li   $t0, 50
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Argumento 5
lw   $t0, 0($sp)
addi $sp, $sp, 4
move â, $t0
# Chamada: jal func_soma
jal  func_soma
# Captura retorno ($v0) -> 'x'
la   $t1, x
sw   $v0, 0($t1)

# Escreve inteiro 'x'
la   $t1, x
lw   $a0, 0($t1)
li   $v0, 1
syscall

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
sw   $a1, -8($fp)
sw   $a2, -12($fp)
sw   $a3, -16($fp)
# Carrega local 'a'
lw   $t0, -4($fp)
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Carrega local 'b'
lw   $t0, -8($fp)
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Adicao
lw   $t1, 0($sp)
addi $sp, $sp, 4
lw   $t0, 0($sp)
addi $sp, $sp, 4
add  $t0, $t0, $t1
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Carrega local 'c'
lw   $t0, -12($fp)
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Adicao
lw   $t1, 0($sp)
addi $sp, $sp, 4
lw   $t0, 0($sp)
addi $sp, $sp, 4
add  $t0, $t0, $t1
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Carrega local 'd'
lw   $t0, -16($fp)
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Adicao
lw   $t1, 0($sp)
addi $sp, $sp, 4
lw   $t0, 0($sp)
addi $sp, $sp, 4
add  $t0, $t0, $t1
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Carrega local 'e'
lw   $t0, -20($fp)
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Adicao
lw   $t1, 0($sp)
addi $sp, $sp, 4
lw   $t0, 0($sp)
addi $sp, $sp, 4
add  $t0, $t0, $t1
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Atribuicao -> 'resultado'
lw   $t0, 0($sp)
addi $sp, $sp, 4
sw   $t0, -24($fp)
# Carrega local 'resultado'
lw   $t0, -24($fp)
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
