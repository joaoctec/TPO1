// Arquivo: processo.c
// Implementação das funções para manipulação dos dados de processos

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "processo.h"

// Função auxiliar para converter uma data em dias desde um ponto de referência
int dataParaDias(const char *dataStr) {
    if (dataStr == NULL || strlen(dataStr) == 0) {
        return -1;
    }
    int ano, mes, dia;
    if (sscanf(dataStr, "%d-%d-%d", &ano, &mes, &dia) == 3) {
        return ano * 365 + mes * 30 + dia;
    }
    return -1;
}

// 1. O número de processos presentes na base de dados
// Função para ler dados de um arquivo CSV e popular um array de structs
Processo *lerDados(const char *nomeArquivo, int *totalProcessos) {
    FILE *fp = fopen(nomeArquivo, "r");
    if (fp == NULL) {
        printf("ERRO: Nao foi possivel abrir o arquivo %s.\n", nomeArquivo);
        exit(ERRO);
    }

    Processo *processos = (Processo *)malloc(MAX_PROCESSOS * sizeof(Processo));
    if (processos == NULL) {
        printf("ERRO: Nao foi possivel alocar memoria para os processos.\n");
        exit(ERRO);
    }

    char linha[TAM_LINHA];
    fgets(linha, TAM_LINHA, fp); // Ignora a linha de cabeçalho

    int i = 0;
    while (fgets(linha, TAM_LINHA, fp)!= NULL && i < MAX_PROCESSOS) {
        char *token;
        char *rest = linha;

        token = strtok(rest, ";\n");
        processos[i].id_processo = (token)? atoll(token) : 0;
        
        token = strtok(NULL, ";\n");
        if (token) strncpy(processos[i].numero_sigilo, token, sizeof(processos[i].numero_sigilo) - 1);
        
        token = strtok(NULL, ";\n");
        if (token) strncpy(processos[i].sigla_grau, token, sizeof(processos[i].sigla_grau) - 1);
        
        token = strtok(NULL, ";\n");
        if (token) strncpy(processos[i].procedimento, token, sizeof(processos[i].procedimento) - 1);
        
        token = strtok(NULL, ";\n");
        if (token) strncpy(processos[i].ramo_justica, token, sizeof(processos[i].ramo_justica) - 1);
        
        token = strtok(NULL, ";\n");
        if (token) strncpy(processos[i].sigla_tribunal, token, sizeof(processos[i].sigla_tribunal) - 1);
        
        token = strtok(NULL, ";\n");
        processos[i].id_tribunal = (token)? atoi(token) : 0;
        
        token = strtok(NULL, ";\n");
        processos[i].recurso = (token)? atoi(token) : 0;
        
        token = strtok(NULL, ";\n");
        processos[i].id_ultimo_oj = (token)? atoi(token) : 0;
        
        token = strtok(NULL, ";\n");
        if (token) strncpy(processos[i].dt_recebimento, token, sizeof(processos[i].dt_recebimento) - 1);
        
        token = strtok(NULL, ";\n");
        processos[i].id_ultima_classe = (token)? atoi(token) : 0;
        
        token = strtok(NULL, ";\n");
        processos[i].flag_violencia_domestica = (token)? atoi(token) : 0;
        
        token = strtok(NULL, ";\n");
        processos[i].flag_feminicidio = (token)? atoi(token) : 0;
        
        token = strtok(NULL, ";\n");
        processos[i].flag_ambiental = (token)? atoi(token) : 0;
        
        token = strtok(NULL, ";\n");
        processos[i].flag_quilombolas = (token)? atoi(token) : 0;
        
        token = strtok(NULL, ";\n");
        processos[i].flag_indigenas = (token)? atoi(token) : 0;
        
        token = strtok(NULL, ";\n");
        processos[i].flag_infancia = (token)? atoi(token) : 0;
        
        token = strtok(NULL, ";\n");
        if (token) strncpy(processos[i].decisao, token, sizeof(processos[i].decisao) - 1);
        
        token = strtok(NULL, ";\n");
        if (token) strncpy(processos[i].dt_resolvido, token, sizeof(processos[i].dt_resolvido) - 1);
        
        token = strtok(NULL, ";\n");
        processos[i].cnm1 = (token)? atoi(token) : 0;
        
        token = strtok(NULL, ";\n");
        processos[i].primeirasentm1 = (token)? atoi(token) : 0;
        
        token = strtok(NULL, ";\n");
        processos[i].baixm1 = (token)? atoi(token) : 0;
        
        token = strtok(NULL, ";\n");
        processos[i].decm1 = (token)? atoi(token) : 0;
        
        token = strtok(NULL, ";\n");
        processos[i].mpum1 = (token)? atoi(token) : 0;
        
        token = strtok(NULL, ";\n");
        processos[i].julgadom1 = (token)? atoi(token) : 0;
        
        token = strtok(NULL, ";\n");
        processos[i].desm1 = (token)? atoi(token) : 0;
        
        token = strtok(NULL, ";\n");
        processos[i].susm1 = (token)? atoi(token) : 0;
        
        i++;
    }

    *totalProcessos = i;
    fclose(fp);
    return processos;
}

// 2. O id_processo do processo com dt_recebimento mais antigo
void encontrarProcessoMaisAntigo(const Processo *processos, int totalProcessos) {
    if (totalProcessos == 0) {
        printf("Nenhum processo encontrado para analise.\n");
        return;
    }

    long long int idMaisAntigo = processos[0].id_processo;
    int dataMaisAntiga = dataParaDias(processos[0].dt_recebimento);

    for (int i = 1; i < totalProcessos; i++) {
        int dataAtual = dataParaDias(processos[i].dt_recebimento);
        if (dataAtual!= -1 && dataAtual < dataMaisAntiga) {
            dataMaisAntiga = dataAtual;
            idMaisAntigo = processos[i].id_processo;
        }
    }
    printf("O id_processo do processo com dt_recebimento mais antigo e: %lld\n", idMaisAntigo);
}

// 3. O id_ultimo_oj a partir de id_processo
void encontrarOJPorId(const Processo *processos, int totalProcessos, long long int id_busca) {
    for (int i = 0; i < totalProcessos; i++) {
        if (processos[i].id_processo == id_busca) {
            printf("O id_ultimo_oj para o processo %lld e: %d\n", id_busca, processos[i].id_ultimo_oj);
            return;
        }
    }
    printf("Processo com id %lld nao encontrado.\n", id_busca);
}

// 4-9. Contadores de flags temáticas
int contarViolenciaDomestica(const Processo *processos, int totalProcessos) {
    int contador = 0;
    for (int i = 0; i < totalProcessos; i++) {
        contador += processos[i].flag_violencia_domestica;
    }
    return contador;
}

int contarFeminicidio(const Processo *processos, int totalProcessos) {
    int contador = 0;
    for (int i = 0; i < totalProcessos; i++) {
        contador += processos[i].flag_feminicidio;
    }
    return contador;
}

int contarAmbiental(const Processo *processos, int totalProcessos) {
    int contador = 0;
    for (int i = 0; i < totalProcessos; i++) {
        contador += processos[i].flag_ambiental;
    }
    return contador;
}

int contarQuilombolas(const Processo *processos, int totalProcessos) {
    int contador = 0;
    for (int i = 0; i < totalProcessos; i++) {
        contador += processos[i].flag_quilombolas;
    }
    return contador;
}

int contarIndigenas(const Processo *processos, int totalProcessos) {
    int contador = 0;
    for (int i = 0; i < totalProcessos; i++) {
        contador += processos[i].flag_indigenas;
    }
    return contador;
}

int contarInfancia(const Processo *processos, int totalProcessos) {
    int contador = 0;
    for (int i = 0; i < totalProcessos; i++) {
        contador += processos[i].flag_infancia;
    }
    return contador;
}

// 10. O número de dias entre dt_recebimento e dt_resolvido
void calcularDiasResolucao(const Processo *processos, int totalProcessos) {
    for (int i = 0; i < totalProcessos; i++) {
        int diasRecebimento = dataParaDias(processos[i].dt_recebimento);
        int diasResolvido = dataParaDias(processos[i].dt_resolvido);

        if (diasRecebimento!= -1 && diasResolvido!= -1) {
            int diferenca = diasResolvido - diasRecebimento;
            printf("Processo %lld: %d dias entre recebimento e resolucao.\n", processos[i].id_processo, diferenca);
        } else {
            printf("Processo %lld: Nao foi possivel calcular a diferenca de dias (dados invalidos).\n", processos[i].id_processo);
        }
    }
}

// 11. O percentual de cumprimento da meta 1
void calcularCumprimentoMeta1(const Processo *processos, int totalProcessos) {
    long long int cnm1_total = 0;
    long long int julgadom1_total = 0;
    long long int desm1_total = 0;
    long long int susm1_total = 0;
    
    for(int i = 0; i < totalProcessos; i++){
        cnm1_total += processos[i].cnm1;
        julgadom1_total += processos[i].julgadom1;
        desm1_total += processos[i].desm1;
        susm1_total += processos[i].susm1;
    }

    long long int denominador = cnm1_total + desm1_total - susm1_total;

    if (denominador == 0) {
        printf("Nao foi possivel calcular a Meta 1, denominador e zero.\n");
        return;
    }
    
    double cumprimento = ((double)julgadom1_total / denominador) * 100.0;
    printf("O percentual de cumprimento da Meta 1 e: %.2f%%\n", cumprimento);
}

// 12. Gerar um arquivo CSV com todos os processos julgados (mérito) na Meta 1
void gerarCSVJulgados(const Processo *processos, int totalProcessos, const char *nomeArquivoSaida) {
    FILE *fp = fopen(nomeArquivoSaida, "w");
    if (fp == NULL) {
        printf("ERRO: Nao foi possivel criar o arquivo %s.\n", nomeArquivoSaida);
        return;
    }
    
    // Escreve o cabeçalho
    fprintf(fp, "id_processo;numero_sigilo;sigla_grau;procedimento;ramo_justica;sigla_tribunal;id_tribunal;recurso;id_ultimo_oj;dt_recebimento;id_ultima_classe;flag_violencia_domestica;flag_feminicidio;flag_ambiental;flag_quilombolas;flag_indigenas;flag_infancia;decisao;dt_resolvido;cnm1;primeirasentm1;baixm1;decm1;mpum1;julgadom1;desm1;susm1\n");

    int count = 0;
    for (int i = 0; i < totalProcessos; i++) {
        if (processos[i].julgadom1 > 0) {
            fprintf(fp, "%lld;%s;%s;%s;%s;%s;%d;%d;%d;%s;%d;%d;%d;%d;%d;%d;%d;%s;%s;%d;%d;%d;%d;%d;%d;%d;%d\n",
                    processos[i].id_processo, processos[i].numero_sigilo, processos[i].sigla_grau,
                    processos[i].procedimento, processos[i].ramo_justica, processos[i].sigla_tribunal,
                    processos[i].id_tribunal, processos[i].recurso, processos[i].id_ultimo_oj,
                    processos[i].dt_recebimento, processos[i].id_ultima_classe,
                    processos[i].flag_violencia_domestica, processos[i].flag_feminicidio,
                    processos[i].flag_ambiental, processos[i].flag_quilombolas,
                    processos[i].flag_indigenas, processos[i].flag_infancia,
                    processos[i].decisao, processos[i].dt_resolvido,
                    processos[i].cnm1, processos[i].primeirasentm1, processos[i].baixm1,
                    processos[i].decm1, processos[i].mpum1, processos[i].julgadom1,
                    processos[i].desm1, processos[i].susm1);
            count++;
        }
    }

    fclose(fp);
    printf("Gerado arquivo '%s' com %d processos julgados (merito).\n", nomeArquivoSaida, count);
}