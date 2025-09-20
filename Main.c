/* =========================
 * arquivo: main.c
 * ========================= */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "processo.h"

// Cronômetro simples (CPU time, em milissegundos)
#define TIC(var) clock_t var = clock()
#define TOC_MS(var) (1000.0 * (clock() - (var)) / CLOCKS_PER_SEC)

// Formata milissegundos em MM:SS.mmm
static void print_mm_ss_mmm(double ms, const char *label){
    long long total_ms = (long long)(ms + 0.5); // arredonda
    int mm  = (int)(total_ms / 60000);
    int ss  = (int)((total_ms % 60000) / 1000);
    int mmm = (int)(total_ms % 1000);
    printf("%s %02d:%02d.%03d (min:seg.ms)\n", label, mm, ss, mmm);
}

int main(void){
    int total = 0;

    // 1) Cronômetro do programa inteiro
    TIC(t_total);

    Processo *processos = lerDados("TJDFT_filtrado.csv", &total); // troque o arquivo quando quiser

    if (total > 0){

        puts("------------------------------------------");
        puts("Analise do tempo de resolucao dos processos");
        puts("Calculo do tempo de resolucao:");
        puts("------------------------------------------\n");

        // 2) Cronômetro só do cálculo de datas
        TIC(t_datas);
        // Grave tudo em CSV para não cortar no terminal…
        salvarDiasResolucaoCSV(processos, total, "dias_resolucao.csv");
        // …e mostre só uma amostra no console:
        imprimirDiasResolucaoAmostra(processos, total, 20);

        double ms_datas = TOC_MS(t_datas);
        print_mm_ss_mmm(ms_datas, "\n[tempo] Dias de resolucao:");

        puts("\n------------------------------------------");
        puts("Analise de Dados do Tribunal de Justica do Distrito Federal");
        puts("------------------------------------------\n");

        puts("------------------------------------------");
        printf("Total de processos lidos: %d\n\n", total);

        encontrarProcessoMaisAntigo(processos, total);
        encontrarOJPorId(processos, total, 323961063);

        puts("------------------------------------------\n");

        printf("Processos relacionados a violencia domestica: %d\n", contarViolenciaDomestica(processos, total));
        printf("Processos relacionados a feminicidio: %d\n", contarFeminicidio(processos, total));
        printf("Processos relacionadas a causas ambientais: %d\n", contarAmbiental(processos, total));
        printf("Processos relacionadas a comunidades quilombolas: %d\n", contarQuilombolas(processos, total));
        printf("Processos relacionadas a povos indigenas: %d\n", contarIndigenas(processos, total));
        printf("Processos relacionadas a infancia e juventude: %d\n", contarInfancia(processos, total));

        puts("\n------------------------------------------");
        // Cálculo da Meta 1 em TODOS os anos (cumprimento global, como o professor pediu)
        long long s_cnm1=0, s_julg=0, s_desm=0, s_sus=0;
        double pct_total = calcularCumprimentoMeta1Streaming("TJDFT_filtrado.csv", 0,
                                                            &s_cnm1, &s_julg, &s_desm, &s_sus);
        if (pct_total >= 0.0){
            printf("O percentual de cumprimento da Meta 1 e: %.2f%%\n", pct_total);
        } else {
            puts("Falha no calculo streaming da Meta 1.");
        }

        puts("------------------------------------------\n");
        gerarCSVJulgados(processos, total, "julgado_meta1.csv");
        puts("------------------------------------------\n");
    }

    free(processos);

    // Final do cronômetro total
    double ms_total = TOC_MS(t_total);
    print_mm_ss_mmm(ms_total, "[tempo] TOTAL do programa:");
    printf("\n");

    return 0;
}