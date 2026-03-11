/**
 * @file gen.c
 * @author Anna Bheatryz Martins dos Santos e Mariana Sanchez Pedroni
 * @brief Codificacao do modulo gerador de codigo - saida MIPS
 * @version 0.3
 * @date 2022-02-23
 *
 * Convencoes MIPS adotadas:
 *   $t0-$t9  : registradores temporarios
 *   $a0-$a3  : argumentos de funcao
 *   $v0      : valor de retorno
 *   $fp      : frame pointer
 *   $ra      : endereco de retorno
 *   $sp      : stack pointer
 *   $zero    : constante zero
 */

#include "gen.h"

extern type_symbol_table_variables global_symbol_table_variables;
extern type_symbol_table_string symbol_table_string;
char output_file_name[MAX_CHAR];
FILE *output_file;

/* ---------------------------------------------------------------
 * Helpers internos: push/pop da pilha de expressoes via $sp
 * --------------------------------------------------------------- */

static void mips_push(const char *reg) {
    fprintf(output_file, "addi $sp, $sp, -4\n");
    fprintf(output_file, "sw   %s, 0($sp)\n", reg);
}

static void mips_pop(const char *reg) {
    fprintf(output_file, "lw   %s, 0($sp)\n", reg);
    fprintf(output_file, "addi $sp, $sp, 4\n");
}

/* ---------------------------------------------------------------
 * Aritmetica
 * --------------------------------------------------------------- */

void genAdd() {
    fprintf(output_file, "# Adicao\n");
    mips_pop("$t1");
    mips_pop("$t0");
    fprintf(output_file, "add  $t0, $t0, $t1\n");
    mips_push("$t0");
}

void genSub() {
    fprintf(output_file, "# Subtracao\n");
    mips_pop("$t1");
    mips_pop("$t0");
    fprintf(output_file, "sub  $t0, $t0, $t1\n");
    mips_push("$t0");
}

void genMult() {
    fprintf(output_file, "# Multiplicacao\n");
    mips_pop("$t1");
    mips_pop("$t0");
    fprintf(output_file, "mul  $t0, $t0, $t1\n");
    mips_push("$t0");
}

void genDiv() {
    fprintf(output_file, "# Divisao\n");
    mips_pop("$t1");
    mips_pop("$t0");
    fprintf(output_file, "div  $t0, $t1\n");
    fprintf(output_file, "mflo $t0\n");
    mips_push("$t0");
}

void genNum(char num_string[MAX_TOKEN]) {
    fprintf(output_file, "# Numero imediato %s\n", num_string);
    fprintf(output_file, "li   $t0, %s\n", num_string);
    mips_push("$t0");
}

/* ---------------------------------------------------------------
 * Atribuicao
 * --------------------------------------------------------------- */

void gen_assign(char *var_name) {
    type_symbol_table_entry *sym = sym_find_any(var_name);
    if (sym == NULL) {
        printf("[ERRO] Variavel '%s' nao declarada.\n", var_name);
        return;
    }
    fprintf(output_file, "# Atribuicao -> '%s'\n", var_name);
    mips_pop("$t0");
    if (sym->is_global) {
        fprintf(output_file, "la   $t1, %s\nsw   $t0, 0($t1)\n", sym->name);
    } else {
        fprintf(output_file, "sw   $t0, -%d($fp)\n", sym->addr);
    }
}

/* ---------------------------------------------------------------
 * Estrutura do programa
 * --------------------------------------------------------------- */

void gen_preambule(void) {
    fprintf(output_file, "# UFMT - Compiladores\n");
    fprintf(output_file, "# Prof. Ivairton\n");
    fprintf(output_file, "# Montagem: $ spim -file <arquivo>.asm\n");
    fprintf(output_file, "#        ou: $ mars <arquivo>.asm\n\n");
    /* Nao abre .data aqui - sera feito por gen_data_section antes do .text */
}

void gen_data_section(void) {
    int i, n;

    /* Secao .data vem ANTES do .text, com todas as variaveis globais.
       As strings literais (descobertas durante parsing) serao emitidas
       em uma segunda diretiva .data ao final do arquivo - o SPIM/MARS
       aceita multiplas secoes .data. */
    fprintf(output_file, ".data\n");
    fprintf(output_file, "newline: .asciiz \"\\n\"\n");

    n = global_symbol_table_variables.n_variables;
    for (i = 0; i < n; i++) {
        char *vname = global_symbol_table_variables.variable[i].name;
        switch (global_symbol_table_variables.variable[i].type) {
            case INT:    fprintf(output_file, "%s: .word 0\n",    vname); break;
            case FLOAT:  fprintf(output_file, "%s: .float 0.0\n", vname); break;
            case CHAR:   fprintf(output_file, "%s: .byte 0\n",    vname); break;
            case STRING: fprintf(output_file, "%s: .space 256\n", vname); break;
            default:     fprintf(output_file, "# [ERRO] Tipo desconhecido: '%s'\n", vname); break;
        }
    }
}

/* Emite strings literais em secao .data separada ao final do arquivo.
   Chamada apos func_code() em program(), quando todas as strings ja foram coletadas. */
void gen_string_section(void) {
    int i, n;
    n = symbol_table_string.n_strings;
    if (n == 0) return;
    fprintf(output_file, "\n# Strings literais\n");
    fprintf(output_file, ".data\n");
    for (i = 0; i < n; i++) {
        fprintf(output_file, "%s: .asciiz %s\n",
            symbol_table_string.string[i].name,
            symbol_table_string.string[i].value);
    }
}

void gen_preambule_code(void) {
    fprintf(output_file, "\n.text\n");
    fprintf(output_file, ".globl main\n");
    fprintf(output_file, "main:\n");
}

void gen_epilog_code(void) {
    fprintf(output_file, "\n# Encerra programa (syscall exit)\n");
    fprintf(output_file, "li   $v0, 10\n");
    fprintf(output_file, "syscall\n");
}

/* ---------------------------------------------------------------
 * Labels e saltos
 * --------------------------------------------------------------- */

void gen_label_name(char *name) {
    static int nlabels = 0;
    sprintf(name, "label%d", nlabels++);
}

void gen_label(char *label) {
    fprintf(output_file, "%s:\n", label);
}

void gen_cond_jump(char *label) {
    fprintf(output_file, "# Salto condicional\n");
    mips_pop("$t0");
    fprintf(output_file, "beq  $t0, $zero, %s\n", label);
}

void gen_incond_jump(char *label) {
    fprintf(output_file, "# Salto incondicional\n");
    fprintf(output_file, "j    %s\n", label);
}

/* ---------------------------------------------------------------
 * Expressao booleana
 * --------------------------------------------------------------- */

void gen_bool_label_name(char *name) {
    static int nbool_labels = 0;
    sprintf(name, "bool_label%d", nbool_labels++);
}

void gen_bool(int oper) {
    char label_true[MAX_CHAR];
    char label_end[MAX_CHAR];
    static int bool_end_count = 0;

    gen_bool_label_name(label_true);
    sprintf(label_end, "bool_end%d", bool_end_count++);

    fprintf(output_file, "# Expressao booleana\n");
    mips_pop("$t1");   /* operando direito */
    mips_pop("$t0");   /* operando esquerdo */

    switch (oper) {
        case EQUAL:
            fprintf(output_file, "beq  $t0, $t1, %s\n", label_true);
            break;
        case NE:
            fprintf(output_file, "bne  $t0, $t1, %s\n", label_true);
            break;
        case LT:
            fprintf(output_file, "slt  $t2, $t0, $t1\n");
            fprintf(output_file, "bne  $t2, $zero, %s\n", label_true);
            break;
        case GT:
            fprintf(output_file, "slt  $t2, $t1, $t0\n");
            fprintf(output_file, "bne  $t2, $zero, %s\n", label_true);
            break;
        case LE:
            fprintf(output_file, "slt  $t2, $t1, $t0\n");
            fprintf(output_file, "beq  $t2, $zero, %s\n", label_true);
            break;
        case GE:
            fprintf(output_file, "slt  $t2, $t0, $t1\n");
            fprintf(output_file, "beq  $t2, $zero, %s\n", label_true);
            break;
        default:
            printf("[ERRO] Operador booleano nao suportado.\n");
            break;
    }

    fprintf(output_file, "li   $t0, 0\n");
    fprintf(output_file, "j    %s\n", label_end);
    fprintf(output_file, "%s:\n", label_true);
    fprintf(output_file, "li   $t0, 1\n");
    fprintf(output_file, "%s:\n", label_end);
    mips_push("$t0");
}

/* ---------------------------------------------------------------
 * Entrada e saida (syscalls MIPS/SPIM)
 *   1  = print_int      5  = read_int
 *   2  = print_float    6  = read_float
 *   4  = print_string   8  = read_string
 *   11 = print_char     12 = read_char
 * --------------------------------------------------------------- */

void gen_read(char *lexeme_of_id, int type) {
    type_symbol_table_entry *sym = sym_find_any(lexeme_of_id);
    switch (type) {
        case INT:
            fprintf(output_file, "\n# Le inteiro -> '%s'\n", lexeme_of_id);
            fprintf(output_file, "li   $v0, 5\n");
            fprintf(output_file, "syscall\n");
            if (sym != NULL && !sym->is_global)
                fprintf(output_file, "sw   $v0, -%d($fp)\n", sym->addr);
            else
                fprintf(output_file, "la   $t1, %s\nsw   $v0, 0($t1)\n", lexeme_of_id);
            break;
        case FLOAT:
            fprintf(output_file, "\n# Le float -> '%s'\n", lexeme_of_id);
            fprintf(output_file, "li   $v0, 6\n");
            fprintf(output_file, "syscall\n");
            if (sym != NULL && !sym->is_global)
                fprintf(output_file, "swc1 $f0, -%d($fp)\n", sym->addr);
            else
                fprintf(output_file, "swc1 $f0, %s\n", lexeme_of_id);
            break;
        case CHAR:
            fprintf(output_file, "\n# Le char -> '%s'\n", lexeme_of_id);
            fprintf(output_file, "li   $v0, 12\n");
            fprintf(output_file, "syscall\n");
            if (sym != NULL && !sym->is_global)
                fprintf(output_file, "sb   $v0, -%d($fp)\n", sym->addr);
            else
                fprintf(output_file, "sb   $v0, %s\n", lexeme_of_id);
            break;
        case STRING:
            fprintf(output_file, "\n# Le string -> '%s'\n", lexeme_of_id);
            fprintf(output_file, "li   $v0, 8\n");
            if (sym != NULL && !sym->is_global)
                fprintf(output_file, "addi $a0, $fp, -%d\n", sym->addr);
            else
                fprintf(output_file, "la   $a0, %s\n", lexeme_of_id);
            fprintf(output_file, "li   $a1, 256\n");
            fprintf(output_file, "syscall\n");
            break;
    }
}

void gen_write(char *lexeme_of_id, int type) {
    type_symbol_table_entry *sym = sym_find_any(lexeme_of_id);
    switch (type) {
        case INT:
            fprintf(output_file, "\n# Escreve inteiro '%s'\n", lexeme_of_id);
            if (sym != NULL && !sym->is_global)
                fprintf(output_file, "lw   $a0, -%d($fp)\n", sym->addr);
            else
                fprintf(output_file, "la   $t1, %s\nlw   $a0, 0($t1)\n", lexeme_of_id);
            fprintf(output_file, "li   $v0, 1\n");
            fprintf(output_file, "syscall\n");
            break;
        case FLOAT:
            fprintf(output_file, "\n# Escreve float '%s'\n", lexeme_of_id);
            if (sym != NULL && !sym->is_global)
                fprintf(output_file, "lwc1 $f12, -%d($fp)\n", sym->addr);
            else
                fprintf(output_file, "lwc1 $f12, %s\n", lexeme_of_id);
            fprintf(output_file, "li   $v0, 2\n");
            fprintf(output_file, "syscall\n");
            break;
        case CHAR:
            fprintf(output_file, "\n# Escreve char '%s'\n", lexeme_of_id);
            if (sym != NULL && !sym->is_global)
                fprintf(output_file, "lb   $a0, -%d($fp)\n", sym->addr);
            else
                fprintf(output_file, "lb   $a0, %s\n", lexeme_of_id);
            fprintf(output_file, "li   $v0, 11\n");
            fprintf(output_file, "syscall\n");
            break;
        case STRING:
            fprintf(output_file, "\n# Escreve string '%s'\n", lexeme_of_id);
            fprintf(output_file, "la   $a0, %s\n", lexeme_of_id);
            fprintf(output_file, "li   $v0, 4\n");
            fprintf(output_file, "syscall\n");
            break;
    }
}

/* ---------------------------------------------------------------
 * Funcoes MIPS
 * --------------------------------------------------------------- */

void gen_func_arg(int arg_index) {
    const char *arg_regs[] = {"$a0", "$a1", "$a2", "$a3"};
    if (arg_index < 0 || arg_index >= MAX_PARAMS) {
        printf("[ERRO] Indice de argumento invalido: %d\n", arg_index);
        return;
    }
    fprintf(output_file, "# Argumento %d\n", arg_index + 1);
    mips_pop("$t0");
    fprintf(output_file, "move %s, $t0\n", arg_regs[arg_index]);
}

void gen_call(char *label, char *result_var) {
    fprintf(output_file, "# Chamada: jal %s\n", label);
    fprintf(output_file, "jal  %s\n", label);

    if (result_var != NULL) {
        type_symbol_table_entry *sym = sym_find_any(result_var);
        if (sym != NULL) {
            fprintf(output_file, "# Captura retorno ($v0) -> '%s'\n", result_var);
            if (sym->is_global)
                fprintf(output_file, "la   $t1, %s\nsw   $v0, 0($t1)\n", sym->name);
            else
                fprintf(output_file, "sw   $v0, -%d($fp)\n", sym->addr);
        } else {
            printf("[ERRO] Variavel de destino '%s' nao declarada.\n", result_var);
        }
    }
}

void gen_func_prolog(char *func_label) {
    fprintf(output_file, "\n# Prologo de %s\n", func_label);
    fprintf(output_file, "%s:\n", func_label);
    /* salva $ra e $fp */
    fprintf(output_file, "addi $sp, $sp, -8\n");
    fprintf(output_file, "sw   $ra, 4($sp)\n");
    fprintf(output_file, "sw   $fp, 0($sp)\n");
    fprintf(output_file, "move $fp, $sp\n");
    /* espaco para variaveis locais */
    fprintf(output_file, "addi $sp, $sp, -64\n");
}

void gen_func_epilog(void) {
    fprintf(output_file, "\n# Epilogo da funcao\n");
    fprintf(output_file, "move $sp, $fp\n");
    fprintf(output_file, "lw   $fp, 0($sp)\n");
    fprintf(output_file, "lw   $ra, 4($sp)\n");
    fprintf(output_file, "addi $sp, $sp, 8\n");
    fprintf(output_file, "jr   $ra\n");
}

void gen_return(void) {
    fprintf(output_file, "# return -> $v0\n");
    mips_pop("$v0");
}

void gen_id_value(char *id_name) {
    type_symbol_table_entry *sym = sym_find_any(id_name);
    if (sym == NULL) {
        printf("[ERRO] Variavel '%s' nao declarada.\n", id_name);
        return;
    }
    if (sym->is_global) {
        fprintf(output_file, "# Carrega global '%s'\n", id_name);
        fprintf(output_file, "la   $t1, %s\nlw   $t0, 0($t1)\n", id_name);
    } else {
        fprintf(output_file, "# Carrega local '%s'\n", id_name);
        fprintf(output_file, "lw   $t0, -%d($fp)\n", sym->addr);
    }
    mips_push("$t0");
}