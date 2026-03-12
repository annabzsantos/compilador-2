# UFMT - Compiladores
# Prof. Ivairton
# Montagem: $ spim -file <arquivo>.asm
#        ou: $ mars <arquivo>.asm

.data
newline: .asciiz "\n"
x: .word 0
y: .word 0
resultado: .word 0
valor: .word 0

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
# Chamada: jal func_soma
jal  func_soma
# Captura retorno ($v0) -> 'resultado'
la   $t1, resultado
sw   $v0, 0($t1)

# Escreve string 'str2'
la   $a0, str2
li   $v0, 4
syscall

# Escreve inteiro 'resultado'
la   $t1, resultado
lw   $a0, 0($t1)
li   $v0, 1
syscall
# Carrega global 'resultado'
la   $t1, resultado
lw   $t0, 0($t1)
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Argumento 1
lw   $t0, 0($sp)
addi $sp, $sp, 4
move $a0, $t0
# Chamada: jal func_testeEscopo
jal  func_testeEscopo
# Captura retorno ($v0) -> 'resultado'
la   $t1, resultado
sw   $v0, 0($t1)

# Escreve string 'str3'
la   $a0, str3
li   $v0, 4
syscall

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
str0: .asciiz "Digite o valor de X:"
str1: .asciiz "\nDigite o valor de Y:"
str2: .asciiz "\nResultado da soma:"
str3: .asciiz "\nResultado apos teste de escopo:"

# Prologo de func_soma
func_soma:
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

# Prologo de func_testeEscopo
func_testeEscopo:
addi $sp, $sp, -8
sw   $ra, 4($sp)
sw   $fp, 0($sp)
move $fp, $sp
addi $sp, $sp, -64
sw   $a0, -4($fp)
# Carrega local 'valor'
lw   $t0, -4($fp)
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Numero imediato 2
li   $t0, 2
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Multiplicacao
lw   $t1, 0($sp)
addi $sp, $sp, 4
lw   $t0, 0($sp)
addi $sp, $sp, 4
mul  $t0, $t0, $t1
addi $sp, $sp, -4
sw   $t0, 0($sp)
# Atribuicao -> 'x'
lw   $t0, 0($sp)
addi $sp, $sp, 4
sw   $t0, -8($fp)
# Carrega local 'x'
lw   $t0, -8($fp)
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
