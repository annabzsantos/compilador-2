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
;Carrega valor de variavel global
mov eax, dword [x]
push rax
;Passa argumento 1 para chamada de funcao
pop rax
mov r8, rax
;Carrega valor de variavel global
mov eax, dword [y]
push rax
;Passa argumento 2 para chamada de funcao
pop rax
mov r9, rax
;Chamada de funcao
call func_calcula

; --- escreve valor inteiro ---
mov eax, dword [resultado]
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

;Prologo de funcao func_calcula
func_calcula:
push rbp
mov rbp, rsp
sub rsp, 64
mov dword [rbp - 4], r8d
mov dword [rbp - 8], r9d

;Epilogo da funcao
mov rsp, rbp
pop rbp
ret

	section .data
buffer_io db 0, 0
x: dd 0
y: dd 0
resultado: dd 0
str0: db "Informe o valor de X:"
str1: db "\nInforme o valor de Y:"
