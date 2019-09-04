#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    int tempoInicial;
    int tempoPrevisto;
    int prioridade;
    int tempoExecutado;
    int posFila;
} Tarefa;

typedef struct {
    float medioExec;
    float medioEspera;
    int trocaCont;
} Resultado;

int criaTarefas(Tarefa ** tarefas, int numeroDeTarefas, int * tempoPrevisto, FILE * arq){

    int i = 0;
    
    *tarefas = (Tarefa*) malloc(sizeof(Tarefa) * numeroDeTarefas);

    for(i = 0 ; i < numeroDeTarefas ; i++){
        ((*tarefas) + i)->id = i;
        ((*tarefas) + i)->tempoExecutado = 0;
        fscanf(arq, "%d", &((*tarefas) + i)->tempoInicial);
    }
    for(i = 0 ; i < numeroDeTarefas ; i++){
        fscanf(arq, "%d", &((*tarefas) + i)->tempoPrevisto);
        tempoPrevisto += ((*tarefas) + i)->tempoPrevisto;
    }
    for(i = 0 ; i < numeroDeTarefas ; i++){
        fscanf(arq, "%d", &((*tarefas) + i)->prioridade);
    }

    return 1;

}

int comp (const void * elem1, const void * elem2){

    Tarefa *f = ((Tarefa *) elem1);
    Tarefa *s = ((Tarefa *) elem2);
    
    if(f->tempoInicial > s->tempoInicial) return 1;
    if(f->tempoInicial < s->tempoInicial) return -1;
    return 0;

}

void enfilera(Tarefa * tarefas, int n, int * fila){
    
    int i, j = 0;

    for(i = 0 ; i < n ; i++){
        if( tarefas[i].tempoInicial == 0 ){ fila[j] = tarefas[i].id; j++; }
    }
    
}

void fifo(Tarefa * tarefas, Resultado * resultados, int n, int tempoTotal){

    int i, tempoExecucao = 0;

    resultados[0].trocaCont = n - 1;

    qsort(tarefas, n, sizeof(Tarefa), comp);

    for(i = 0 ; i < n ; i++){
        resultados[0].medioEspera += (tempoExecucao - tarefas[i].tempoInicial);
        tempoExecucao += tarefas[i].tempoPrevisto;
        resultados[0].medioExec += (tempoExecucao - tarefas[i].tempoInicial);
    }

    resultados[0].medioEspera = resultados[0].medioEspera / n;
    resultados[0].medioExec = resultados[0].medioExec / n;

    printf("\nNo FIFO o tempo médio de execução foi de : %.1f, tempo médio de espera : %.1f e número de trocas de contexto : %d\n", resultados[0].medioExec, resultados[0].medioEspera, resultados[0].trocaCont);

}

void rr(Tarefa * tarefas, Resultado * resultados, int n, int tempoTotal){

    int tPreemp = 2;
    int i, fila[n];

    resultados[1].trocaCont = ( tempoTotal / tPreemp );
    
    qsort(tarefas, n, sizeof(Tarefa), comp);
    enfilera(tarefas, n, fila);

    for(i = 0 ; i < tempoTotal ; i++){

    }

}

int main (int argc, char * argv[]){

    int numeroDeTarefas, i, tempoPrevisto = 0;
    Resultado * resultados;
    FILE *arq;
    Tarefa * tarefas;

    if(argc < 1){
        printf("\nDigite o nome do arquivo de entrada na execução\n"); return -1;
    }

    arq = fopen(argv[1], "r");

    fscanf(arq, "%d", &numeroDeTarefas);

    resultados = malloc(sizeof(Resultado) * numeroDeTarefas);

    for(i = 0 ; i < numeroDeTarefas ; i++){
        resultados[i].medioEspera = 0;
        resultados[i].medioExec = 0;
        resultados[i].trocaCont = 0;
    }

    if(criaTarefas(&tarefas, numeroDeTarefas, &tempoPrevisto, arq)){ printf("\n%d Tarefas criadas com sucesso\n", numeroDeTarefas); }

    /*for(i = 0 ; i < numeroDeTarefas ; i++){
        printf("\n A tarefa %d tem tempo de entrada igual a %d, tempo previsto de execução de %d e prioridade de %d\n", tarefas[i].id, tarefas[i].tempoInicial, tarefas[i].tempoPrevisto, tarefas[i].prioridade);
    }*/

    printf("Executando o FIFO\n");

    fifo(tarefas, resultados, numeroDeTarefas, tempoPrevisto);



}