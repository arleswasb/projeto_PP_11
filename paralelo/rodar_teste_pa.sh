#!/bin/bash

# --- PARÂMETROS DE CONFIGURAÇÃO ---
# Nome do seu programa executável
EXECUTAVEL="./navier_stokes_parallel" 

# Lista de números de threads para testar (separados por espaço)
THREADS_LIST="1 2 4 8 16"

# Número de vezes que cada configuração será executada
NUM_REPETICOES=5

# --- LÓGICA DO SCRIPT ---

echo "Iniciando bateria de testes..."
echo "-------------------------------------"

# Laço externo: itera sobre a lista de threads
for thread_count in $THREADS_LIST
do
    echo "Executando com $thread_count thread(s)..."
    
    # Laço interno: repete a execução 5 vezes
    for i in $(seq 1 $NUM_REPETICOES)
    do
        # Imprime a mensagem de status no erro padrão para não poluir a saída de dados
        echo "  -> Repetição $i/$NUM_REPETICOES..." >&2

        # Define a variável de ambiente OMP_NUM_THREADS e executa o programa
        # A saída (o tempo de execução) será impressa no terminal
        OMP_NUM_THREADS=$thread_count $EXECUTAVEL
    done
    echo "-------------------------------------"
done

echo "Bateria de testes concluída."