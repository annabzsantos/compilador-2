# UFMT - Compiladores
# Prof. Ivairton
# Montagem: $ spim -file <arquivo>.asm
#        ou: $ mars <arquivo>.asm

.data
newline: .asciiz "\n"
x: .word 0
y: .word 0
resultado: .word 0

.text
.globl main
main:

# Escreve string 'str0'
la   $a0, str0
li   $v0, 4
syscall

# Le inteiro -> 'x'
li   $v0, 5
syscall
la   $t1, x
sw   $v0, 0($t1)

# Escreve string 'str1'
la   $a0, str1
li   $v0, 4
syscall

# Le inteiro -> 'y'
li   $v0, 5
syscall
la   $t1, y
sw   $v0, 0($t1)
# Carrega global 'x'
la   $t1, x
lw   $t0, 0($t1)
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Argumento 1
lw   $t0, 0($sp)
addi $sp, $sp, 4
move $a0, $t0
# Carrega global 'y'
la   $t1, y
lw   $t0, 0($t1)
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Argumento 2
lw   $t0, 0($sp)
addi $sp, $sp, 4
move $a1, $t0
# Chamada: jal func_calcula
jal  func_calcula

# Escreve inteiro 'resultado'
la   $t1, resultado
lw   $a0, 0($t1)
li   $v0, 1
syscall

# Encerra programa (syscall exit)
li   $v0, 10
syscall

# Strings literais
.data
str0: .asciiz "Informe o valor de X:"
str1: .asciiz "\nInforme o valor de Y:"

# Prologo de func_calcula
func_calcula:
addi $sp, $sp, -8
sw   $ra, 4($sp)
sw   $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -64
sw   $a0, -4($fp)
sw   $a1, -8($fp)
# Carrega local 'a'
lw   $t0, -4($fp)
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Atribuicao -> 'r'
lw   $t0, 0($sp)
addi $sp, $sp, 4
sw   $t0, -12($fp)
# Carrega local 'r'
lw   $t0, -12($fp)
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
