# Trabalho Final #2

**Trabalho final 2 da matéria de compiladores desenvolvida em 2025/2.**

Nesta segunda etapa, o objetivo é evoluir o compilador implementado no Trabalho Final #1, aprimorando e aproximando-o de um modelo real de compilador, com suporte adequado a escopo local e retorno de funções.

O foco principal é:

- Implementação de escopo local de variáveis nas funções;
- Separação entre Tabela de Símbolos Global - TSG (variáveis globais) e Tabela de Símbolos Local - TSL (variáveis de escopo de função);
- Implementação de retorno de função
## Requisito 1:

**1.1 Separação das Tabelas de Símbolos**

Implementar separadamente as tabelas Tabela de Símbolos Global (TSG) e Tabela de Símbolos Local (TSL), esta será por função.

A tabela de funções deve conter os dados já implementados e mais um ponteiro para a TSL da função, ficando com estrutura equivalente a:
- Nome;
- Tipo de retorno;
- Lista de parâmetros e seus tipos;
- Ponteiro para sua TSL (mesmo tipo da tabela de símbolos).

Dessa forma, quando uma função for prototipada este ponteiro será inicializado na tabela de funções com nulo.

**1.2 Regras Semânticas**

Ao processar uma função (após sua chamada), deverá ser criada/alocada uma nova TSL (usando o ponteiro da respectiva função, presente na tabela de funções), e então registrada nos dados da função (por meio do ponteiro). Observe que esta estratégia pode gerar uma falha no contexto de chamadas recursivas. Atente-se em resolver este problema e garantir a criação de múltiplas TSL para possíveis chamadas recursivas.

Os parâmetros da função deverão ser criados/registrados e inicializados na TSL correspondente da função (os parâmetros devem ser tratados como variáveis locais).

No processo de uso e reconhecimento de identificadores/variáveis, primeiro busca-se na TSL.  Se o identificador não for encontrado, busca-se na TSG. Caso o identificador ainda não seja encontrado, então gera-se erro sintático (identificador não declarado).

**1.3 Shadowing**

Deve ser permitido o registro de um mesmo nome para um identificador, desde que ele seja registrado na TSG e em uma (ou várias) TSL de uma função. Sabe-se que o identificador local deve sobrescrever o global dentro do escopo (o global se torna inacessível).

O seguinte trecho de código é um exemplo válido:

```c
int x;
int f() {
    int x;  //válido(escopo local)
}
```

## Requisito 2:
As funções poderão retornar valores. 

Portanto, o comando "return" deve ser adicionado no compilador e a gramática deverá ser modificada para que obrigue a função a executar ao menos uma vez o comando "return". E ainda, o valor retornado deve ser de tipo compatível com o tipo declarado para a função.

Vamos convencionar um processo simplificado que consiste do processo de chamada e retorno da função:

Considere o exemplo de código a ser compilado: 

```c
x = soma(10,20)
```
O código em Assembly a ser gerado deverá ser:

```assembly
li   $a0, 10
li   $a1, 20
jal  soma

move $t0, $v0   #pega valor retornado
sw   $t0, offset($fp) #armazena a "palavra" na memória
```
No escopo da função, o valor que será retornado deverá ser armazenando no registrador padrão $v0 (MIPS). Vamos considerar a estrutura padrão do Assembly com os seguintes preâmbulos e epílogos (padrões), nossa função soma ficaria:
```assembly
soma:
    # Preambulo -Padrao-
    addi $sp, $sp, -16
    sw   $ra, 0($sp)
    sw   $fp, 4($sp)
    move $fp, $sp

    # a + b
    lw   $t0, 8($fp)      # parâmetro a
    lw   $t1, 12($fp)     # parâmetro b
    add  $t2, $t0, $t1

    move $v0, $t2         # <<<<<< VALOR DE RETORNO

    # Epilogo -Padrao-
    move $sp, $fp
    lw   $fp, 4($sp)
    lw   $ra, 0($sp)
    addi $sp, $sp, 16
    jr   $ra              #Chamada de retorno
```
## Requisito 3:
Verifique se o compilador está avaliando os seguintes aspectos:
- Uso de variável não declarada;
- Tipo incompatível em atribuições;
- Chamada de função com tipo incorreto;
- Função declarada mas nunca implementada


## Autores
- Anna Bheatryz Martins dos Santos
- Mariana Sanchez Pedroni
