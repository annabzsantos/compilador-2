/**
 * @file gen.c
 * @author Anna Bheatryz Martins dos Santos e Mariana Sanchez Pedroni
 * @brief Codificacao do modulo gerador de codigo
 * @version 0.2
 * @date 2022-02-23
 * 
 */

// Inclusao do cabecalho
#include "gen.h"

// Inclusao explicita de variaveis globais de outro contexto (symbols.h)
extern type_symbol_table_variables global_symbol_table_variables;
extern type_symbol_table_string symbol_table_string;
char output_file_name[MAX_CHAR];
FILE *output_file;

/**
 * @brief Funcao que gera codigo de montagem para SOMA
 * 
 */
void genAdd() {
    fprintf(output_file, ";Adicao\n");
    fprintf(output_file, "pop rax\n");
    fprintf(output_file, "pop rbx\n");
    fprintf(output_file, "add rax,rbx\n");
    fprintf(output_file, "push rax\n");
}

/**
 * @brief Funcao que gera codigo de montagem para SUBTRACAO
 * 
 */
void genSub() {
    fprintf(output_file, ";Subtracao\n");
    fprintf(output_file, "pop rbx\n");
    fprintf(output_file, "pop rax\n");
    fprintf(output_file, "sub rax,rbx\n");
    fprintf(output_file, "push rax\n");
}

/**
 * @brief Funcao que gera codigo de montagem para MULTIPLICACAO
 * 
 */
void genMult() {
    fprintf(output_file, ";Multiplicacao\n");
    fprintf(output_file, "pop rax\n");
    fprintf(output_file, "pop rbx\n");
    fprintf(output_file, "imul rax,rbx\n");
    fprintf(output_file, "push rax\n");
}

/**
 * @brief Funcao que gera codigo de montagem para DIVISAO
 * 
 */
void genDiv() {
    fprintf(output_file, ";Divisao\n");
    fprintf(output_file, "pop rbx\n");
    fprintf(output_file, "pop rax\n");
    fprintf(output_file, "idiv rax,rbx\n");
    fprintf(output_file, "push rax\n");
}

/**
 * @brief Funcao que gera codigo de montagem para armazenamento de NUMERAL
 * 
 * @param num_string 
 */
void genNum(char num_string[MAX_TOKEN]) {
    fprintf(output_file, ";Amarzenamento de numero\n");
    fprintf(output_file, "mov rax,%s\n", num_string);
    fprintf(output_file, "push rax\n");
}

/**
 * @brief gera codigo de atribuicao de valor a uma variavel
 * exemplo: mov [id], eax
 * @param char* nome da variavel
 */
void gen_assign(char *var_name){
    type_symbol_table_entry *sym = sym_find_any(var_name);
    if (sym == NULL) {
        printf("[ERRO] Variavel '%s' nao declarada.\n", var_name);
        return;
    }
    fprintf(output_file, ";Atribuicao para variavel '%s'\n", var_name);
    fprintf(output_file, "pop rax\n"); // valor a ser atribuido esta no topo da pilha

    if(sym -> is_global) {
        fprintf(output_file, "mov [%s], eax\n", sym->name); // Acessa a variável global pelo nome
    } else {
        fprintf(output_file, " mov dword [rbp - %d], eax\n", sym->addr); 
    }
}

/**
 * @brief Funcao que gera um preambulo que permite o uso das funcoes do C (scanf e printf)
 * 
 */
void gen_preambule(void) {
    fprintf(output_file, ";UFMT-Compiladores\n");
    fprintf(output_file, ";Prof. Ivairton\n");
    fprintf(output_file, ";Procedimento para geracao do executavel apos compilacao (em Linux):\n");
    fprintf(output_file, ";(1) compilacao do Assembly com nasm: $ nasm -f elf64 <nome_do_arquivo>.asm\n");
    fprintf(output_file, ";(2) likedicao: $ ld -m elf_x86_64 <saida> <nome_arquivo_objeto>.o\n\n");
    fprintf(output_file, "extern printf\n");
    fprintf(output_file, "extern scanf\n");
}

/**
 * @brief Funcao que gera codigo da secao de dados (declaracao de variaveis).
 */
void gen_data_section(void) {
    int i, n;
    
    fprintf(output_file, "\n\tsection .data\n");
    fprintf(output_file, "buffer_io db 0, 0\n"); // Buffer para leitura de input do usuario
    
    // processa cada simbolo da tabela e gera um ponteiro para cada variavel na memoria
    n = global_symbol_table_variables.n_variables;
    for (i = 0; i < n; i++) {
       fprintf(output_file, "%s: ", global_symbol_table_variables.variable[i].name); 
       
       switch(global_symbol_table_variables.variable[i].type) { //Por enquanto gera endereco zero
            case INT:
                fprintf(output_file, "dd 0\n");
                break;
            case STRING:
                fprintf(output_file, "db \"                \" \n");
                break;
            case FLOAT:
                fprintf(output_file, "dd \"%%lf\", 16\n");
                break;
            case CHAR:
                fprintf(output_file, "dd \"%%c\", 1\n");
                break;
            default:
                fprintf(output_file, "[ERRO] Tipo desconhecido.\n");       
                break;           
       }
    }

    n = symbol_table_string.n_strings;
    for (i = 0; i < n; i++) {
        fprintf(output_file, "%s: db %s\n",
            symbol_table_string.string[i].name, 
            symbol_table_string.string[i].value);
    }
}

/**
 * @brief Funcao que gera a marcacao do inicio da secao de codigo
 * 
 */
void gen_preambule_code(void) {
    fprintf(output_file, "\n\tsection .text\n");
    fprintf(output_file, "\tglobal main,_start\n");
    fprintf(output_file, "main:\n");
    fprintf(output_file, "_start:\n");
}

/**
 * @brief Funcao que encerra o codigo inserindo comandos de fechamento
 * 
 */
void gen_epilog_code(void) {
    //fprintf(output_file, "\nret\n");
    fprintf(output_file, "\n;encerra programa\n");
    fprintf(output_file, "mov ebx,0\n");
    fprintf(output_file, "mov eax,1\n");
    fprintf(output_file, "int 0x80\n");
}

/**
 * @brief Funcao que gera automaticamente um novo nome para um label
 * @param string name
 */
void gen_label_name(char *name ) {
    static int nlabels = 0;
    char conv_value[16];
    sprintf(conv_value, "%d", nlabels);
    strcpy(name, "label");
    strcat(name, conv_value);
    nlabels++;
}

/**
 * @brief Funcao que registra no codigo um label
 * @param string label 
 */
void gen_label( char *label ) {
    fprintf(output_file, "%s:\n", label);
}

/**
 * @brief Funcao que gera um jump condicional
 * @param string label
 */
void gen_cond_jump(char *label) {
    fprintf(output_file, ";jump condicional\n");
    fprintf(output_file, "pop rax\n");
    fprintf(output_file, "cmp rax, 0\n");
    fprintf(output_file, "jz %s\n", label);
}

/**
 * @brief Funcao que gera um jump incondicional
 * @param string label
 */
void gen_incond_jump(char *label) {
    fprintf(output_file, ";jump incondicional\n");
    fprintf(output_file, "jmp %s\n", label);
}

/**
 * @brief Funcao que gera codigo a partir do processamento da expressao logica
 * 
 * @param oper 
 */
void gen_bool(int oper) {
    static int bool_label;
    char bool_label_name[MAX_CHAR];
    fprintf(output_file,";Aplica operador booleano/exp.logica\n");
    fprintf(output_file,"pop rbx\n");
    fprintf(output_file,"pop rax\n");
    gen_bool_label_name(bool_label_name);
    fprintf(output_file,"mov rcx,1\n");
    fprintf(output_file,"cmp eax,ebx\n");
    switch (oper) {
        case EQUAL:
            fprintf(output_file,"je %s\n", bool_label_name);
            break;
        case NE:
            fprintf(output_file,"jne %s\n",bool_label_name);
            break;
        case LT:
            fprintf(output_file,"jl %s\n", bool_label_name);
            break;
        case GT:
            fprintf(output_file,"jg %s\n", bool_label_name);
            break;
        case LE:
            fprintf(output_file,"jle %s\n", bool_label_name);
            break;
        case GE:
            fprintf(output_file,"jge %s\n", bool_label_name);
            break;
        default:
            printf("[ERRO] operador booleano nao suportado.\n");
            break;
    }
    fprintf(output_file, "mov rcx,0\n");
    gen_label(bool_label_name);
    fprintf(output_file, "mov rax, rcx\n");
    fprintf(output_file, "push rax\n");    
}

void gen_bool_label_name(char *name) {
    static int nbool_labels = 0;
    char conv_value[16];
    sprintf(conv_value, "%d", nbool_labels);
    strcpy(name, "bool_label");
    strcat(name, conv_value);
    nbool_labels++;
}

/**
 * @brief Gera codigo para o comando READ
 * 
 * @param lexeme_of_id nome do identificador
 */
void gen_read(char *lexeme_of_id, int type) {
    switch (type) {
        case INT:
            fprintf(output_file, "\n; --- le valor inteiro ---\n");
            // 1. Lê do teclado para o buffer
            fprintf(output_file, "mov edx, 2\n"); // Lê 1 dígito + a tecla ENTER
            fprintf(output_file, "mov ecx, buffer_io\n"); // Guarda no rascunho
            fprintf(output_file, "mov ebx, 0\n"); // 0 = Teclado
            fprintf(output_file, "mov eax, 3\n"); // sys_read
            fprintf(output_file, "int 0x80\n");

            // 2. Converte ASCII para Número Real
            fprintf(output_file, "movzx eax, byte [buffer_io]\n"); // Pega o caractere digitado
            fprintf(output_file, "sub eax, 48\n"); // Transforma '5' (53) em 5
            fprintf(output_file, "mov dword [%s], eax\n", lexeme_of_id); // Salva na variável real
            break;
        case FLOAT:
            fprintf(output_file, "\n;le valor float\n");
            fprintf(output_file, "mov edx,16\n");
            fprintf(output_file, "mov ecx,%s\n", lexeme_of_id);
            fprintf(output_file, "mov ebx,0\n");
            fprintf(output_file, "mov eax,3\n");
            fprintf(output_file, "int 0x80\n");
            break;
        case CHAR:
            fprintf(output_file, "\n;le valor char\n");
            fprintf(output_file, "mov edx,1\n");
            fprintf(output_file, "mov ecx,%s\n", lexeme_of_id);
            fprintf(output_file, "mov ebx,0\n");
            fprintf(output_file, "mov eax,3\n");
            fprintf(output_file, "int 0x80\n");
            break;
        case STRING:
            fprintf(output_file, "\n;le valor string\n");
            fprintf(output_file, "mov edx,16\n");
            fprintf(output_file, "mov ecx,%s\n", lexeme_of_id);
            fprintf(output_file, "mov ebx,0\n");
            fprintf(output_file, "mov eax,3\n");
            fprintf(output_file, "int 0x80\n");
            break;
    }
}

/**
 * @brief Gera codigo para o comando WRITE
 * 
 * @param lexeme_of_id nome do identificador
 */
void gen_write(char *lexeme_of_id, int type) {
    switch (type) {
        case INT:
            fprintf(output_file, "\n; --- escreve valor inteiro ---\n");
            // 1. Converte Número Real para ASCII
            fprintf(output_file, "mov eax, dword [%s]\n", lexeme_of_id); // Pega o número matemático
            fprintf(output_file, "add eax, 48\n"); // Transforma 5 em '5' (53)
            fprintf(output_file, "mov byte [buffer_io], al\n"); // Salva o texto no rascunho

            // 2. Imprime na tela
            fprintf(output_file, "mov edx, 1\n"); // Imprime apenas 1 caractere
            fprintf(output_file, "mov ecx, buffer_io\n"); // Imprime o que está no rascunho
            fprintf(output_file, "mov ebx, 1\n"); // 1 = Tela
            fprintf(output_file, "mov eax, 4\n"); // sys_write
            fprintf(output_file, "int 0x80\n");
            break;
        case FLOAT:
            fprintf(output_file, "\n;escreve valor float\n");
            fprintf(output_file, "mov edx,16\n");
            fprintf(output_file, "mov ecx,%s\n", lexeme_of_id);
            fprintf(output_file, "mov ebx,1\n");
            fprintf(output_file, "mov eax,4\n");
            fprintf(output_file, "int 0x80\n");
            break;
        case CHAR:
            fprintf(output_file, "\n;escreve valor char\n");
            fprintf(output_file, "mov edx,1\n");
            fprintf(output_file, "mov ecx,%s\n", lexeme_of_id);
            fprintf(output_file, "mov ebx,1\n");
            fprintf(output_file, "mov eax,4\n");
            fprintf(output_file, "int 0x80\n");
            break;
        case STRING:
            fprintf(output_file, "\n;escreve valor string\n");
            fprintf(output_file, "mov edx,16\n");
            fprintf(output_file, "mov ecx,%s\n", lexeme_of_id);
            fprintf(output_file, "mov ebx,1\n");
            fprintf(output_file, "mov eax,4\n");
            fprintf(output_file, "int 0x80\n");
            break;
    }
}

// funcoes mips
/**
 * @brief Passa argumento para chamada de funcao ($a0...$a3)
 * @param arg_index indice do argumento (0 a 3)
 */
void gen_func_arg(int arg_index) {
    const char *arg_regs[] = {"r8", "r9", "r10", "r11"}; // $a0 = r8, $a1 = r9, $a2 = r10, $a3 = r11
    if (arg_index < 0 || arg_index >= MAX_PARAMS) {
        printf("[ERRO] Indice de argumento invalido: %d\n", arg_index);
        return;
    }
    fprintf(output_file, ";Passa argumento %d para chamada de funcao\n", arg_index+1);
    fprintf(output_file, "pop rax\n");
    fprintf(output_file, "mov %s, rax\n", arg_regs[arg_index]); // $a0 = r8, $a1 = r9, $a2 = r10, $a3 = r11
}

/**
 * @brief Gera chamada de funcao (jal) e captura valor de retorno
 * @param label label da funcao a ser chamada
 * @param result_var nome da variavel onde o valor de retorno (pode ser NULL)
 */

void gen_call(char *label, char *result_var) {
    fprintf(output_file, ";Chamada de funcao\n");
    fprintf(output_file, "call %s\n", label);

    if (result_var != NULL) {
        type_symbol_table_entry *sym = sym_find_any(result_var);
        
        if (sym != NULL) {
            fprintf(output_file, ";Captura valor de retorno (RAX) para '%s'\n", result_var);
            
            if (sym->is_global) {
                // Se for global, usa o nome diretamente
                fprintf(output_file, "mov dword [%s], eax\n", sym->name);
            } else {
                // REQUISITO 1.1: Se for local, usa o deslocamento do Frame Pointer
                fprintf(output_file, "mov dword [rbp - %d], eax\n", sym->addr);
            }
        } else {
            // REQUISITO 3: Verificação de variável não declarada
            printf("[ERRO] Variavel de destino '%s' nao declarada.\n", result_var);
        }
    }
}

/**
 * @brief prologo padrao de funcao (empilha $ra, $fp, ajusta $sp)
 * @param func_label label da funcao
 */
void gen_func_prolog(char *func_label){
    fprintf(output_file, "\n;Prologo de funcao %s\n", func_label);
    fprintf(output_file, "%s:\n", func_label);
    fprintf(output_file, "push rbp\n"); // Salva o frame pointer anterior (sw $fp)
    fprintf(output_file, "mov rbp, rsp\n"); // Estabelece o novo (move $fp, $sp)
    fprintf(output_file, "sub rsp, 64\n"); // Reserva espaço para variáveis locais (addi $sp, $sp, -N)
}

/**
 * @brief epilogo padrao de funcao (restaura $ra $fp, jr $ra)
 * 
 */
void gen_func_epilog(void) {
    fprintf(output_file, "\n;Epilogo da funcao\n");
    fprintf(output_file, "mov rsp, rbp\n"); // Restaura o stack pointer (move $sp, $fp)
    fprintf(output_file, "pop rbp\n"); // Restaura o frame pointer anterior (lw $fp, 0($sp))
    fprintf(output_file, "ret\n"); // Retorna para o endereço salvo no stack (jr $ra)
}

/**
 * @brief Gera comando return (move topo pilha para rax ($v0))
 */
void gen_return(void) {
    fprintf(output_file, ";Comando return\n");
    fprintf(output_file, "pop rax\n"); // Move o valor de retorno para rax ($v0)
}

/**
 * @brief Regra de derivação para implementação de funções
 */
void gen_id_value(char *id_name) {
    type_symbol_table_entry *sym = sym_find_any(id_name);
    if (sym == NULL) {
        printf("[ERRO] Variavel '%s' nao declarada.\n", id_name);
        return;
    }
    if (sym->is_global) {
        fprintf(output_file, ";Carrega valor de variavel global\n");
        fprintf(output_file, "mov eax, dword [%s]\n", id_name);
    } else {
        // REQUISITO 1.1: Se for local, usa o deslocamneto do Frame Poiter (RBP)
        fprintf(output_file, ";Carrega valor de variavel local\n");
        fprintf(output_file, "mov eax, dword [rbp - %d]\n", sym->addr);
    }
    fprintf(output_file, "push rax\n");
}