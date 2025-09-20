# N1 de Estrutura de Dados - Código de Análise de Casos Judiciários do TJDFT em C


Este projeto é um programa em C que analisa um grande volume de dados de processos judiciais do Tribunal de Justiça do Distrito Federal e Territórios (TJDFT) a partir de um arquivo CSV. Ele faz análises quantitativas, gera o relatório de casos por categoria e gera estatísticas de  previsão do tempo de resolução, assim ajudando a extrair informações importantes para estudo ou pesquisa.


---

## Funcionalidades 

* **Análise de Desempenho:** Calcula o tempo de execução total do programa e o tempo gasto no processamento dos dados.
* **Leitura de Dados:** Processa um arquivo CSV com mais de 200.000 registros de processos.

* **Contagem de Categorias:** Identifica e conta processos relacionados a:
    * Causas ambientais
    * Causas envolvendo quilombolas e indígenas
    * Violência doméstica, feminicídio, infância e juventude
* **Análise de Tempo de Resolução:** Calcula o tempo (em dias) entre o início e a resolução de cada processo.
* **Busca de Dados:** Permite encontrar o processo mais antigo e o "último Órgão Julgador" de um processo específico pelo seu ID.
* **Cálculo de Meta:** Analisa e calcula o percentual de cumprimento da Meta 1, que visa julgar mais processos do que os que entram.
* **Geração de Relatório:** Cria um novo arquivo CSV contendo apenas os processos que foram "julgados" de acordo com a Meta 1.

---

## Estrutura do Projeto

O projeto é modular e dividido em três arquivos principais:

* `main.c`: A função principal do programa, que orquestra a execução das demais funcionalidades (leitura, análise, contagem e geração de relatórios).
* `processo.c`: Contém a lógica de todas as funções do projeto, incluindo a leitura de dados, análises, cálculos de meta e contagens.
* `processo.h`: Define a estrutura de dados `Processo` e os protótipos de todas as funções públicas.



### 2. Estrutura do Arquivo

* `#ifndef PROCESSO_H`
* `#define PROCESSO_H`

* `#include <time.h>`

Proteção contra inclusão múltipla (#ifndef / #define).
inclusão de <time.h>, necessária para cálculos de datas.

* `#define MAX_PROCESSOS 3000000`
* `#define TAM_LINHA     1024`
* `#define ERRO          -1`

MAX_PROCESSOS: limite máximo de registros suportados (3 milhões).

TAM_LINHA: tamanho máximo de uma linha lida do CSV (1024 caracteres).

ERRO: valor de retorno padrão para indicar falha em operações.

A estrutura Processo contém 27 variáveis, sendo responsável por armazenar todas as informações de uma linha do arquivo CSV.

Cada campo reflete uma informação processual, como:
* id_processo: identificador único do processo.

* numero_sigilo: número ou código com eventual sigilo.

* sigla_grau: grau de jurisdição (ex.: “1G” para primeira instância).

* procedimento: tipo de procedimento.

* ramo_justica: ramo da justiça (ex.: trabalhista, penal, cível).

* sigla_tribunal / id_tribunal: identificadores do tribunal.

* recurso: indica se houve recurso.

* id_ultimo_oj: último órgão julgador.

* dt_recebimento: data de recebimento no formato "YYYY-MM-DD".

* flags (flag_violencia_domestica, flag_feminicidio, etc.): marcadores de situações especiais.

* decisao: texto curto com a decisão judicial.

* dt_resolvido: data de resolução do processo.

* Campos cnm1, primeirasentm1, baixm1, decm1, mpum1, julgadom1, desm1, susm1: métricas relacionadas ao      cumprimento da Meta 1 do CNJ.


## Análise de Dados e Lógica de Negócio
Leitura de CSV: A função lerDados lê o arquivo CSV linha por linha, separando os campos pelo delimitador ponto e vírgula (:).

Data Parsing: A função utilitária dataParaDias converte strings de data no formato "YYYY-MM-DD" para um número de dias a partir de "1970-01-01", facilitando o cálculo de diferenças de tempo.

Contagem de Categorias: Funções como contarViolenciaDomestica e contarFeminicidio percorrem o array de processos e somam os valores dos flags binários (flag_violencia_domestica, flag_feminicidio, etc.) para indicar a presença daquele tipo de ocorrência.

Cálculo da Meta 1: A função calcularCumprimentoMeta1 implementa a fórmula: percentual = (julgadom1 / (cnm1 + desm1 - susm1)) * 100.

Geração de CSV: A função gerarCSVJulgados opera sobre o array de processos e escreve em um novo arquivo CSV apenas os registros que possuem o flag julgadom1 com valor maior que 0.


## Meta 1 do CNJ

A Meta 1 é o foco principal da análise, que avalia o julgamento de processos mais antigos em cada tribunal.
O sistema:

* 1.Identifica processos enquadrados na meta.

* 2.Avalia critérios de julgamento e movimentações.

* 3.Calcula a taxa de cumprimento.

 O resultado  é um índice de 100,52%.

## main.c

## Visão Geral

O arquivo main.c é o ponto de entrada do sistema. Ele coordena a leitura de dados processuais a partir de um arquivo CSV, realiza análises estatísticas, calcula o cumprimento da Meta 1 do CNJ e gera relatórios auxiliares.

Além disso, possui um sistema de cronômetro para medir o tempo de execução de cada etapa crítica, auxiliando na avaliação de desempenho.

## Estrutura e Principais Componentes
## 1. Cabeçalhos e Bibliotecas

stdio.h e stdlib.h: Entrada e saída padrão, alocação dinâmica de memória.

time.h: Funções de tempo, usadas para implementar o cronômetro.

processo.h: Cabeçalho com a definição da struct Processo e os protótipos das funções utilizadas.

## 2. Macros para Cronômetro

TIC(var): Inicia uma variável de tempo.

TOC_MS(var): Calcula o tempo decorrido desde a macro TIC, em milissegundos.

Essas macros permitem medir o tempo de execução de trechos específicos do código (ex.: leitura de dados, cálculo da Meta 1).

## 3. Função Principal – main

* Início do cronômetro total
* Mede o tempo do programa inteiro.
* Lê o arquivo CSV.

* Armazena cada linha em uma instância da struct Processo.

* Retorna a quantidade total de registros lidos.


## 4.Cálculo do tempo de resolução


## 5.Análises Gerais

* Identifica o processo mais antigo.

* Localiza um processo por ID.

* Conta processos por categorias temáticas:

* Violência doméstica

* Feminicídio

* Causas ambientais

* Comunidades quilombolas

* Povos indígenas

* Infância e juventude


## Compilação e Execução 
Pré-requisitos: Possuir o GCC (ou um compilador C compatível) instalado na sua máquina.

Arquivos Necessários: Deve-se ter os arquivos main.c, processo.c, processo.h, TJDFT_filtrado.csv e TJDF_amostra.csv e eles devem ficar na mesma pasta

Compilação
Abra o terminal na pasta do projeto e execute o seguinte comando:

```Bash
gcc processo.c main.c -o processo.exe



Execução
Para rodar o programa, execute o arquivo gerado:

```Bash


./processo.exe

```

