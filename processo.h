#ifndef PROCESSO_H
#define PROCESSO_H
#include <time.h>
#define MAX_PROCESSOS 3000000
#define TAM_LINHA 32768
#define ERRO -1
typedef struct
{
    long long id_processo;
    char numero_sigilo[128], sigla_grau[8], procedimento[128], ramo_justica[64], sigla_tribunal[16];
    int id_tribunal, recurso, id_ultimo_oj;
    char dt_recebimento[11];
    int id_ultima_classe;
    int flag_violencia_domestica, flag_feminicidio, flag_ambiental, flag_quilombolas, flag_indigenas, flag_infancia;
    char decisao[64], dt_resolvido[11];
    int cnm1, primeirasentm1, baixm1, decm1, mpum1, julgadom1, desm1, susm1;
} Processo;

Processo *lerDados(const char *, int *);
void encontrarProcessoMaisAntigo(const Processo *, int);
void encontrarOJPorId(const Processo *, int, long long);
int contarViolenciaDomestica(const Processo *, int), contarFeminicidio(const Processo *, int), contarAmbiental(const Processo *, int),
    contarQuilombolas(const Processo *, int), contarIndigenas(const Processo *, int), contarInfancia(const Processo *, int);
void calcularDiasResolucao(const Processo *, int), calcularCumprimentoMeta1(const Processo *, int), gerarCSVJulgados(const Processo *, int, const char *);
int dataParaDias(const char *);
void salvarDiasResolucaoCSV(const Processo *, int, const char *);
void imprimirDiasResolucaoAmostra(const Processo *, int, int);
double calcularCumprimentoMeta1Streaming(const char *, int, long long *, long long *, long long *, long long *);

#endif