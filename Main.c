// Arquivo: main.c
// Programa principal para executar as análises

#include <stdio.h>
#include <stdlib.h>
#include "processo.h"

int main() {
    int totalProcessos = 0;
    Processo *processos = lerDados("TJDFT_amostra.csv", &totalProcessos);

    if (totalProcessos > 0) {
        printf("------------------------------------------\n");
        printf("Analise de Dados do Tribunal de Justica do Distrito Federal\n");
        printf("------------------------------------------\n");
        
        // 1. O numero de processos presentes na base de dados
        printf("Total de processos lidos: %d\n", totalProcessos);

        // 2. o id_processo do processo com dt_recebimento mais antigo
        encontrarProcessoMaisAntigo(processos, totalProcessos);
        
        // 3. o id_ultimo_oj a partir de id_processo
        // Exemplo de busca:
        encontrarOJPorId(processos, totalProcessos, 323961063);
        
        // 4-9. Contadores de flags tematicas
        printf("Processos relacionados a violencia domestica: %d\n", contarViolenciaDomestica(processos, totalProcessos));
        printf("Processos relacionados a feminicidio: %d\n", contarFeminicidio(processos, totalProcessos));
        printf("Processos relacionados a causas ambientais: %d\n", contarAmbiental(processos, totalProcessos));
        printf("Processos relacionados a comunidades quilombolas: %d\n", contarQuilombolas(processos, totalProcessos));
        printf("Processos relacionados a povos indigenas: %d\n", contarIndigenas(processos, totalProcessos));
        printf("Processos relacionados a infancia e juventude: %d\n", contarInfancia(processos, totalProcessos));
        
        // 10. o numero de dias entre dt_recebimento e dt_resolvido
        printf("------------------------------------------\n");
        printf("Calculo do tempo de resolucao:\n");
        calcularDiasResolucao(processos, totalProcessos);
        
        // 11. o percentual de cumprimento da meta 1
        printf("------------------------------------------\n");
        calcularCumprimentoMeta1(processos, totalProcessos);
        
        // 12. gerar um arquivo CSV com todos os processos julgados (merito) na Meta 1
        printf("------------------------------------------\n");
        gerarCSVJulgados(processos, totalProcessos, "julgado_meta1.csv");
        
        // Libera a memoria alocada
        free(processos);
    }

    return 0;
}