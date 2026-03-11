# UFMT - Compiladores
# Prof. Ivairton
# Montagem: $ spim -file <arquivo>.asm
#        ou: $ mars <arquivo>.asm

.data
newline: .asciiz "\n"
x: .word 0
y: .word 0
igual: .word 0
diferente: .word 0

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
# Numero imediato 0
li   $t0, 0
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Atribuicao -> 'igual'
lw   $t0, 0($sp)
addi $sp, $sp, 4
la   $t1, igual
sw   $t0, 0($t1)
# Numero imediato 0
li   $t0, 0
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Atribuicao -> 'diferente'
lw   $t0, 0($sp)
addi $sp, $sp, 4
la   $t1, diferente
sw   $t0, 0($t1)
# Carrega global 'x'
la   $t1, x
lw   $t0, 0($t1)
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Carrega global 'y'
la   $t1, y
lw   $t0, 0($t1)
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Expressao booleana
lw   $t1, 0($sp)
addi $sp, $sp, 4
lw   $t0, 0($sp)
addi $sp, $sp, 4
beq  $t0, $t1, bool_label0
li   $t0, 0
j    bool_end0
bool_label0:
li   $t0, 1
bool_end0:
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Salto condicional
lw   $t0, 0($sp)
addi $sp, $sp, 4
beq  $t0, $zero, label0
# Numero imediato 1
li   $t0, 1
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Atribuicao -> 'igual'
lw   $t0, 0($sp)
addi $sp, $sp, 4
la   $t1, igual
sw   $t0, 0($t1)
# Salto incondicional
j    label1
label0:
label1:
# Carrega global 'x'
la   $t1, x
lw   $t0, 0($t1)
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Carrega global 'y'
la   $t1, y
lw   $t0, 0($t1)
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Expressao booleana
lw   $t1, 0($sp)
addi $sp, $sp, 4
lw   $t0, 0($sp)
addi $sp, $sp, 4
bne  $t0, $t1, bool_label1
li   $t0, 0
j    bool_end1
bool_label1:
li   $t0, 1
bool_end1:
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Salto condicional
lw   $t0, 0($sp)
addi $sp, $sp, 4
beq  $t0, $zero, label2
# Numero imediato 1
li   $t0, 1
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Atribuicao -> 'diferente'
lw   $t0, 0($sp)
addi $sp, $sp, 4
la   $t1, diferente
sw   $t0, 0($t1)
# Salto incondicional
j    label3
label2:
label3:

# Escreve inteiro 'igual'
la   $t1, igual
lw   $a0, 0($t1)
li   $v0, 1
syscall

# Escreve inteiro 'diferente'
la   $t1, diferente
lw   $a0, 0($t1)
li   $v0, 1
syscall

# Encerra programa (syscall exit)
li   $v0, 10
syscall
