;UFMT-Compiladores
;Prof. Ivairton
;Procedimento para geracao do executavel apos compilacao (em Linux):
;(1) compilacao do Assembly com nasm: $ nasm -f elf64 <nome_do_arquivo>.asm
;(2) likedicao: $ ld -m elf_x86_64 <saida> <nome_arquivo_objeto>.o

extern printf
extern scanf

	section .text
	global main,_start
main:
_start:

;escreve valor string
mov edx,16
mov ecx,str0
mov ebx,1
mov eax,4
int 0x80

; --- le valor inteiro ---
mov edx, 2
mov ecx, buffer_io
mov ebx, 0
mov eax, 3
int 0x80
movzx eax, byte [buffer_io]
sub eax, 48
mov dword [x], eax

;escreve valor string
mov edx,16
mov ecx,str1
mov ebx,1
mov eax,4
int 0x80

; --- le valor inteiro ---
mov edx, 2
mov ecx, buffer_io
mov ebx, 0
mov eax, 3
int 0x80
movzx eax, byte [buffer_io]
sub eax, 48
mov dword [y], eax
;Amarzenamento de numero
mov rax,0
push rax
;Atribuicao para variavel 'igual'
pop rax
mov [igual], eax
;Amarzenamento de numero
mov rax,0
push rax
;Atribuicao para variavel 'diferente'
pop rax
mov [diferente], eax
;Carrega valor de variavel global
mov eax, dword [x]
push rax
;Carrega valor de variavel global
mov eax, dword [y]
push rax
;Aplica operador booleano/exp.logica
pop rbx
pop rax
mov rcx,1
cmp eax,ebx
je bool_label0
mov rcx,0
bool_label0:
mov rax, rcx
push rax
;jump condicional
pop rax
cmp rax, 0
jz label0
;Amarzenamento de numero
mov rax,1
push rax
;Atribuicao para variavel 'igual'
pop rax
mov [igual], eax
;jump incondicional
jmp label1
label0:
label1:
;Carrega valor de variavel global
mov eax, dword [x]
push rax
;Carrega valor de variavel global
mov eax, dword [y]
push rax
;Aplica operador booleano/exp.logica
pop rbx
pop rax
mov rcx,1
cmp eax,ebx
jne bool_label1
mov rcx,0
bool_label1:
mov rax, rcx
push rax
;jump condicional
pop rax
cmp rax, 0
jz label2
;Amarzenamento de numero
mov rax,1
push rax
;Atribuicao para variavel 'diferente'
pop rax
mov [diferente], eax
;jump incondicional
jmp label3
label2:
label3:

; --- escreve valor inteiro ---
mov eax, dword [igual]
add eax, 48
mov byte [buffer_io], al
mov edx, 1
mov ecx, buffer_io
mov ebx, 1
mov eax, 4
int 0x80

; --- escreve valor inteiro ---
mov eax, dword [diferente]
add eax, 48
mov byte [buffer_io], al
mov edx, 1
mov ecx, buffer_io
mov ebx, 1
mov eax, 4
int 0x80

;encerra programa
mov ebx,0
mov eax,1
int 0x80

	section .data
buffer_io db 0, 0
x: dd 0
y: dd 0
igual: dd 0
diferente: dd 0
str0: db "Informe o valor de X:"
str1: db "Informe o valor de Y:"
