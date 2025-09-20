/* =========================
 * arquivo: processo.c
 * ========================= */
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

// Sanitiza campos para CSV
static void sanitize_csv_field(char *s){
    if (!s) return;
    for (char *p = s; *p; ++p){
        if (*p == '\r' || *p == '\n') *p = ' ';
        else if (*p == ';') *p = ','; 
    }
}

// Converte data "YYYY-MM-DD" em dias desde 1970-01-01
static int days_from_civil(int y, unsigned m, unsigned d){
    y -= (m <= 2);
    const int era = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe = (unsigned)(y - era * 400);
    const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + (int)doe - 719468;
}

int dataParaDias(const char *iso8601){
    if (!iso8601 || !*iso8601) return -1;
    int y, m, d;
    if (sscanf(iso8601, "%d-%d-%d", &y, &m, &d) != 3) return -1;
    if (m < 1 || m > 12 || d < 1 || d > 31) return -1;
    return days_from_civil(y, (unsigned)m, (unsigned)d);
}

// Divide linha por ';'
static int split_semicolon(char *linha, char **out, int max_cols){
    int col = 0;
    char *p = linha;
    while (col < max_cols && p){
        char *sep = strchr(p, ';');
        if (sep){
            *sep = '\0';
            strip_crlf(p);
            out[col++] = p;
            p = sep + 1;
        } else {
            strip_crlf(p);
            out[col++] = p;
            p = NULL;
        }
    }
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
    if (!fgets(linha, sizeof(linha), fp)){
        fclose(fp);
        *totalProcessos = 0;
        return processos;
    }

    int i = 0;
    const int NUM_COLS = 27;
    while (i < MAX_PROCESSOS && fgets(linha, sizeof(linha), fp)){
        char *cols[27];
        split_semicolon(linha, cols, NUM_COLS);

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
            printf("Processo %lld: %d dias entre recebimento e resolucao.\n", processos[i].id_processo, dj - dr);
        }else{
            printf("Processo %lld: Nao foi possivel calcular a diferenca de dias (dados invalidos).\n", processos[i].id_processo);
        }
    }
}

void salvarDiasResolucaoCSV(const Processo *processos, int totalProcessos, const char *nomeCSV){
    FILE *fp = fopen(nomeCSV, "wb");
    if (!fp){ printf("ERRO: Nao foi possivel criar %s.\n", nomeCSV); return; }
    fputs("id_processo;dias_entre_recebimento_e_resolucao\r\n", fp);

    int gravados = 0;
    for (int i = 0; i < totalProcessos; i++){
        int dr = dataParaDias(processos[i].dt_recebimento);
        int dj = dataParaDias(processos[i].dt_resolvido);
        if (dr != -1 && dj != -1 && dj >= dr){
            fprintf(fp, "%lld;%d\r\n", processos[i].id_processo, dj - dr);
            gravados++;
        }
    }
    fclose(fp);
    printf("Dias de resolucao gravados em '%s' (%d linhas).\n", nomeCSV, gravados);
}

void imprimirDiasResolucaoAmostra(const Processo *processos, int totalProcessos, int max_linhas){
    int mostradas = 0;
    for (int i = 0; i < totalProcessos && mostradas < max_linhas; i++){
        int dr = dataParaDias(processos[i].dt_recebimento);
        int dj = dataParaDias(processos[i].dt_resolvido);
        if (dr != -1 && dj != -1 && dj >= dr){
            printf("Processo %lld: %d dias.\n", processos[i].id_processo, dj - dr);
            mostradas++;
        }
    }
    if (totalProcessos > max_linhas){
        printf("... (mostradas %d de %d; veja o CSV completo)\n", mostradas, totalProcessos);
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
    long long denom = cnm1 + desm - sus;
    if (denom <= 0){ puts("Nao foi possivel calcular a Meta 1, denominador e zero."); return; }
    double pct = (double)julg / (double)denom * 100.0;
    printf("O percentual de cumprimento da Meta 1 e: %.2f%%\n", pct);
}

double calcularCumprimentoMeta1Streaming(const char *nomeArquivo,
                                         int ano,
                                         long long *out_cnm1,
                                         long long *out_julg,
                                         long long *out_desm,
                                         long long *out_sus){
    FILE *fp = fopen(nomeArquivo, "r");
    if (!fp){
        printf("ERRO: Nao foi possivel abrir %s para streaming.\n", nomeArquivo);
        return -1.0;
    }
    char linha[TAM_LINHA];
    if (!fgets(linha, sizeof(linha), fp)){
        fclose(fp);
        return -1.0;
    }

    long long cnm1 = 0, julg = 0, desm = 0, sus = 0;
    const int NUM_COLS = 27;
    while (fgets(linha, sizeof(linha), fp)){
        char *cols[27];
        split_semicolon(linha, cols, NUM_COLS);

        // Se ano > 0, aplica filtro, senão considera todos
        if (ano > 0){
            char prefixo[6];
            snprintf(prefixo, sizeof(prefixo), "%04d-", ano);
            if (strncmp(cols[9], prefixo, 5) != 0) continue;
        }

        cnm1 += cols[19][0] ? atoll(cols[19]) : 0;
        julg += cols[24][0] ? atoll(cols[24]) : 0;
        desm += cols[25][0] ? atoll(cols[25]) : 0;
        sus  += cols[26][0] ? atoll(cols[26]) : 0;
    }
    fclose(fp);

    if (out_cnm1) *out_cnm1 = cnm1;
    if (out_julg) *out_julg = julg;
    if (out_desm) *out_desm = desm;
    if (out_sus)  *out_sus  = sus;

    long long denom = cnm1 + desm - sus;
    if (denom <= 0){
        puts("Nao foi possivel calcular a Meta 1 (denominador <= 0)");
        return -1.0;
    }
    return (double)julg * 100.0 / (double)denom;
}

void gerarCSVJulgados(const Processo *p, int n, const char *nomeArquivoSaida){
    FILE *fp = fopen(nomeArquivoSaida, "wb");
    if (!fp){
        printf("ERRO: Nao foi possivel criar o arquivo %s.\n", nomeArquivoSaida);
        return;
    }
    fputs("id_processo;numero_sigilo;sigla_grau;procedimento;ramo_justica;sigla_tribunal;id_tribunal;recurso;id_ultimo_oj;dt_recebimento;id_ultima_classe;flag_violencia_domestica;flag_feminicidio;flag_ambiental;flag_quilombolas;flag_indigenas;flag_infancia;decisao;dt_resolvido;cnm1;primeirasentm1;baixm1;decm1;mpum1;julgadom1;desm1;susm1\r\n", fp);

    int count = 0;
    for (int i = 0; i < n; i++){
        if (p[i].julgadom1 > 0){
            char numero_sigilo[128];  snprintf(numero_sigilo, sizeof(numero_sigilo), "%s", p[i].numero_sigilo);
            char sigla_grau[8];       snprintf(sigla_grau,    sizeof(sigla_grau),    "%s", p[i].sigla_grau);
            char procedimento[128];   snprintf(procedimento,  sizeof(procedimento),  "%s", p[i].procedimento);
            char ramo_justica[64];    snprintf(ramo_justica,  sizeof(ramo_justica),  "%s", p[i].ramo_justica);
            char sigla_tribunal[16];  snprintf(sigla_tribunal,sizeof(sigla_tribunal),"%s", p[i].sigla_tribunal);
            char dt_recebimento[11];  snprintf(dt_recebimento,sizeof(dt_recebimento),"%s", p[i].dt_recebimento);
            char decisao[64];         snprintf(decisao,       sizeof(decisao),       "%s", p[i].decisao);
            char dt_resolvido[11];    snprintf(dt_resolvido,  sizeof(dt_resolvido),  "%s", p[i].dt_resolvido);

            sanitize_csv_field(numero_sigilo);
            sanitize_csv_field(sigla_grau);
            sanitize_csv_field(procedimento);
            sanitize_csv_field(ramo_justica);
            sanitize_csv_field(sigla_tribunal);
            sanitize_csv_field(dt_recebimento);
            sanitize_csv_field(decisao);
            sanitize_csv_field(dt_resolvido);

            fprintf(fp, "%lld;%s;%s;%s;%s;%s;%d;%d;%d;%s;%d;%d;%d;%d;%d;%d;%d;%s;%s;%d;%d;%d;%d;%d;%d;%d;%d\r\n",
                    p[i].id_processo, numero_sigilo, sigla_grau, procedimento,
                    ramo_justica, sigla_tribunal, p[i].id_tribunal, p[i].recurso, p[i].id_ultimo_oj,
                    dt_recebimento, p[i].id_ultima_classe, p[i].flag_violencia_domestica, p[i].flag_feminicidio,
                    p[i].flag_ambiental, p[i].flag_quilombolas, p[i].flag_indigenas, p[i].flag_infancia,
                    decisao, dt_resolvido, p[i].cnm1, p[i].primeirasentm1, p[i].baixm1, p[i].decm1,
                    p[i].mpum1, p[i].julgadom1, p[i].desm1, p[i].susm1);
            count++;
        }
    }
    fclose(fp);
    printf("Gerado arquivo '%s' com %d processos julgados (merito).\n", nomeArquivoSaida, count);
}
