// Definição do TAD Processo e protótipos

#ifndef PROCESSO_H
#define PROCESSO_H

#include <time.h>

#define MAX_PROCESSOS 3000000
#define TAM_LINHA     32768
#define ERRO          -1

typedef struct {
    long long int id_processo;
    char numero_sigilo[128];
    char sigla_grau[8];
    char procedimento[128];
    char ramo_justica[64];
    char sigla_tribunal[16];
    int  id_tribunal;
    int  recurso;
    int  id_ultimo_oj;
    char dt_recebimento[11]; // "YYYY-MM-DD"
    int  id_ultima_classe;
    int  flag_violencia_domestica;
    int  flag_feminicidio;
    int  flag_ambiental;
    int  flag_quilombolas;
    int  flag_indigenas;
    int  flag_infancia;
    char decisao[64];
    char dt_resolvido[11];   // "YYYY-MM-DD"
    int  cnm1;
    int  primeirasentm1;
    int  baixm1;
    int  decm1;
    int  mpum1;
    int  julgadom1;
    int  desm1;
    int  susm1;
} Processo;

Processo *lerDados(const char *nomeArquivo, int *totalProcessos);
void encontrarProcessoMaisAntigo(const Processo *processos, int totalProcessos);
void encontrarOJPorId(const Processo *processos, int totalProcessos, long long int id_busca);
int  contarViolenciaDomestica(const Processo *processos, int totalProcessos);
int  contarFeminicidio(const Processo *processos, int totalProcessos);
int  contarAmbiental(const Processo *processos, int totalProcessos);
int  contarQuilombolas(const Processo *processos, int totalProcessos);
int  contarIndigenas(const Processo *processos, int totalProcessos);
int  contarInfancia(const Processo *processos, int totalProcessos);
void calcularDiasResolucao(const Processo *processos, int totalProcessos);
void calcularCumprimentoMeta1(const Processo *processos, int totalProcessos);
void gerarCSVJulgados(const Processo *processos, int totalProcessos, const char *nomeArquivoSaida);

// util
int dataParaDias(const char *iso8601); // "YYYY-MM-DD" -> dias desde epoch (ou -1 se vazio/invalid)

#endif