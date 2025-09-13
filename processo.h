// Arquivo: processo.h
// Definição do Tipo Abstrato de Dados (TAD) Processo e protótipos de funções

#ifndef PROCESSO_H
#define PROCESSO_H

#define MAX_PROCESSOS 1000  // Capacidade máxima de processos a serem lidos
#define TAM_LINHA 500       // Tamanho máximo da linha do arquivo CSV
#define ERRO -1

// Estrutura para representar um processo judicial
typedef struct {
    long long int id_processo;
    char numero_sigilo;
    char sigla_grau;
    char procedimento;
    char ramo_justica;
    char sigla_tribunal;
    int id_tribunal;
    int recurso;
    int id_ultimo_oj;
    char dt_recebimento;
    int id_ultima_classe;
    int flag_violencia_domestica;
    int flag_feminicidio;
    int flag_ambiental;
    int flag_quilombolas;
    int flag_indigenas;
    int flag_infancia;
    char decisao;
    char dt_resolvido;
    int cnm1;
    int primeirasentm1;
    int baixm1;
    int decm1;
    int mpum1;
    int julgadom1;
    int desm1;
    int susm1;
} Processo;

// Protótipos das funções
Processo *lerDados(const char *nomeArquivo, int *totalProcessos);
void encontrarProcessoMaisAntigo(const Processo *processos, int totalProcessos);
void encontrarOJPorId(const Processo *processos, int totalProcessos, long long int id_busca);
int contarViolenciaDomestica(const Processo *processos, int totalProcessos);
int contarFeminicidio(const Processo *processos, int totalProcessos);
int contarAmbiental(const Processo *processos, int totalProcessos);
int contarQuilombolas(const Processo *processos, int totalProcessos);
int contarIndigenas(const Processo *processos, int totalProcessos);
int contarInfancia(const Processo *processos, int totalProcessos);
void calcularDiasResolucao(const Processo *processos, int totalProcessos);
void calcularCumprimentoMeta1(const Processo *processos, int totalProcessos);
void gerarCSVJulgados(const Processo *processos, int totalProcessos, const char *nomeArquivoSaida);

#endif