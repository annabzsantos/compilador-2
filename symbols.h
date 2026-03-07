/**
 * @file symbols.h
 * @author Anna Bheatryz Martins dos Santos e Mariana Sanchez Pedroni
 * @brief Modulo do gerador da tabela de simbolos
 * @version 0.1
 * @date 2022-02-04
 */

#ifndef _SYMBOLS_H_
#define _SYMBOLS_H_

#include <stdio.h>
#include <string.h>

#include "struct_compiler.h"

#define MAX_SYMBOLS 4096
#define MAX_CHAR 256
#define MAX_PARAMS 8
#define MAX_FUNCS 64
#define MAX_STRINGSZ 512
#define MAX_STRINGS 64
#define MAX_TOKSZ 256 

// Entrada da tabela de simbolos de variaveis
// TSG / TSL (Tabela de Simbolos de Variaveis / Tabela de Simbolos Local)
struct st_symbol_table_entry {
    int type;
    char name[MAX_TOKSZ];
    int addr;
};
typedef struct st_symbol_table_entry type_symbol_table_entry;

// Tabela de simbolos de variaveis
struct st_symbol_table_variables {
    type_symbol_table_entry variable[MAX_SYMBOLS];
    int n_variables;
    struct st_symbol_table_variables *next;
};
typedef struct st_symbol_table_variables type_symbol_table_variables;

// Entrada da tabela de simbolos de strings
struct st_sym_string_entry {
    char name[MAX_TOKSZ];
    char value[MAX_STRINGSZ];
};
typedef struct st_sym_string_entry type_symbol_table_string_entry;

// tabela de simbolos de strings
struct st_symbol_table_strings {
    type_symbol_table_string_entry string[MAX_SYMBOLS];
    int n_strings;
};
typedef struct st_symbol_table_strings type_symbol_table_string;

// Parametro de funcao
struct st_param {
    char name[MAX_TOKSZ];
    int type;
};
typedef struct st_param type_param;

// Estrutura da tabela de simbolos para funcoes
struct st_sym_func {
    char name[MAX_TOKSZ];
    int return_type;
    type_param params[MAX_PARAMS];
    int nparams;
    char label[MAX_TOKSZ];

    /**ponteiro para a TSL dessa funcao
     * NULL enquanto a funcao apenas foi prototipada/declarada
     * apontara para a TSL alocada quando o corpo processado
     * em chamadas recursivas , uma nova TSL e alocada por chamada
     */
    type_symbol_table_variables *local_table;

    // flag para indicar se a funcao foi apenas prototipada/declarada ou se o corpo ja foi processado
    // 1 = implementada, 0 = apenas prototipada/declarada
    int implemented;

};
typedef struct st_sym_func type_symbol_function;


extern type_symbol_table_variables global_symbol_table_variables; // TSG
extern type_symbol_table_string symbol_table_string; // Tabela de simbolos de strings

// Variaveis globais para TSF
extern type_symbol_function symfuncs[MAX_FUNCS];
extern int symfuncspos;

// ponteiro para a TSL ativa (NULL quando esta no escopo global)
extern type_symbol_table_variables *current_local_table;

// Prototipos
//busca: primeiro na TSL ativa (se houver), depois na TSG
type_symbol_table_entry *sym_find_any(char *s);

//busca apenas na tabela informada
type_symbol_table_entry *sym_find(char *s, type_symbol_table_variables *stv);

//declara variavel na tabela informada
type_symbol_table_entry *sym_declare(char *name, int type, int addr, type_symbol_table_variables *stv);

// strings
type_symbol_table_string_entry *sym_string_find(char *s);
type_symbol_table_string_entry *sym_string_declare(char *s);

// funcoes
type_symbol_function *sym_func_find(char *s);
type_symbol_function *sym_func_declare(char *name, int return_type, type_param *params, int nparams);

// cria e retorna uma nova TSL alocada dinamicamente (suporta recursao)
type_symbol_table_variables *sym_create_local_table(void);

// libera TSL
void sym_free_local_table(type_symbol_table_variables *tsl);

// inicializacao
void initSymbolTableVariables(type_symbol_table_variables *stv);
void initSymbolTableString(void);
void initSymbolTableFunctions(void);

// verificacao semantica final: funcoes declaradas mas nao implementadas
void sem_check_unimplemented_functions(void);

// impressao (debug)
void printSTVariables(type_symbol_table_variables *stv);
void printSTString(void);
void printSTFunctions(void);

#endif //_SYMBOLS_H_