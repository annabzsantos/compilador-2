/**
 * @file gen.h
 * @author Anna Bheatryz Martins dos Santos e Mariana Sanchez Pedroni
 * @brief Modulo do gerador de codigo
 * @version 0.3
 * @date 2022-02-04
 * 
 */
#ifndef _GEN_H_
#define _GEN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "struct_compiler.h"
#include "symbols.h"

// Prototipos
// aritmetica
void genAdd();
void genSub();
void genMult();
void genDiv();
void genNum(char num_string[MAX_TOKEN]);

// atribuicao
void gen_assign(char*);

// estrutura do programa
void gen_preambule(void);
void gen_data_section(void);
void gen_preambule_code(void);
void gen_epilog_code(void);
void gen_string_section(void);

// labels e saltos
void gen_label_name(char*);
void gen_label(char*);
void gen_cond_jump(char*);
void gen_incond_jump(char*);

// entrada e saida
void gen_read(char*, int);
void gen_write(char*, int);

// expressao booleana
void gen_bool(int);
void gen_bool_label_name(char*);

// funcoes mips

void gen_func_arg(int); // passa argumento para chamada de funcao ($a0...$a3)
void gen_call(char*, char*); // gera chamada de funcao (jal) e captura valor de retorno
void gen_func_prolog(char*); // prologo padrao de funcao (empilha $ra, $fp, ajusta $sp)
void gen_func_epilog(void); // restaura $ra $fp, jr $ra
void gen_return(void);  // gera comando return (move topo pilha para $v0)
void gen_id_value(char*);

#endif //_GEN_H_