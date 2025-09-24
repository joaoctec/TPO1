
# N1 de Estrutura de Dados - Código de Análise de Casos Judiciários do TJDFT em C

Este projeto é um programa em C que analisa um grande volume de dados de processos judiciais do Tribunal de Justiça do Distrito Federal e Territórios (TJDFT) a partir de um arquivo CSV. Ele faz análises quantitativas, gera o relatório de casos por categoria e gera estatísticas de previsão do tempo de resolução, assim ajudando a extrair informações importantes para estudo ou pesquisa. 

## 1. Número de Processos 

A contagem total de processos é realizada diretamente na função `lerDados` em **processo.c**. Essa função lê o arquivo **TJDFT_filtrado.csv** linha por linha e armazena cada processo em um array de struct **Processo**. Uma variável de controle (passada por referência `int *tot`) é incrementada a cada linha lida, retornando o número exato de processos no final da leitura. 

## 2. id_ultimo_oj por id_processo 

A função `encontrarOJPorId` foi criada para resolver este problema. 

- Onde é criada e chamada: Implementada em **processo.c** e chamada no **main.c**.  
- Funcionalidade: Recebe o array de processos, o número total de processos e um `id_processo` fornecido pelo usuário via `scanf`. A função percorre o array até encontrar o `id_processo` correspondente e, em seguida, imprime o valor de `id_ultimo_oj`. 

## 3. id_processo do processo mais antigo 

Este problema é resolvido pela função `encontrarProcessoMaisAntigo`. 

- Onde é criada e chamada: Implementada em **processo.c** e chamada no **main.c**.  
- Funcionalidade: A função percorre o array de processos, comparando a data de recebimento de cada um (`dt_recebimento`). Para fazer essa comparação, ela utiliza a função auxiliar `dataParaDias`, que converte a string de data `"YYYY-MM-DD"` em um número inteiro de dias. O processo com o menor número de dias é o mais antigo. 

## 4-9. Contagem de processos por categoria 

Para cada categoria (violência doméstica, feminicídio, ambiental, quilombolas, indígenas, infância), uma função de contagem específica foi criada: 

- `contarViolenciaDomestica`  
- `contarFeminicidio`  
- `contarAmbiental`  
- `contarQuilombolas`  
- `contarIndigenas`  
- `contarInfancia`  

- Onde são criadas e chamadas: Todas implementadas em **processo.c** e chamadas no **main.c**.  
- Funcionalidade: Cada função percorre o array de processos e soma o valor de um flag binário (ex: `flag_violencia_domestica`). Como esses flags contêm 1 se a condição é verdadeira e 0 se é falsa, a soma direta resulta na contagem total de processos para aquela categoria. 

## 10. Dias entre dt_recebimento e dt_resolvido 

O cálculo da diferença de dias é feito na função `mostrarComparacaoDatasPorId`. 

- Onde é criada e chamada: Implementada em **processo.c** e chamada no **main.c**.  
- Funcionalidade: Similar à busca de `id_ultimo_oj`, esta função recebe o `id_processo` do usuário. Ela localiza o processo e usa `dataParaDias` para converter as datas de recebimento e resolução em inteiros. A diferença entre os dois inteiros é o número de dias. Se as datas forem inválidas ou o processo não estiver resolvido, uma mensagem de erro apropriada é exibida.  

A versão original `calcularDiasResolucao` foi criada para mostrar todas as comparações do processo em outro arquivo CSV, mas por causa do tempo de execução e a dificuldade de se achar um `id_processo` específico decidimos criar uma nova função, que oferece uma forma interativa e mais específica de consulta, sem a necessidade de gerar um novo arquivo. 

## Estruturação do Código e Análise do CSV  

O projeto é estruturado em três arquivos para promover a modularidade e organização. 

- **processo.h:** O cabeçalho. Serve como uma "interface" para o resto do código. Ele define a estrutura de dados **Processo**, que atua como um molde para cada linha do CSV, e declara os protótipos de todas as funções que serão usadas. As constantes `MAX_PROCESSOS`, `TAM_LINHA` e `ERRO` também são definidas aqui.  
- **processo.c:** O arquivo de implementação. É aqui que a "lógica" de cada função declarada em `processo.h` é escrita. Ele inclui funções para leitura de arquivo, análise de dados e cálculos.  
- **main.c:** O arquivo principal. Contém a função `main`, que coordena todas as ações. Ele chama as funções de `processo.c` na ordem correta, interage com o usuário para obter dados de entrada e imprime os resultados. 

## O Struct Processo 

A `struct Processo` foi declarada em **processo.h** para agrupar as 27 colunas de dados de cada processo em uma única variável. Isso melhora a abstração do problema, pois em vez de lidar com 27 variáveis separadas, o programa manipula apenas uma estrutura de dados. 

```c
typedef struct{ 
    long long id_processo; 
    char numero_sigilo[128], sigla_grau[8], procedimento[128], ramo_justica[64], sigla_tribunal[16]; 
    int id_tribunal, recurso, id_ultimo_oj; 
    char dt_recebimento[11]; 
    int id_ultima_classe; 
    int flag_violencia_domestica, flag_feminicidio, flag_ambiental, flag_quilombolas, flag_indigenas, flag_infancia; 
    char decisao[64], dt_resolvido[11]; 
    int cnm1, primeirasentm1, baixm1, decm1, mpum1, julgadom1, desm1, susm1; 
} Processo; 
```

## Leitura e Análise do CSV 

A função `lerDados` em **processo.c** é a chave para a análise do arquivo CSV. 

1. Ela tenta abrir o arquivo (**"TJDFT_filtrado.csv"**).  
2. Lê a linha de cabeçalho e a descarta.  
3. Usa um loop `while` para ler cada linha do arquivo.  
4. Para cada linha, a função auxiliar `split_semicolon` a divide em campos, usando o ponto e vírgula `;` como delimitador.  
5. Os campos são convertidos para o tipo de dado correto (ex: `atoll` para `long long`, `atoi` para `int`) e armazenados em uma struct **Processo**.  
6. A struct preenchida é adicionada a um array dinâmico de **Processo** (`Processo *p`).  

Para ler um arquivo CSV diferente, basta alterar a string `"TJDFT_filtrado.csv"` na chamada da função `lerDados` no **main.c**: 

```c
Processo *ps = lerDados("TJDFT_filtrado.csv", &total); 
```

## Criação do Arquivo Meta e Meta 1  

A Meta 1 do Conselho Nacional de Justiça (CNJ) é um indicador de produtividade que mede a capacidade de um tribunal de julgar mais processos do que os que entram. O cálculo da meta foi implementado no código de duas maneiras: uma em memória e outra por streaming (mais eficiente para arquivos grandes). 

## 11. Percentual de Cumprimento da Meta 1 

A função `calcularCumprimentoMeta1Streaming` é usada para este cálculo. 

- Lógica e Fórmula: A função lê o arquivo CSV sem carregar todos os dados para a memória, o que a torna ideal para arquivos grandes. Ela soma os valores de quatro campos específicos de cada linha:  
  - `julgadom1`: Processos julgados (numerador)  
  - `cnm1`: Processos conhecidos  
  - `desm1`: Processos que foram baixados  
  - `susm1`: Processos suspensos, a fórmula utilizada é:
    
    <img width="331" height="30" alt="Percentual" src="https://github.com/user-attachments/assets/b2192846-c231-43ae-9a1a-d0bee74d5019" />
  
    Especificações do pc do membro João Cesar, utilizados para imagem do tempo de execução:

- Resultado: O índice reflete o percentual de cumprimento da meta. Um valor acima de 100% indica que o tribunal julgou mais processos do que os que entraram. 

## 12. Geração do CSV de Processos Julgados 

A função `gerarCSVJulgados` foi criada para este fim. 

- Funcionalidade: Percorre todo o array de processos em memória. Para cada processo onde o campo `julgadom1` for maior que 0, a função escreve todos os seus dados em um novo arquivo chamado **"julgado_meta1.csv"**.  
- Construção do Cabeçalho: A primeira linha do novo CSV é um cabeçalho que lista todas as colunas, exatamente como no arquivo de origem. Isso garante que o novo arquivo seja facilmente legível por qualquer programa de planilhas ou análise de dados. 

## Teste, Orquestramento e Tempo de Execução  

O grupo de desenvolvimento se dividiu para otimizar o trabalho: 

- João César e Davi Rodrigues: Ficaram responsáveis pelo desenvolvimento e implementação do código principal em **processo.c** e **main.c**.  
- Lucas Daniel, Carlos Augustus e Davi: Ficaram responsáveis pela documentação detalhada e pela análise dos resultados, incluindo a medição de desempenho e a criação do arquivo **processo.h**.  

## Medição do Tempo de Execução 

O tempo de execução do programa é medido usando a biblioteca `<time.h>`.  

- A macro `TIC(v)` armazena o tempo inicial.  
- A macro `TOC_MS(v)` calcula o tempo decorrido em milissegundos.  
- A função `print_mm_ss_mmm` formata o resultado de forma legível (min:seg.ms).

  Tempo de execução do código do computador do integrante João César,
  
    - <img width="340" height="26" alt="image" src="https://github.com/user-attachments/assets/c859c3a5-53fb-4b6e-8466-45c8b729eca7" />

  Especificações do pc do membro João Cesar, utilizados para imagem do  tempo de execução:

    - <img width="667" height="57" alt="image" src="https://github.com/user-attachments/assets/c7002705-c7dc-48bf-bd0c-179010d0fb74" />

  Tempo de execução do código do computador do integrante Lucas Daniel,

    - <img width="382" height="25" alt="image" src="https://github.com/user-attachments/assets/a6b2e9f2-aeaf-446f-9639-1febfe903136" />

  Especificações do pc do membro Lucas Daniel, utilizados para imagem do tempo de execução
  
    - <img width="527" height="42" alt="image" src="https://github.com/user-attachments/assets/b1195561-f195-453b-b61b-7aef01bf180f" />

  Tempo de execução do código do computador da Universidade,

    - <img width="336" height="31" alt="image" src="https://github.com/user-attachments/assets/0a47f7ed-59b6-4458-8029-d39a9d94ae70" />

  Especificações do pc da Universidade, utilizados para imagem do tempo de execução:

    - <img width="460" height="71" alt="image" src="https://github.com/user-attachments/assets/8410ae7b-cf2a-4d07-81bb-3de19d13339b" />

Essa implementação permite medir o tempo total do programa, bem como o tempo de execução de blocos de código específicos, como a leitura de dados. Isso foi crucial para verificar se o programa está rodando de forma eficiente e identificar gargalos de desempenho.  

O esqueleto do código é a estrutura de arquivos e dependências que garante a modularidade: **processo.h** (cabeçalho) define o que as funções fazem, **processo.c** (implementação) define como elas funcionam, e **main.c** (orquestrador) as chama na ordem correta. As constantes e macros, como `MAX_PROCESSOS` e `TIC`, são usadas em diferentes arquivos para manter uma estrutura coesa e consistente.  

## Saída do terminal esperada

<img width="672" height="624" alt="image" src="https://github.com/user-attachments/assets/8f47a172-064e-4e3b-a92a-1c77200c23f4" />

