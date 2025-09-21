/* =========================
 * arquivo: main.c
 * ========================= */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "processo.h"

#define TIC(v)       clock_t v = clock()
#define TOC_MS(v)    (1000.0 * (clock() - (v)) / CLOCKS_PER_SEC)

/* Formata milissegundos em MM:SS.mmm */
static void print_mm_ss_mmm(double ms, const char *lbl){
    long long t = (long long)(ms + 0.5);
    int mm = (int)(t / 60000), ss = (int)((t % 60000) / 1000), mmm = (int)(t % 1000);
    printf("%s %02d:%02d.%03d (min:seg.ms)\n", lbl, mm, ss, mmm);
}

/* (Opcional) consome o resto da linha depois de leituras com scanf */
static void limpa_buffer(void){
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int main(void){
    int total = 0;

    TIC(t_total);
    Processo *ps = lerDados("TJDFT_filtrado.csv", &total);

    if (total > 0){
        puts("------------------------------------------");
        puts("Analise do tempo de resolucao dos processos");
        puts("Calculo do tempo de resolucao:");
        puts("------------------------------------------\n");

        TIC(t_datas);
        salvarDiasResolucaoCSV(ps, total, "dias_resolucao.csv");
        imprimirDiasResolucaoAmostra(ps, total, 20);
        double ms_datas = TOC_MS(t_datas);
        
        puts("\n------------------------------------------");
        puts("Analise de Dados do Tribunal de Justica do Distrito Federal");
        puts("------------------------------------------\n");
        puts("------------------------------------------");
        printf("Total de processos lidos: %d\n\n", total);

        /* 3) Processo mais antigo por dt_recebimento */
        encontrarProcessoMaisAntigo(ps, total);

        /* 2) id_ultimo_oj a partir de id_processo — usando scanf APENAS aqui */
        long long idq;
        printf("Informe um id_processo para consultar o id_ultimo_oj (0 para pular): ");
        if (scanf("%lld", &idq) != 1){
            printf("Entrada invalida. Pulando consulta.\n");
            limpa_buffer();
        } else if (idq > 0){
            encontrarOJPorId(ps, total, idq);
        }
        puts("------------------------------------------\n");

        /* 4–9) Contagens de flags */
        printf("Processos relacionados a violencia domestica: %d\n", contarViolenciaDomestica(ps, total));
        printf("Processos relacionados a feminicidio: %d\n", contarFeminicidio(ps, total));
        printf("Processos relacionadas a causas ambientais: %d\n", contarAmbiental(ps, total));
        printf("Processos relacionadas a comunidades quilombolas: %d\n", contarQuilombolas(ps, total));
        printf("Processos relacionadas a povos indigenas: %d\n", contarIndigenas(ps, total));
        printf("Processos relacionadas a infancia e juventude: %d\n", contarInfancia(ps, total));

        puts("\n------------------------------------------");

        /* 11) Percentual da Meta 1 (streaming para evitar estouro de terminal/memoria) */
        long long s_c = 0, s_j = 0, s_d = 0, s_s = 0;
        double pct = calcularCumprimentoMeta1Streaming("TJDFT_filtrado.csv", 0, &s_c, &s_j, &s_d, &s_s);
        if (pct >= 0.0){
            printf("O percentual de cumprimento da Meta 1 e: %.2f%%\n", pct);
        } else {
            puts("Falha no calculo streaming da Meta 1.");
        }

        puts("------------------------------------------\n");

        /* 12) CSV com julgados (merito) na Meta 1 */
        gerarCSVJulgados(ps, total, "julgado_meta1.csv");
        puts("------------------------------------------\n");
    }

    free(ps);
    print_mm_ss_mmm(TOC_MS(t_total), "tempo TOTAL do programa:");
    printf("\n");
    return 0;
}