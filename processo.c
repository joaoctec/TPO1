#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "processo.h"

/* --- utilitários compactos --- */
static void strip_crlf(char *s){
    if (!s) return;
    size_t n = strlen(s);
    while (n && (s[n - 1] == '\n' || s[n - 1] == '\r')) s[--n] = '\0';
}
static void sanitize_csv_field(char *s){
    if (!s) return;
    for (char *p = s; *p; ++p){
        if (*p == '\r' || *p == '\n') *p = ' ';
        else if (*p == ';') *p = ',';
    }
}
static int split_semicolon(char *ln, char **out, int maxc){
    int c = 0; char *p = ln;
    while (c < maxc && p){
        char *sep = strchr(p, ';');
        if (sep){ *sep = '\0'; strip_crlf(p); out[c++] = p; p = sep + 1; }
        else   { strip_crlf(p); out[c++] = p; p = NULL;   }
    }
    while (c < maxc) out[c++] = "";
    return c;
}

/* --- datas (algoritmo proleptico) --- */
static int days_from_civil(int y, unsigned m, unsigned d){
    y -= (m <= 2);
    int era = (y >= 0 ? y : y - 399) / 400;
    unsigned yoe = (unsigned)(y - era * 400);
    unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + (int)doe - 719468;
}
int dataParaDias(const char *iso){
    if (!iso || !*iso) return -1;
    int y, m, d;
    if (sscanf(iso, "%d-%d-%d", &y, &m, &d) != 3) return -1;
    if (m < 1 || m > 12 || d < 1 || d > 31) return -1;
    return days_from_civil(y, (unsigned)m, (unsigned)d);
}

/* --- leitura do CSV em memória --- */
Processo *lerDados(const char *arq, int *tot){
    FILE *fp = fopen(arq, "r");
    if (!fp){ printf("ERRO: Nao foi possivel abrir o arquivo %s.\n", arq); exit(ERRO); }
    Processo *p = (Processo *)calloc(MAX_PROCESSOS, sizeof(Processo));
    if (!p){ puts("ERRO: Nao foi possivel alocar memoria."); exit(ERRO); }

    char ln[TAM_LINHA];
    if (!fgets(ln, sizeof(ln), fp)){ fclose(fp); *tot = 0; return p; } /* cabeçalho */

    int i = 0, NC = 27;
    while (i < MAX_PROCESSOS && fgets(ln, sizeof(ln), fp)){
        char *c[27]; split_semicolon(ln, c, NC);
        p[i].id_processo = c[0][0] ? atoll(c[0]) : 0;
        snprintf(p[i].numero_sigilo, sizeof(p[i].numero_sigilo), "%s", c[1]);
        snprintf(p[i].sigla_grau,   sizeof(p[i].sigla_grau),   "%s", c[2]);
        snprintf(p[i].procedimento, sizeof(p[i].procedimento), "%s", c[3]);
        snprintf(p[i].ramo_justica, sizeof(p[i].ramo_justica), "%s", c[4]);
        snprintf(p[i].sigla_tribunal,sizeof(p[i].sigla_tribunal),"%s", c[5]);
        p[i].id_tribunal = c[6][0] ? atoi(c[6]) : 0;
        p[i].recurso     = c[7][0] ? atoi(c[7]) : 0;
        p[i].id_ultimo_oj= c[8][0] ? atoi(c[8]) : 0;
        snprintf(p[i].dt_recebimento, sizeof(p[i].dt_recebimento), "%s", c[9]);
        p[i].id_ultima_classe = c[10][0] ? atoi(c[10]) : 0;
        p[i].flag_violencia_domestica = c[11][0] ? atoi(c[11]) : 0;
        p[i].flag_feminicidio         = c[12][0] ? atoi(c[12]) : 0;
        p[i].flag_ambiental           = c[13][0] ? atoi(c[13]) : 0;
        p[i].flag_quilombolas         = c[14][0] ? atoi(c[14]) : 0;
        p[i].flag_indigenas           = c[15][0] ? atoi(c[15]) : 0;
        p[i].flag_infancia            = c[16][0] ? atoi(c[16]) : 0;
        snprintf(p[i].decisao,      sizeof(p[i].decisao),      "%s", c[17]);
        snprintf(p[i].dt_resolvido, sizeof(p[i].dt_resolvido), "%s", c[18]);
        p[i].cnm1           = c[19][0] ? atoi(c[19]) : 0;
        p[i].primeirasentm1 = c[20][0] ? atoi(c[20]) : 0;
        p[i].baixm1         = c[21][0] ? atoi(c[21]) : 0;
        p[i].decm1          = c[22][0] ? atoi(c[22]) : 0;
        p[i].mpum1          = c[23][0] ? atoi(c[23]) : 0;
        p[i].julgadom1      = c[24][0] ? atoi(c[24]) : 0;
        p[i].desm1          = c[25][0] ? atoi(c[25]) : 0;
        p[i].susm1          = c[26][0] ? atoi(c[26]) : 0;
        i++;
    }
    fclose(fp);
    *tot = i;
    return p;
}

/* --- 3) mais antigo --- */
void encontrarProcessoMaisAntigo(const Processo *p, int n){
    if (!n){ puts("Nenhum processo encontrado para analise."); return; }
    int idx = -1, menor = -1;
    for (int i = 0; i < n; i++){
        int d = dataParaDias(p[i].dt_recebimento);
        if (d != -1 && (menor == -1 || d < menor)){ menor = d; idx = i; }
    }
    if (idx == -1) puts("Nao foi possivel determinar o mais antigo (todas as datas invalidas).");
    else printf("O id_processo do processo com dt_recebimento mais antigo e: %lld\n", p[idx].id_processo);
}

/* --- 2) OJ por id --- */
void encontrarOJPorId(const Processo *p, int n, long long id){
    for (int i = 0; i < n; i++)
        if (p[i].id_processo == id){
            printf("O id_ultimo_oj para o processo %lld e: %d\n", id, p[i].id_ultimo_oj);
            return;
        }
    printf("Processo com id %lld nao encontrado.\n", id);
}

/* --- contagens de flags (diretas para clareza) --- */
int contarViolenciaDomestica(const Processo *p, int n){ int c=0; for(int i=0;i<n;i++) c+=p[i].flag_violencia_domestica; return c; }
int contarFeminicidio      (const Processo *p, int n){ int c=0; for(int i=0;i<n;i++) c+=p[i].flag_feminicidio;       return c; }
int contarAmbiental        (const Processo *p, int n){ int c=0; for(int i=0;i<n;i++) c+=p[i].flag_ambiental;         return c; }
int contarQuilombolas      (const Processo *p, int n){ int c=0; for(int i=0;i<n;i++) c+=p[i].flag_quilombolas;       return c; }
int contarIndigenas        (const Processo *p, int n){ int c=0; for(int i=0;i<n;i++) c+=p[i].flag_indigenas;         return c; }
int contarInfancia         (const Processo *p, int n){ int c=0; for(int i=0;i<n;i++) c+=p[i].flag_infancia;          return c; }

/* --- 10) impressão completa de dias (mantida p/ compatibilidade) --- */
void calcularDiasResolucao(const Processo *p, int n){
    for (int i = 0; i < n; i++){
        int dr = dataParaDias(p[i].dt_recebimento), dj = dataParaDias(p[i].dt_resolvido);
        if (dr != -1 && dj != -1 && dj >= dr)
            printf("Processo %lld: %d dias entre recebimento e resolucao.\n", p[i].id_processo, dj - dr);
        else
            printf("Processo %lld: Nao foi possivel calcular a diferenca de dias (dados invalidos).\n", p[i].id_processo);
    }
}

/* --- NOVO: compara datas por id_processo --- */
int mostrarComparacaoDatasPorId(const Processo *p, int n, long long id){
    for (int i = 0; i < n; i++){
        if (p[i].id_processo == id){
            int dr = dataParaDias(p[i].dt_recebimento);
            int dj = dataParaDias(p[i].dt_resolvido);

            printf("\nComparacao de datas para o processo %lld\n", id);
            printf("dt_recebimento: %s\n", p[i].dt_recebimento[0] ? p[i].dt_recebimento : "(vazia)");
            printf("dt_resolvido : %s\n", p[i].dt_resolvido[0]  ? p[i].dt_resolvido  : "(vazia)");

            if (dr == -1){
                puts("Dados invalidos de dt_recebimento.");
                return 0;
            }
            /* Sem data de resolucao, invalida ou anterior ao recebimento => nao finalizado */
            if (!p[i].dt_resolvido[0] || dj == -1 || dj < dr){
                puts("O processo nao foi finalizado.");
                return 0;
            }

            printf("Dias entre recebimento e resolucao: %d\n", dj - dr);
            return 1;
        }
    }
    printf("Processo com id %lld nao encontrado.\n", id);
    return 0;
}

/* --- 11) Meta 1 em memória (mantida) --- */
void calcularCumprimentoMeta1(const Processo *p, int n){
    long long cn = 0, ju = 0, de = 0, su = 0;
    for (int i = 0; i < n; i++){ cn += p[i].cnm1; ju += p[i].julgadom1; de += p[i].desm1; su += p[i].susm1; }
    long long den = cn + de - su;
    if (den <= 0){ puts("Nao foi possivel calcular a Meta 1, denominador e zero."); return; }
    printf("O percentual de cumprimento da Meta 1 e: %.2f%%\n", (double)ju * 100.0 / (double)den);
}

/* --- 11) Meta 1 streaming (para arquivo grande) --- */
double calcularCumprimentoMeta1Streaming(const char *arq, int ano, long long *oc, long long *oj, long long *od, long long *os){
    FILE *fp = fopen(arq, "r");
    if (!fp){ printf("ERRO: Nao foi possivel abrir %s para streaming.\n", arq); return -1.0; }
    char ln[TAM_LINHA];
    if (!fgets(ln, sizeof(ln), fp)){ fclose(fp); return -1.0; } /* header */

    long long cn = 0, ju = 0, de = 0, su = 0;
    int NC = 27;
    while (fgets(ln, sizeof(ln), fp)){
        char *c[27]; split_semicolon(ln, c, NC);
        if (ano > 0){
            char pref[6]; snprintf(pref, sizeof(pref), "%04d-", ano);
            if (strncmp(c[9], pref, 5) != 0) continue; /* filtra por ano do recebimento */
        }
        cn += c[19][0] ? atoll(c[19]) : 0;
        ju += c[24][0] ? atoll(c[24]) : 0;
        de += c[25][0] ? atoll(c[25]) : 0;
        su += c[26][0] ? atoll(c[26]) : 0;
    }
    fclose(fp);

    if (oc) *oc = cn; if (oj) *oj = ju; if (od) *od = de; if (os) *os = su;
    long long den = cn + de - su;
    if (den <= 0){ puts("Nao foi possivel calcular a Meta 1 (denominador <= 0)"); return -1.0; }
    return (double)ju * 100.0 / (double)den;
}

/* --- 12) CSV com julgados (mérito) --- */
void gerarCSVJulgados(const Processo *p, int n, const char *out){
    FILE *fp = fopen(out, "wb");
    if (!fp){ printf("ERRO: Nao foi possivel criar o arquivo %s.\n", out); return; }
    fputs("id_processo;numero_sigilo;sigla_grau;procedimento;ramo_justica;sigla_tribunal;id_tribunal;recurso;id_ultimo_oj;dt_recebimento;id_ultima_classe;flag_violencia_domestica;flag_feminicidio;flag_ambiental;flag_quilombolas;flag_indigenas;flag_infancia;decisao;dt_resolvido;cnm1;primeirasentm1;baixm1;decm1;mpum1;julgadom1;desm1;susm1\r\n", fp);
    int cnt = 0;
    for (int i = 0; i < n; i++) if (p[i].julgadom1 > 0){
        char numero_sigilo[128], sigla_grau[8], procedimento[128], ramo_justica[64], sigla_tribunal[16], dt_recebimento[11], decisao[64], dt_resolvido[11];
        snprintf(numero_sigilo, sizeof(numero_sigilo), "%s", p[i].numero_sigilo);
        snprintf(sigla_grau, sizeof(sigla_grau), "%s", p[i].sigla_grau);
        snprintf(procedimento, sizeof(procedimento), "%s", p[i].procedimento);
        snprintf(ramo_justica, sizeof(ramo_justica), "%s", p[i].ramo_justica);
        snprintf(sigla_tribunal, sizeof(sigla_tribunal), "%s", p[i].sigla_tribunal);
        snprintf(dt_recebimento, sizeof(dt_recebimento), "%s", p[i].dt_recebimento);
        snprintf(decisao, sizeof(decisao), "%s", p[i].decisao);
        snprintf(dt_resolvido, sizeof(dt_resolvido), "%s", p[i].dt_resolvido);
        sanitize_csv_field(numero_sigilo); sanitize_csv_field(sigla_grau); sanitize_csv_field(procedimento);
        sanitize_csv_field(ramo_justica);   sanitize_csv_field(sigla_tribunal);
        sanitize_csv_field(dt_recebimento); sanitize_csv_field(decisao); sanitize_csv_field(dt_resolvido);
        fprintf(fp, "%lld;%s;%s;%s;%s;%s;%d;%d;%d;%s;%d;%d;%d;%d;%d;%d;%d;%s;%s;%d;%d;%d;%d;%d;%d;%d;%d\r\n",
                p[i].id_processo, numero_sigilo, sigla_grau, procedimento, ramo_justica, sigla_tribunal, p[i].id_tribunal, p[i].recurso, p[i].id_ultimo_oj,
                dt_recebimento, p[i].id_ultima_classe, p[i].flag_violencia_domestica, p[i].flag_feminicidio, p[i].flag_ambiental, p[i].flag_quilombolas,
                p[i].flag_indigenas, p[i].flag_infancia, decisao, dt_resolvido, p[i].cnm1, p[i].primeirasentm1, p[i].baixm1, p[i].decm1, p[i].mpum1,
                p[i].julgadom1, p[i].desm1, p[i].susm1);
        cnt++;
    }
    fclose(fp);
    printf("Gerado arquivo '%s' com %d processos julgados (merito).\n", out, cnt);
}