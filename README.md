1-Descrição Geral-processo.h

O arquivo processo.h define a estrutura de dados Processo e os protótipos de funções auxiliares que compõem o sistema de análise de processos judiciais a partir de arquivos CSV.

Ele estabelece:

°Constantes globais de configuração.

°Estrutura Processo, que corresponde a uma linha do arquivo CSV.

°Funções de leitura, consulta, análise e exportação de informações.

°Função utilitária para manipulação de datas.

2-Estrutura do Arquivo

#ifndef PROCESSO_H
#define PROCESSO_H

#include <time.h>

°Proteção contra inclusão múltipla (#ifndef / #define).

°Inclusão de <time.h>, necessária para cálculos de datas.

#define MAX_PROCESSOS 3000000
#define TAM_LINHA     1024
#define ERRO          -1


°MAX_PROCESSOS: limite máximo de registros suportados (3 milhões).

°TAM_LINHA: tamanho máximo de uma linha lida do CSV (1024 caracteres).

°ERRO: valor de retorno padrão para indicar falha em operações.


struct Processo contém 27 variáveis, sendo responsável por armazenar todas as informações de uma linha do arquivo CSV.

Cada campo reflete uma informação processual, como:

°id_processo: identificador único do processo.

°numero_sigilo: número ou código com eventual sigilo.

°sigla_grau: grau de jurisdição (ex.: “1G” para primeira instância).

°procedimento: tipo de procedimento.

°ramo_justica: ramo da justiça (ex.: trabalhista, penal, cível).

°sigla_tribunal / id_tribunal: identificadores do tribunal.

°recurso: indica se houve recurso.

°id_ultimo_oj: último órgão julgador.

°dt_recebimento: data de recebimento no formato "YYYY-MM-DD".

°flags (flag_violencia_domestica, flag_feminicidio, etc.): marcadores de situações especiais.

°decisao: texto curto com a decisão judicial.

°dt_resolvido: data de resolução do processo.

°Campos cnm1, primeirasentm1, baixm1, decm1, mpum1, julgadom1, desm1, susm1: métricas relacionadas ao cumprimento da Meta 1 do CNJ.

3-Protótipos de Funções

3.1 Entrada e Leitura
Processo *lerDados(const char *nomeArquivo, int *totalProcessos);

Responsável por ler os dados de um arquivo CSV e armazenar em memória.

Retorna um vetor dinâmico de Processo.

Atualiza totalProcessos com a quantidade carregada.

3.2 Consultas e Análises
void encontrarProcessoMaisAntigo(const Processo *processos, int totalProcessos);
void encontrarOJPorId(const Processo *processos, int totalProcessos, long long int id_busca);

Localizam informações específicas:

O processo mais antigo.

O último órgão julgador (OJ) de um processo por ID.

3.3 Contagem de Categorias