#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "processo.h"

// --- util ----

static void strip_crlf(char *s){
    if (!s) return;
    size_t n = strlen(s);
    while (n && (s[n-1] == '\n' || s[n-1] == '\r')) s[--n] = '\0';
}

// Converte "YYYY-MM-DD" para dias desde 1970-01-01 usando mktime.
// Retorna -1 se string vazia ou inválida.
int dataParaDias(const char *iso8601){
    if (!iso8601 || !*iso8601) return -1;
    int y, m, d;
    if (sscanf(iso8601, "%d-%d-%d", &y, &m, &d) != 3) return -1;
    struct tm t = {0};
    t.tm_year = y - 1900;
    t.tm_mon  = m - 1;
    t.tm_mday = d;
    t.tm_isdst = -1;
    time_t seconds = mktime(&t);
    if (seconds == (time_t)-1) return -1;
    return (int)(seconds / 86400);
}

// Divide a linha em até `max_cols` campos separados por ';', preservando
// campos vazios. Escreve em `out[col]` strings já sem CR/LF.
static int split_semicolon(char *linha, char **out, int max_cols){
    int col = 0;
    char *p = linha;
    while (col < max_cols && p){
        char *sep = strchr(p, ';');
        if (sep)
        {
            *sep = '\0';   // termina o campo com NUL (seguro)
            strip_crlf(p); // remove CR/LF do fim, se houver
            out[col++] = p;
            p = sep + 1; // próximo campo começa após o ';'
        }
        else
        {
            strip_crlf(p);
            out[col++] = p;
            p = NULL;
        }
    }
    // completa campos faltantes como vazios
    while (col < max_cols) out[col++] = "";
    return col;
}

// --- leitura ----

Processo *lerDados(const char *nomeArquivo, int *totalProcessos){
    FILE *fp = fopen(nomeArquivo, "r");
    if (!fp){
        printf("ERRO: Nao foi possivel abrir o arquivo %s.\n", nomeArquivo);
        exit(ERRO);
    }

    Processo *processos = (Processo*)calloc(MAX_PROCESSOS, sizeof(Processo));
    if (!processos){
        printf("ERRO: Nao foi possivel alocar memoria.\n");
        exit(ERRO);
    }

    char linha[TAM_LINHA];
    // cabeçalho
    if (!fgets(linha, sizeof(linha), fp)){
        fclose(fp);
        *totalProcessos = 0;
        return processos;
    }

    int i = 0;
    const int NUM_COLS = 27; // conforme enunciado/arquivo
    while (i < MAX_PROCESSOS && fgets(linha, sizeof(linha), fp)){
        char *cols[27];
        split_semicolon(linha, cols, NUM_COLS);

        // Preenchimento seguro (com vazios possíveis)
        processos[i].id_processo      = cols[0][0]  ? atoll(cols[0])  : 0;
        snprintf(processos[i].numero_sigilo, sizeof(processos[i].numero_sigilo), "%s", cols[1]);
        snprintf(processos[i].sigla_grau,    sizeof(processos[i].sigla_grau),    "%s", cols[2]);
        snprintf(processos[i].procedimento,  sizeof(processos[i].procedimento),  "%s", cols[3]);
        snprintf(processos[i].ramo_justica,  sizeof(processos[i].ramo_justica),  "%s", cols[4]);
        snprintf(processos[i].sigla_tribunal,sizeof(processos[i].sigla_tribunal),"%s", cols[5]);
        processos[i].id_tribunal      = cols[6][0]  ? atoi(cols[6])  : 0;
        processos[i].recurso          = cols[7][0]  ? atoi(cols[7])  : 0;
        processos[i].id_ultimo_oj     = cols[8][0]  ? atoi(cols[8])  : 0;
        snprintf(processos[i].dt_recebimento, sizeof(processos[i].dt_recebimento), "%s", cols[9]);
        processos[i].id_ultima_classe = cols[10][0] ? atoi(cols[10]) : 0;

        processos[i].flag_violencia_domestica = cols[11][0] ? atoi(cols[11]) : 0;
        processos[i].flag_feminicidio         = cols[12][0] ? atoi(cols[12]) : 0;
        processos[i].flag_ambiental           = cols[13][0] ? atoi(cols[13]) : 0;
        processos[i].flag_quilombolas         = cols[14][0] ? atoi(cols[14]) : 0;
        processos[i].flag_indigenas           = cols[15][0] ? atoi(cols[15]) : 0;
        processos[i].flag_infancia            = cols[16][0] ? atoi(cols[16]) : 0;

        snprintf(processos[i].decisao,      sizeof(processos[i].decisao),      "%s", cols[17]);
        snprintf(processos[i].dt_resolvido, sizeof(processos[i].dt_resolvido), "%s", cols[18]);

        processos[i].cnm1            = cols[19][0] ? atoi(cols[19]) : 0;
        processos[i].primeirasentm1  = cols[20][0] ? atoi(cols[20]) : 0;
        processos[i].baixm1          = cols[21][0] ? atoi(cols[21]) : 0;
        processos[i].decm1           = cols[22][0] ? atoi(cols[22]) : 0;
        processos[i].mpum1           = cols[23][0] ? atoi(cols[23]) : 0;
        processos[i].julgadom1       = cols[24][0] ? atoi(cols[24]) : 0;
        processos[i].desm1           = cols[25][0] ? atoi(cols[25]) : 0;
        processos[i].susm1           = cols[26][0] ? atoi(cols[26]) : 0;

        i++;
    }

    fclose(fp);
    *totalProcessos = i;
    return processos;
}

// --- análises ----

void encontrarProcessoMaisAntigo(const Processo *processos, int totalProcessos){
    if (totalProcessos == 0){ puts("Nenhum processo encontrado para analise."); return; }

    int idx = -1, menor = -1;
    for (int i=0;i<totalProcessos;i++){
        int d = dataParaDias(processos[i].dt_recebimento);
        if (d != -1 && (menor == -1 || d < menor)){
            menor = d; idx = i;
        }
    }
    if (idx == -1) puts("Nao foi possivel determinar o mais antigo (todas as datas invalidas).");
    else printf("O id_processo do processo com dt_recebimento mais antigo e: %lld\n", processos[idx].id_processo);
}

void encontrarOJPorId(const Processo *processos, int totalProcessos, long long int id_busca){
    for (int i = 0; i < totalProcessos; i++){
        if (processos[i].id_processo == id_busca){
            printf("O id_ultimo_oj para o processo %lld e: %d\n", id_busca, processos[i].id_ultimo_oj);
            return;
        }
    }
    printf("Processo com id %lld nao encontrado.\n", id_busca);
}

int contarViolenciaDomestica(const Processo *p, int n){ int c=0; for (int i=0;i<n;i++) c += p[i].flag_violencia_domestica; return c; }
int contarFeminicidio      (const Processo *p, int n){ int c=0; for (int i=0;i<n;i++) c += p[i].flag_feminicidio; return c; }
int contarAmbiental        (const Processo *p, int n){ int c=0; for (int i=0;i<n;i++) c += p[i].flag_ambiental; return c; }
int contarQuilombolas      (const Processo *p, int n){ int c=0; for (int i=0;i<n;i++) c += p[i].flag_quilombolas; return c; }
int contarIndigenas        (const Processo *p, int n){ int c=0; for (int i=0;i<n;i++) c += p[i].flag_indigenas; return c; }
int contarInfancia         (const Processo *p, int n){ int c=0; for (int i=0;i<n;i++) c += p[i].flag_infancia; return c; }

void calcularDiasResolucao(const Processo *processos, int totalProcessos){
    for (int i=0;i<totalProcessos;i++){
        int dr = dataParaDias(processos[i].dt_recebimento);
        int dj = dataParaDias(processos[i].dt_resolvido);
        if (dr != -1 && dj != -1 && dj >= dr){
            printf("Processo %lld: %d dias entre recebimento e resolucao.\n",
                   processos[i].id_processo, dj - dr);
        }else{
            printf("Processo %lld: Nao foi possivel calcular a diferenca de dias (dados invalidos).\n",
                   processos[i].id_processo);
        }
    }
}

void calcularCumprimentoMeta1(const Processo *processos, int totalProcessos){
    long long cnm1=0, julg=0, desm=0, sus=0;
    for (int i=0;i<totalProcessos;i++){
        cnm1 += processos[i].cnm1;
        julg += processos[i].julgadom1;
        desm += processos[i].desm1;
        sus  += processos[i].susm1;
    }
    long long denom = cnm1 + desm - sus; // conforme enunciado
    if (denom <= 0){ puts("Nao foi possivel calcular a Meta 1, denominador e zero."); return; }
    double pct = (double)julg / (double)denom * 100.0;
    printf("O percentual de cumprimento da Meta 1 e: %.2f%%\n", pct);
}

void gerarCSVJulgados(const Processo *p, int n, const char *nomeArquivoSaida){
    FILE *fp = fopen(nomeArquivoSaida, "w");
    if (!fp){ printf("ERRO: Nao foi possivel criar o arquivo %s.\n", nomeArquivoSaida); return; }
    fprintf(fp, "id_processo;numero_sigilo;sigla_grau;procedimento;ramo_justica;sigla_tribunal;id_tribunal;recurso;id_ultimo_oj;dt_recebimento;id_ultima_classe;flag_violencia_domestica;flag_feminicidio;flag_ambiental;flag_quilombolas;flag_indigenas;flag_infancia;decisao;dt_resolvido;cnm1;primeirasentm1;baixm1;decm1;mpum1;julgadom1;desm1;susm1\n");
    int count=0;
    for (int i=0;i<n;i++){
        if (p[i].julgadom1 > 0){
            fprintf(fp, "%lld;%s;%s;%s;%s;%s;%d;%d;%d;%s;%d;%d;%d;%d;%d;%d;%d;%s;%s;%d;%d;%d;%d;%d;%d;%d;%d\n",
                    p[i].id_processo, p[i].numero_sigilo, p[i].sigla_grau, p[i].procedimento,
                    p[i].ramo_justica, p[i].sigla_tribunal, p[i].id_tribunal, p[i].recurso, p[i].id_ultimo_oj,
                    p[i].dt_recebimento, p[i].id_ultima_classe, p[i].flag_violencia_domestica, p[i].flag_feminicidio,
                    p[i].flag_ambiental, p[i].flag_quilombolas, p[i].flag_indigenas, p[i].flag_infancia,
                    p[i].decisao, p[i].dt_resolvido, p[i].cnm1, p[i].primeirasentm1, p[i].baixm1, p[i].decm1,
                    p[i].mpum1, p[i].julgadom1, p[i].desm1, p[i].susm1);
            count++;
        }
    }
    fclose(fp);
    printf("Gerado arquivo '%s' com %d processos julgados (merito).\n", nomeArquivoSaida, count);
}
