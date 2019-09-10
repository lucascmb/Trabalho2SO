#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <iostream>

using namespace std;

typedef struct {
    int id;
    int tempoInicial;
    int tempoPrevisto;
    int prioridade;
    int tempoExecutado;
    int posFila;
    int prioridadeDinamica;
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
        *tempoPrevisto += ((*tarefas) + i)->tempoPrevisto;
    }
    for(i = 0 ; i < numeroDeTarefas ; i++){
        fscanf(arq, "%d", &((*tarefas) + i)->prioridade);
    }

    return 1;

}

void constroiFilaInicial(queue<int>& q, Tarefa * tarefas, int tam){
    for(int i = 0 ; i < tam ; i++){
        if(tarefas[i].tempoInicial == 0){
            q.push(tarefas[i].id);
        }
    }
}

void arrumaVetorSJF(vector<int>& v, Tarefa * tarefas, int tam){
    for(int i = 0 ; i < tam ; i++){
        if(tarefas[i].tempoInicial == 0){ v.push_back(i); }
    }
}

void arrumaTarefas(Tarefa * tarefas, int tam){
    for(int i = 0 ; i < tam ; i++){
        tarefas[i].tempoExecutado = 0;
    }
}

void fifo(Tarefa * tarefas, Resultado * resultados, int n, int tempoTotal){

    int i, tempoExecucao = 0;

    resultados[0].trocaCont = n - 1;

    for(i = 0 ; i < n ; i++){
        resultados[0].medioEspera += (tempoExecucao - tarefas[i].tempoInicial);
        tempoExecucao += tarefas[i].tempoPrevisto;
        resultados[0].medioExec += (tempoExecucao - tarefas[i].tempoInicial);
    }

    resultados[0].medioEspera = resultados[0].medioEspera / n;
    resultados[0].medioExec = resultados[0].medioExec / n;


}

void rr(Tarefa * tarefas, Resultado * resultados, int n, int tempoTotal){

    int tPreemp = 2;
    int i, idAtual, init, tDecor = 0;
    queue<int> q;
    
    constroiFilaInicial(q, tarefas, n);
    arrumaTarefas(tarefas, n);
    init = q.size();
    idAtual = q.front();
    q.pop();
    
    for(i = 1 ; i <= tempoTotal ; i++){
        tDecor++;
        tarefas[idAtual].tempoExecutado++;
        resultados[1].medioEspera += q.size();
        resultados[1].medioExec += q.size() + 1;
        if(init != n){
            for(int j = 0 ; j < n ; j++){
                if(tarefas[j].tempoInicial == i){ q.push(tarefas[j].id); init++; }
            }
        }
        if(tDecor == tPreemp && q.size() > 0){
            resultados[1].trocaCont += 1;
            tDecor = 0;
            if(tarefas[idAtual].tempoExecutado < tarefas[idAtual].tempoPrevisto){
                q.push(idAtual);
            }
            idAtual = q.front();
            q.pop();
        }
        if((tarefas[idAtual].tempoExecutado == tarefas[idAtual].tempoPrevisto) && q.size() > 0 && tDecor < tPreemp){
            tDecor = 0;
            resultados[1].trocaCont += 1;
            idAtual = q.front();
            q.pop();
        }
    }

    resultados[1].medioEspera = resultados[1].medioEspera / n;
    resultados[1].medioExec = resultados[1].medioExec / n;


}

void sjf(Tarefa * tarefas, Resultado * resultados, int n, int tempoTotal){

    int h, idAtual, init, posVetIdAtual;
    vector<int> v;

    arrumaVetorSJF(v, tarefas, n);
    arrumaTarefas(tarefas, n);
    init = v.size();

    idAtual = v.front();    
    for(int value : v){
        if(tarefas[value].tempoPrevisto < tarefas[idAtual].tempoPrevisto){ idAtual = value; posVetIdAtual = value;}
    }
    
    v.erase(v.begin() + posVetIdAtual);
    
    for(int i = 1 ; i <= tempoTotal ; i++){
    
    
        tarefas[idAtual].tempoExecutado++;
        resultados[2].medioEspera += v.size();
        resultados[2].medioExec += v.size() + 1;

        if(init < n){
            for(int j = 0 ; j < n ; j++){
                if(tarefas[j].tempoInicial == i){ v.push_back(j); init++; }
            }
        }

        if(tarefas[idAtual].tempoExecutado == tarefas[idAtual].tempoPrevisto){
            idAtual = v.front();
            h = 0;
            for(int value : v){
                if(tarefas[value].tempoPrevisto < tarefas[idAtual].tempoPrevisto){ idAtual = value; posVetIdAtual = h; }
                h++;
            }
            if(v.size() > 1) { v.erase(v.begin() + posVetIdAtual); }
            else if(v.size() == 1) {v.erase(v.begin()); }
            resultados[2].trocaCont++;
        }

    }

    resultados[2].medioEspera = resultados[2].medioEspera / n;
    resultados[2].medioExec = resultados[2].medioExec / n;


}

void srtf(Tarefa * tarefas, Resultado * resultados, int n, int tempoTotal){

    int h, idAtual, init, posVetIdAtual, muda = -1;
    vector<int> v;

    arrumaVetorSJF(v, tarefas, n);
    arrumaTarefas(tarefas, n);
    init = v.size();

    idAtual = v.front();    

    for(int value : v){
        if(tarefas[value].tempoPrevisto < tarefas[idAtual].tempoPrevisto){ idAtual = value; posVetIdAtual = value;}
    }
    
    v.erase(v.begin() + posVetIdAtual);
    
    for(int i = 1 ; i <= tempoTotal ; i++){

        tarefas[idAtual].tempoExecutado++;
        resultados[3].medioEspera += v.size();
        resultados[3].medioExec += v.size() + 1;

        if(init < n){
            for(int j = 0 ; j < n ; j++){
                if(tarefas[j].tempoInicial == i){ v.push_back(j); init++; }
            }
        }


        if(tarefas[idAtual].tempoExecutado == tarefas[idAtual].tempoPrevisto && v.size() != 0){
            idAtual = v.front();
            h = 0;
            posVetIdAtual = -1;
            for(int value : v){
                if(tarefas[value].tempoPrevisto < tarefas[idAtual].tempoPrevisto){ idAtual = value; posVetIdAtual = h; }
                h++;
            }
            if(posVetIdAtual == -1 && v.size() > 1){
                v.erase(v.begin());
            }
            else{
                if(v.size() > 1) { v.erase(v.begin() + posVetIdAtual); }
                else if(v.size() == 1) {v.erase(v.begin()); }
            }
            resultados[3].trocaCont++;
        }


        h = 0;
        for(int value : v){
            
            if(tarefas[value].tempoPrevisto < (tarefas[idAtual].tempoPrevisto - tarefas[idAtual].tempoExecutado)) { muda = idAtual; idAtual = value; posVetIdAtual = h; }
            h++;

        }

        if(muda != -1){
            v.push_back(muda);
            v.erase(v.begin() + posVetIdAtual);
            muda = -1;
            resultados[3].trocaCont++;
        }

    }

    resultados[3].medioEspera = resultados[3].medioEspera / n;
    resultados[3].medioExec = resultados[3].medioExec / n;


}

void prioc(Tarefa * tarefas, Resultado * resultados, int n, int tempoTotal){

    int h, idAtual, init, posVetIdAtual, proxId;
    vector<int> v;

    arrumaVetorSJF(v, tarefas, n);
    arrumaTarefas(tarefas, n);
    init = v.size();

    idAtual = v.front();    

    for(int value : v){
        if(tarefas[value].prioridade > tarefas[idAtual].prioridade){proxId = idAtual; idAtual = value; posVetIdAtual = value;}
    }
    
    v.erase(v.begin() + posVetIdAtual);
    
    for(int i = 1 ; i <= tempoTotal ; i++){
    
        tarefas[idAtual].tempoExecutado++;
        resultados[4].medioEspera += v.size();
        resultados[4].medioExec += v.size() + 1;

        if(init < n){
            for(int j = 0 ; j < n ; j++){
                if(tarefas[j].tempoInicial == i){ 
                    v.push_back(j); init++;
                    if(tarefas[j].prioridade > tarefas[proxId].prioridade){
                        proxId = j;
                    } 
                }
            }
        }


        if(tarefas[idAtual].tempoExecutado == tarefas[idAtual].tempoPrevisto && v.size() > 0){
            idAtual = v.front();
            h = 0;
            for(int value : v){
                if(tarefas[value].prioridade > tarefas[idAtual].prioridade){ idAtual = value; posVetIdAtual = h; }
                h++;
            }

            if(v.size() > 1) { v.erase(v.begin() + posVetIdAtual); }
            else if(v.size() == 1) {v.erase(v.begin()); }
            resultados[4].trocaCont++;
        }


    }

    resultados[4].medioEspera = resultados[4].medioEspera / n;
    resultados[4].medioExec = resultados[4].medioExec / n;



}

void priop(Tarefa * tarefas, Resultado * resultados, int n, int tempoTotal){

    int h, idAtual, init, posVetIdAtual, muda = -1;
    vector<int> v;

    arrumaVetorSJF(v, tarefas, n);
    arrumaTarefas(tarefas, n);
    init = v.size();

    idAtual = v.front();    

    for(int value : v){
        if(tarefas[value].tempoPrevisto < tarefas[idAtual].tempoPrevisto){ idAtual = value; posVetIdAtual = value;}
    }
    
    v.erase(v.begin() + posVetIdAtual);
    
    for(int i = 1 ; i <= tempoTotal ; i++){

        tarefas[idAtual].tempoExecutado++;
        resultados[5].medioEspera += v.size();
        resultados[5].medioExec += v.size() + 1;

        if(init < n){
            for(int j = 0 ; j < n ; j++){
                if(tarefas[j].tempoInicial == i){ 
                    v.push_back(j); 
                    init++; 
                }
            }
        }

        if(tarefas[idAtual].tempoExecutado == tarefas[idAtual].tempoPrevisto){
            idAtual = v.front();
            h = 0;
            posVetIdAtual = -1;
            for(int value : v){
                if(tarefas[value].prioridade > tarefas[idAtual].prioridade){ idAtual = value; posVetIdAtual = h;}
                h++;
            }
            if(posVetIdAtual == -1 && v.size() > 1){
                v.erase(v.begin());
            }
            else{
                if(v.size() > 1) { v.erase(v.begin() + posVetIdAtual); }
                else if(v.size() == 1) {v.erase(v.begin()); }
            }
            if(i != tempoTotal){
                resultados[5].trocaCont++;
            }
        }

        h = 0;
        for(int value : v){
            if(tarefas[value].prioridade > tarefas[idAtual].prioridade) { muda = idAtual; idAtual = value; posVetIdAtual = h;}
            h++;
        }

        if(muda != -1){
            v.erase(v.begin() + posVetIdAtual);
            v.push_back(muda);
            muda = -1;
            resultados[5].trocaCont++;
        }
    }

    resultados[5].medioEspera = resultados[5].medioEspera / n;
    resultados[5].medioExec = resultados[5].medioExec / n;


}

void priod(Tarefa * tarefas, Resultado * resultados, int n, int tempoTotal){

    int h, idAtual, init, posVetIdAtual, muda = -1;
    vector<int> v;

    arrumaVetorSJF(v, tarefas, n);
    arrumaTarefas(tarefas, n);
    init = v.size();

    idAtual = v.front();

    for(int l = 0 ; l < n ; l++){
        tarefas[l].prioridadeDinamica = tarefas[l].prioridade;
    }

    for(int value : v){
        if(tarefas[value].tempoPrevisto < tarefas[idAtual].tempoPrevisto){ idAtual = value; posVetIdAtual = value;}
    }
    
    v.erase(v.begin() + posVetIdAtual);
    
    for(int i = 1 ; i <= tempoTotal ; i++){

        tarefas[idAtual].tempoExecutado++;
        resultados[6].medioEspera += v.size();
        resultados[6].medioExec += v.size() + 1;

        if(init < n){
            for(int j = 0 ; j < n ; j++){
                if(tarefas[j].tempoInicial == i){ 
                    v.push_back(j); 
                    init++; 
                }
            }
        }

        if(tarefas[idAtual].tempoExecutado == tarefas[idAtual].tempoPrevisto){
            idAtual = v.front();
            h = 0;
            posVetIdAtual = -1;
            for(int value : v){
                if(tarefas[value].prioridadeDinamica > tarefas[idAtual].prioridadeDinamica){ idAtual = value; posVetIdAtual = h;}
                h++;
            }
            if(posVetIdAtual == -1 && v.size() > 1){
                v.erase(v.begin());
            }
            else{
                if(v.size() > 1) { v.erase(v.begin() + posVetIdAtual); }
                else if(v.size() == 1) {v.erase(v.begin()); }
            }
            if(i != tempoTotal){
                resultados[6].trocaCont++;

                for(int val : v){
                tarefas[val].prioridadeDinamica++;
                                tarefas[idAtual].prioridadeDinamica = tarefas[idAtual].prioridade;

            }
            }
        }

        h = 0;
        for(int value : v){
            if(tarefas[value].prioridadeDinamica > tarefas[idAtual].prioridadeDinamica) { muda = idAtual; idAtual = value; posVetIdAtual = h;}
            h++;
        }

        if(muda != -1){
            v.erase(v.begin() + posVetIdAtual);
            v.push_back(muda);
            muda = -1;
            resultados[6].trocaCont++;
            
            for(int val : v){
                tarefas[val].prioridadeDinamica++;
                tarefas[idAtual].prioridadeDinamica = tarefas[idAtual].prioridade;
            }
        }
    }

    resultados[6].medioEspera = resultados[6].medioEspera / n;
    resultados[6].medioExec = resultados[6].medioExec / n;


}

int main (int argc, char * argv[]){

    int numeroDeTarefas, i, tempoPrevisto = 0;
    Resultado * resultados;
    FILE *arq;
    Tarefa * tarefas;

    if(argc < 2){
        printf("\nDigite o nome do arquivo de entrada na execução\n\nEx : ./nomedoprograma entrada.txt\n\n"); return -1;
    }

    arq = fopen(argv[1], "r");

    fscanf(arq, "%d", &numeroDeTarefas);

    resultados = (Resultado *) malloc (sizeof(Resultado) * numeroDeTarefas);

    for(i = 0 ; i < numeroDeTarefas ; i++){
        resultados[i].medioEspera = 0;
        resultados[i].medioExec = 0;
        resultados[i].trocaCont = 0;
    }

    if(!criaTarefas(&tarefas, numeroDeTarefas, &tempoPrevisto, arq)) { return -1; }

    fifo(tarefas, resultados, numeroDeTarefas, tempoPrevisto);
    rr(tarefas, resultados, numeroDeTarefas, tempoPrevisto);
    sjf(tarefas, resultados, numeroDeTarefas, tempoPrevisto);
    srtf(tarefas, resultados, numeroDeTarefas, tempoPrevisto);
    prioc(tarefas, resultados, numeroDeTarefas, tempoPrevisto);
    priop(tarefas, resultados, numeroDeTarefas, tempoPrevisto);
    priod(tarefas, resultados, numeroDeTarefas, tempoPrevisto);

    printf("\nAlgoritmo :        FCFS     RR     SJF     SRTF     PRIOc     PRIOp     PRIOd\n");
    printf("Tempo Médio Tt : %5.1f%9.1f%7.1f%8.1f%9.1f%10.1f%10.1f\n", resultados[0].medioExec, resultados[1].medioExec, resultados[2].medioExec, resultados[3].medioExec, resultados[4].medioExec, resultados[5].medioExec, resultados[6].medioExec);
    printf("Tempo Médio Tw : %5.1f%9.1f%7.1f%8.1f%9.1f%10.1f%10.1f\n", resultados[0].medioEspera, resultados[1].medioEspera, resultados[2].medioEspera, resultados[3].medioEspera, resultados[4].medioEspera, resultados[5].medioEspera, resultados[6].medioEspera);
    printf("Troca Contexto : %5.1d%9.1d%7.1d%8.1d%9.1d%10.1d%10.1d\n", resultados[0].trocaCont, resultados[1].trocaCont, resultados[2].trocaCont, resultados[3].trocaCont, resultados[4].trocaCont, resultados[5].trocaCont, resultados[6].trocaCont);
    printf("Tempo Previsto : %5.1d%9.1d%7.1d%8.1d%9.1d%10.1d%10.1d\n\n", tempoPrevisto, tempoPrevisto, tempoPrevisto, tempoPrevisto, tempoPrevisto, tempoPrevisto, tempoPrevisto);
}