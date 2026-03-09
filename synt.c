/**
 * @file synt.c
 * @author Anna Bheatryz Martins dos Santos e Mariana Sanchez Pedroni
 * @brief Codificacao do modulo do analisador sintatico
 * @version 0.4
 * @date 2022-02-04
 * 
 */

// Inclusao do cabecalho
#include "synt.h"
#include <stdbool.h>

// Variaveis globais
type_token *lookahead;
extern type_symbol_table_variables global_symbol_table_variables;
extern type_symbol_table_string symbol_table_string;
extern char output_file_name[MAX_CHAR];
extern FILE *output_file;

/**
 * @brief Verifica se o proximo caracter (a frente) na cadeia eh o esperado
 * 
 * @param token_tag (int) codigo do token a ser verificado 
 * @return int true/false
 */
int match(int token_tag) {
    if ( lookahead->tag == token_tag ) {
        lookahead = getToken(); //Pega o proximo token por meio do lexico
        return true;
    }
    printf("[ERRO] Entrada esperada: %s\n", lookahead->lexema);
    return false;
}

/**
 * @brief Regra de derivacao inicial
 */
void program (void) {
    gen_preambule(); 
    gen_preambule_code(); 
    declarations(); 
    
    match(BEGIN);   
    statements();   
    match(END);     
    
    gen_epilog_code();  // Encerra o programa
    func_code(); // Gera o codigo das funcoes
    gen_data_section(); // Gera a secao de dados (variaveis globais e strings)
    printSTFunctions();
    sem_check_unimplemented_functions();

}

/**
 * @brief Regra de derivacao para declaracoes
 */
void declarations(void) {
    while ( declaration() ); //Laco para processamento continuo das declaracoes
}

/**
 * @brief Regra de derivacao declaracao
 * @return int true/false
 */
int declaration(void) {
    int type = lookahead->tag;
    if (type == INT || type == FLOAT || type == CHAR || type == STRING) {
        int return_type = type;
        match(type);
        char name[MAX_CHAR];
        strcpy(name, lookahead->lexema);
        match(ID);
        if (lookahead->tag == SEMICOLON) {
            // Declaração de variável
            if (sym_find(name, &global_symbol_table_variables) != NULL) {
                printf("[ERRO] Variavel '%s' ja declarada.\n", name);
                return false;
            }
            sym_declare(name, type, 0, &global_symbol_table_variables);
            return match(SEMICOLON);
        } else if (lookahead->tag == OPEN_PAR) {
            // Declaração de função (protótipo)
            if (sym_func_find(name) != NULL) {
                printf("[ERRO] Funcao '%s' ja declarada.\n", name);
                return false;
            }
            match(OPEN_PAR);
            type_param params[MAX_PARAMS];
            int nparams = 0;
            bool has_param = (lookahead->tag != CLOSE_PAR);
            while (has_param) {
                int param_type = lookahead->tag;
                if (param_type != INT && param_type != FLOAT && param_type != CHAR && param_type != STRING) {
                    printf("[ERRO] Tipo de parametro invalido.\n");
                    return false;
                }
                match(param_type);
                char param_name[MAX_CHAR];
                strcpy(param_name, lookahead->lexema);
                match(ID);
                // parametros nao sao declarados na TSG 
                // serao registrados na TSL quando o corpo for processado
                params[nparams].type = param_type;
                strcpy(params[nparams].name, param_name);
                nparams++;
                if (lookahead->tag == COMMA) {
                    match(COMMA);
                } else {                    
                    has_param = false;
                }
            }
            match (CLOSE_PAR);
            match(SEMICOLON);
            // Declara a função na TSF (implemented=0, local_table=NULL, setados em sym_func_declare)
            sym_func_declare(name, return_type, params, nparams);
            return true;
        } else {
            printf("[ERRO] Esperado ';' ou '(' apos ID.\n");
            return false;
        }
    } else {
        return false;
    }
}

/**
 * @brief Regra de derivacao para comandos
 */
void statements (void) {
   while ( statement() );  //processa enquanto houver comandos
}

/**
 * @brief Regra de derivacao que processa os comandos
 * 
 * @return int true/false
 */
int statement (void) {
    char var_destino[MAX_CHAR];
    type_symbol_table_entry *search_symbol;
    type_symbol_table_string_entry *gen_string;
    int ok1, ok2, type;
    char string_value[MAX_CHAR];

    if (lookahead->tag == READ) {
        match(READ);
        strcpy(var_destino, lookahead->lexema);
        ok1 = match(ID);
        search_symbol = sym_find_any(var_destino); // busca TSL depois TSG
        if (search_symbol != NULL) {
            type = search_symbol->type;
            gen_read(var_destino, type);
            ok2 = match(SEMICOLON);
            return ok1 && ok2;
        } else {
            printf("[ERRO] Simbolo desconhecido (Variavel nao declarada): %s\n", var_destino);
            return false;
        }
    } else if (lookahead->tag == WRITE) {
        match(WRITE);

        if (lookahead->tag == STRING) {
            strcpy(string_value, lookahead->lexema);
            gen_string = sym_string_declare(string_value);
            match(STRING);

            if (gen_string != NULL) {
                strcpy(var_destino, gen_string->name);
                gen_write(var_destino, STRING);
            }

            match(SEMICOLON);
            return true;
        } else if ( lookahead->tag == ID) {
            strcpy(var_destino, lookahead->lexema);
            match(ID);
            search_symbol = sym_find_any(var_destino); // busca TSL depois TSG
            if (search_symbol != NULL) {
                type = search_symbol->type;
                gen_write(var_destino, type);
                match(SEMICOLON);
                return true;
            } else {
                printf("[ERRO] Simbolo desconhecido (Variavel nao declarada): %s\n", var_destino);
                return false;
            }
        }
    } else if (lookahead->tag == IF) {
        char label_else[MAX_CHAR];
        char label_end[MAX_CHAR];
        gen_label_name(label_else);
        gen_label_name(label_end);

        match(IF);
        match(OPEN_PAR);
        B(); //Expressao booleana
        gen_cond_jump(label_else);
        match(CLOSE_PAR);
        
        match(BEGIN);
        statements();
        match(END);

        gen_incond_jump(label_end);
        gen_label(label_else);

        //Verifica se ocorre um ELSE
        if (lookahead->tag == ELSE) {
            match(ELSE);
            match(BEGIN);
            statements();
            match(END);
        }
        gen_label(label_end);
        return true;
    } else if (lookahead->tag == WHILE) {
        char label_begin[MAX_CHAR];
        char label_end[MAX_CHAR];
        gen_label_name(label_begin);
        gen_label_name(label_end);

        match(WHILE);
        match(OPEN_PAR);
        gen_label(label_begin);
        B(); //Expressao booleana
        gen_cond_jump(label_end);
        match(CLOSE_PAR);
        match(BEGIN);
        statements();
        match(END);
        gen_incond_jump(label_begin);
        gen_label(label_end);
        return true;

    } else if (lookahead->tag == ID) {
        char var_destino[MAX_CHAR];
        strcpy(var_destino, lookahead->lexema);
        
        // Busca o ID em ambas as tabelas para resolver ambiguidade
        type_symbol_table_entry *search_symbol = sym_find_any(var_destino); // busca TSL depois TSG
        type_symbol_function *func = sym_func_find(var_destino);
        
        match(ID);
        
        // CASO 1: Atribuição (id = E;)
        if (lookahead->tag == ASSIGN) {
            if (search_symbol == NULL) {
                printf("[ERRO] Variavel nao declarada: %s\n", var_destino);
                return false;
            }
            match(ASSIGN);

            type_symbol_function *func_call = sym_func_find(var_destino);
            if (func_call != NULL) {
                char func_name[MAX_CHAR];
                strcpy(func_name, lookahead->lexema);
                match(ID);
                match(OPEN_PAR);
                int nargs = 0;
                if (lookahead->tag != CLOSE_PAR) {
                    do {
                        E(); // Processa argumento como expressao (pode ser numero, id, expressao aritmetica, chamada de funcao, etc)
                        gen_func_arg(nargs); // Move o argumento para o registrador correspondente ($a0-$a3)
                        nargs++;
                        if (lookahead->tag == COMMA) match(COMMA);
                        else break;
                    } while (true);
                }
                match(CLOSE_PAR);
                match(SEMICOLON);

                gen_call(func_call->label, var_destino); // Gera chamada de funcao e captura valor de retorno
                return true;
            } else {
                E(); // Processa o lado direito da atribuicao como expressao (pode ser numero, id, expressao aritmetica, chamada de funcao, etc)
                gen_assign(var_destino); // Gera codigo de atribuicao
                return match(SEMICOLON);
            }
        }
        
        else if (lookahead->tag == OPEN_PAR) {
            if (func == NULL) {
                printf("[ERRO] Funcao nao declarada ou prototipada: %s\n", var_destino);
                return false;
            }
            match(OPEN_PAR);
            
            int nargs = 0;
            if (lookahead->tag != CLOSE_PAR) {
                do {

                    if (lookahead->tag == ID) {
                        char param_name[MAX_CHAR];
                        strcpy(param_name, lookahead->lexema);
                        
                        // busca argumento na TSL (se houver) e depois na TSG
                        type_symbol_table_entry *var_param = sym_find_any(param_name);
                        
                        if (var_param == NULL) {
                            printf("[ERRO] Variavel parametro nao declarada: %s\n", param_name);
                            return false;
                        }

                        if (nargs < func->nparams) {
                            // Verifica o Tipo (requisito 3)
                            if (var_param->type != func->params[nargs].type) {
                                printf("[ERRO SEMANTICO] Tipo incompativel no parametro %d de '%s'.\n", nargs+1, func->name);
                                return false;
                            }
                        }

                        gen_id_value(param_name); // Empilha o valor do argumento
                        gen_func_arg(nargs); // Move o argumento para o registrador correspondente ($a0-$a3)
                        match(ID);
                    } else{
                        // argumento pode ser expressao (numero, expressao aritmetica, chamada de funcao, etc)
                        E();
                        gen_func_arg(nargs); // Move o argumento para o registrador correspondente ($a0-$a3)
                    }    
                    nargs++;
                    if (lookahead->tag == COMMA) match(COMMA);
                    else break;
                } while (true);
            }
            match(CLOSE_PAR);

            if (nargs != func->nparams) {
                printf("[ERRO] Numero de argumentos incorreto para '%s'. Esperado: %d, Recebido: %d\n", 
                        var_destino, func->nparams, nargs);
                return false;
            }

            gen_call(func->label, NULL); 
            return match(SEMICOLON);
        }
        else {
            printf("[ERRO] Esperado '=' ou '(' apos o identificador '%s'\n", var_destino);
            return false;
        }
    } else if (lookahead->tag == ENDTOKEN) {
        return false;
    } else if (lookahead->tag == END){
        return false;
    } else if (lookahead->tag == RETURN){
        return false; // return sera consumido por func_implementation, que tem regras especificas para processar o comando return dentro do corpo da funcao
    } else {
        printf("[ERRO] Comando desconhecido.\nTag=%d; Lexema=%s\n",lookahead->tag, lookahead->lexema);
        return false;
    }
}

/**
 * @brief processa uma declaracao de variavel local dentro do corpo de funcao (TSL)
 * @return int true/false
 */
int local_declaration(){
    int type = lookahead->tag;
    if (type != INT && type != FLOAT && type != CHAR && type != STRING) {
        return false;
    }
    match(type);
    char name[MAX_CHAR];
    strcpy(name, lookahead->lexema);
    match(ID);
    match(SEMICOLON);
    // declara na TSL ativa (current_local_table)
    sym_declare(name, type, 0, current_local_table);
    return true;
}

/**
 * @brief loop para processar implementacao de funcoes (corpo)
 */
void func_code(void){
    while (func_implementation());
}

/**
 * @brief Regra de derivação para implementação de funções
 * 
 * @return int true/false
 */
int func_implementation(void){
    int type = lookahead->tag;
    if (type != INT && type != FLOAT && type != CHAR && type != STRING) {
        return false;
    }
    int return_type = type;
    match(type);
    char name[MAX_CHAR];
    strcpy(name, lookahead->lexema);
    match(ID);
    type_symbol_function *func = sym_func_find(name);
    if (func == NULL) {
        printf("[ERRO] Funcao nao prototipada: %s\n", name);
        return false;
    }
    if (func->return_type != return_type) {
        printf("[ERRO] Tipo de retorno nao corresponde para funcao %s\n", name);
        return false;
    }
    match(OPEN_PAR);
    type_param temp_params[MAX_PARAMS];
    int temp_nparams = 0;
    bool has_param = (lookahead->tag != CLOSE_PAR);
    while (has_param) {
        int param_type = lookahead->tag;
        match(param_type);
        char param_name[MAX_CHAR];
        strcpy(param_name, lookahead->lexema);
        match(ID);
        temp_params[temp_nparams].type = param_type;
        strcpy(temp_params[temp_nparams].name, param_name);
        temp_nparams++;
        if (lookahead->tag == COMMA) {
            match(COMMA);
        } else {
            has_param = false;
        }
    }
    match(CLOSE_PAR);
    // Verifica se parametros correspondem ao prototipo (numero, tipo e nome)
    if (temp_nparams != func->nparams) {
        printf("[ERRO] Numero de parametros nao corresponde na implementacao de %s\n", name);
        return false;
    }
    for (int i = 0; i < temp_nparams; i++) {
        if (temp_params[i].type != func->params[i].type || strcmp(temp_params[i].name, func->params[i].name) != 0) {
            printf("[ERRO] Parametro %d nao corresponde (tipo ou nome) na implementacao de %s\n", i, name);
            return false;
        }
    }
    // requisito 1.2: cria tsl nova para esta funcao
    // alocacao dinamica garante que chamadas recursivas tenham TSL propria
    type_symbol_table_variables *new_tsl = sym_create_local_table();
    func->local_table = new_tsl; // associa a TSL criada para a func

    // salva contexto anterior e ativa nova TSL
    type_symbol_table_variables *saved_tsl = current_local_table;
    current_local_table = new_tsl;

    // registra parametros como variaveis locais na TSL (requisito 1.2)
    for (int i = 0; i < temp_nparams; i++) {
        sym_declare(temp_params[i].name, temp_params[i].type, i, current_local_table);
    }

    // gera prologo padrao da funcao
    gen_func_prolog(func->label); // label da funcao

    match(BEGIN);
    
    const char *arg_regs[] = {"r8d", "r9d", "r10d", "r11d"}; 
    for (int i = 0; i < temp_nparams && i < 4; i++) {
        fprintf(output_file, "mov dword [rbp - %d], %s\n", (i+1)*4, arg_regs[i]); // move argumento do registrador para a posição correta na pilha (TSL)
    }

    // declaracoes locais dentro da funcao (requisito 1.1 - TSL)
    while (local_declaration());

    statements();
    
    // requisito 2: obriga return com verificacao de tipo
    if (lookahead->tag != RETURN) {
        printf("[ERRO] Funcao '%s' deve conter comando 'return'.\n", name);
    } else {
        match(RETURN);
        E(); // avalia expressao de retorno
        gen_return(); // pop rax
        match(SEMICOLON);
    }
    match(END);
    gen_func_epilog(); // ret

    // marca funcao como implementada (requisito 3.4)
    func->implemented = 1;

    // restaura contexto anterior (TSL anterior)
    current_local_table = saved_tsl;

    return true;  
}

/**
 * @brief Regra de derivação que analiza expressoes booleanas
 *        no padrao 'id op_rel expr'
 */
int B() {
    int operator;
    if (E()){
        if (boolOperator(&operator)){
            if (E()){
                gen_bool(operator);
                return true;
            }
        }
    }
    return false;
}

int boolOperator(int *operator) {
    int lookahead_tag;
    lookahead_tag = lookahead->tag;
    *operator = -1;
    
    if (lookahead_tag == EQUAL ||
        lookahead_tag == NE ||
        lookahead_tag == LT ||
        lookahead_tag == GT ||
        lookahead_tag == LE ||
        lookahead_tag == GE) {
            *operator = lookahead_tag;
            match(lookahead_tag);
            return true;
    } else {
        printf("[ERRO] Operador relacional esperado. Encontrado: %s\n", lookahead->lexema);
        return false;
    }
}

//------
/* Funções que representam a gramatica que reconhece expressoes aritmeticas */
/* Elaborada nas primeiras aulas */
int E() {
    int b1, b2;
    b2 = -1; // inicializa com valor invalido para evitar warnings
    b1 = T();
    if (b1) 
        b2 = ER();
    return b1 && b2;
}

int ER() {
    if (lookahead->tag == '+')  { 
        int b1, b2;
        match('+');
        b1 = T();
        genAdd();
        if (b1) 
            b2 = ER();
        return b1 && b2;

    } else if (lookahead->tag == '-') {
        int b1, b2;
        match('-');
        b1 = T();
        genSub();
        if (b1)
            b2 = ER();
        return b1 && b2;

    } 
    // TOKENS DE FOLLOW (fim de expressão)
    else if (lookahead->tag == ')'
          || lookahead->tag == SEMICOLON
          || lookahead->tag == COMMA
          || lookahead->tag == ENDTOKEN
          || lookahead->tag == GT
          || lookahead->tag == LT
          || lookahead->tag == GE
          || lookahead->tag == LE
          || lookahead->tag == EQUAL
          || lookahead->tag == NE) {
        return true;
    } 
    else {
        return false;
    }
}

int T() {
    int b1, b2;
    b1 = F();
    if (b1)
        b2 = TR();
    return b1 && b2;
}

int TR() {
    if (lookahead->tag == '*') { 
        int b1, b2;
        match('*');
        b1 = F();
        genMult();
        if (b1)
            b2 = TR();
        return b1 && b2;

    } else if (lookahead->tag == '/') {
        int b1, b2;
        match('/');
        b1 = F();
        genDiv();
        if (b1)
            b2 = TR();
        return b1 && b2;

    } 
    // TOKENS DE FOLLOW
    else if (lookahead->tag == ')'
          || lookahead->tag == SEMICOLON
          || lookahead->tag == COMMA
          || lookahead->tag == ENDTOKEN
          || lookahead->tag == '+'
          || lookahead->tag == '-'
          || lookahead->tag == GT
          || lookahead->tag == LT
          || lookahead->tag == GE
          || lookahead->tag == LE
          || lookahead->tag == EQUAL
          || lookahead->tag == NE) {
        return true;
    } 
    else {
        return false;
    }
}

int F() {
    if (lookahead->tag == '(') {
        int b1,b2;
        match('(');
        b1 = E();
        if (b1)
            b2 = match(')');
        return b1 && b2;
    } else if (lookahead->tag == NUM) {
        int b1;
        char lexema[MAX_TOKEN];
        strcpy(lexema, lookahead->lexema);
        b1 = match(NUM); //substituir 'id' por NUM
        genNum(lexema);
        return b1;
    } else if (lookahead->tag == ID) {
        int b1;
        char lexema[MAX_TOKEN];
        strcpy(lexema, lookahead->lexema);
        if ( sym_find_any(lexema) == NULL ) {
            printf("[ERRO] Variavel nao declarada: %s\n", lexema);
            return false;
        }
        b1 = match(ID);
        gen_id_value(lexema);
        return b1;
    } else {
        return false;
    }
}
/*fim da importação */
//------


//--------------------- MAIN -----------------------

/**
 * @brief Funcao principal (main) do compilador
 * 
 * @return int 
 */
int main(int argc, char *argv[]) {

    //Inicializa a tabela de simbolo global
    initSymbolTableVariables(&global_symbol_table_variables);
    initSymbolTableString();
    initSymbolTableFunctions();

    //Verifica a passagem de parametro
    if (argc != 2) {
        printf("[ERRO]\n\tÉ necessário informar um arquivo de entrada (código) como parâmetro.\n\n");
        exit(EXIT_FAILURE);
    }
    
    initLex(argv[1]); //Carrega codigo
    lookahead = getToken(); //Inicializacao do lookahead

    //Abre o arquivo de saida 
    strcpy(output_file_name, argv[1]);
    strcat(output_file_name, ".asm");
    output_file = fopen(output_file_name, "w+");

    program(); //Chamada da derivacao/funcao inicial da gramatica

    fclose(output_file);
    return 1;
}