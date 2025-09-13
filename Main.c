#include <stdio.h>
#include <stdlib.h>
#include "processo.h"

int main(void){
    int total = 0;
    Processo *processos = lerDados("TJDFT_amostra.csv", &total);

    if (total > 0){
        puts("------------------------------------------");
        puts("Analise de Dados do Tribunal de Justica do Distrito Federal");
        puts("------------------------------------------");

        printf("Total de processos lidos: %d\n", total);

        encontrarProcessoMaisAntigo(processos, total);
        encontrarOJPorId(processos, total, 323961063);

        printf("Processos relacionados a violencia domestica: %d\n", contarViolenciaDomestica(processos, total));
        printf("Processos relacionados a feminicidio: %d\n", contarFeminicidio(processos, total));
        printf("Processos relacionados a causas ambientais: %d\n", contarAmbiental(processos, total));
        printf("Processos relacionados a comunidades quilombolas: %d\n", contarQuilombolas(processos, total));
        printf("Processos relacionados a povos indigenas: %d\n", contarIndigenas(processos, total));
        printf("Processos relacionados a infancia e juventude: %d\n", contarInfancia(processos, total));

        puts("------------------------------------------");
        puts("Calculo do tempo de resolucao:");
        calcularDiasResolucao(processos, total);

        puts("------------------------------------------");
        calcularCumprimentoMeta1(processos, total);

        puts("------------------------------------------");
        gerarCSVJulgados(processos, total, "julgado_meta1.csv");
    }

    free(processos);
    return 0;
}