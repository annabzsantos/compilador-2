# Trabalho Final #2

**Trabalho final 2 da matéria de compiladores desenvolvida em 2025/2.**

Nesta segunda etapa, o objetivo é evoluir o compilador implementado no Trabalho Final #1, aprimorando e aproximando-o de um modelo real de compilador, com suporte adequado a escopo local e retorno de funções.

O foco principal é:

* Implementação de escopo local de variáveis nas funções;
* Separação entre Tabela de Símbolos Global (TSG) e Tabela de Símbolos Local (TSL);
* Implementação de retorno de função.

---

## 🛠 Requisito 1

Nesta etapa, a gestão de memória e símbolos deve diferenciar o contexto global do contexto interno de cada função, permitindo o isolamento de dados.

### 1.1 Separação das Tabelas de Símbolos

Implementar separadamente a **TSG** e a **TSL** (esta será por função). A tabela de funções deve conter os dados já implementados e mais um ponteiro para a TSL da função, ficando com estrutura equivalente a:

* Nome;
* Tipo de retorno;
* Lista de parâmetros e seus tipos;
* **Ponteiro para sua TSL** (mesmo tipo da tabela de símbolos).

> Ao prototipar uma função, este ponteiro será inicializado como `null`.

### 1.2 Regras Semânticas

Ao processar uma função, deverá ser alocada uma nova TSL usando o ponteiro da respectiva função.

* **Recursão:** A estratégia deve garantir a criação de múltiplas TSLs para suportar chamadas recursivas.
* **Parâmetros:** Devem ser registrados e inicializados na TSL correspondente (tratados como variáveis locais).
* **Busca de Identificadores:** Primeiro busca-se na TSL. Se não encontrado, busca-se na TSG. Caso ainda assim não seja encontrado, gera-se erro de "identificador não declarado".

### 1.3 Shadowing

Deve ser permitido o registro de um mesmo nome para um identificador, desde que um seja global (TSG) e o outro local (TSL). O identificador local deve sobrescrever o global dentro de seu escopo.

```c
int x;
int f() {
    int x;  // Válido (escopo local - shadowing)
}

```

---

## 🛠 Requisito 2

As funções agora poderão retornar valores. O comando `return` deve ser adicionado e a gramática modificada para obrigar a função a executá-lo ao menos uma vez, validando a compatibilidade de tipos.

### Processo de Chamada e Retorno (MIPS)

Para o exemplo `x = soma(10, 20)`, o Assembly gerado deve seguir o padrão:

```assembly
li   $a0, 10
li   $a1, 20
jal  soma

move $t0, $v0         # Pega valor retornado
sw   $t0, offset($fp) # Armazena na memória

```

### Estrutura da Função (Preâmbulo e Epílogo)

O valor de retorno deve ser armazenado no registrador `$v0`. A estrutura padrão deve ser:

```assembly
soma:
    # Preambulo - Padrao
    addi $sp, $sp, -16
    sw   $ra, 0($sp)
    sw   $fp, 4($sp)
    move $fp, $sp

    # Corpo da Função (ex: a + b)
    lw   $t0, 8($fp)      # parâmetro a
    lw   $t1, 12($fp)     # parâmetro b
    add  $t2, $t0, $t1
    move $v0, $t2         # VALOR DE RETORNO EM $v0

    # Epilogo - Padrao
    move $sp, $fp
    lw   $fp, 4($sp)
    lw   $ra, 0($sp)
    addi $sp, $sp, 16
    jr   $ra              # Chamada de retorno

```

---

## 🛠 Requisito 3

O compilador deve realizar as seguintes verificações semânticas e de integridade:

* **Uso de variável não declarada:** Verificação em ambos os escopos (TSL e TSG);
* **Tipo incompatível:** Validação em atribuições de valores;
* **Chamada de função:** Verificação se os tipos de argumentos passados condizem com o protótipo;
* **Implementação:** Verificar se funções declaradas/prototipadas foram efetivamente implementadas.

---

## Autores

* Anna Bheatryz Martins dos Santos
* Mariana Sanchez Pedroni

---
