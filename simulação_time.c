#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <omp.h> // 1. INCLUIR O CABEÇALHO DA OPENMP

// --- Parâmetros da Simulação ---
#define NX 301       
#define NY 301       
#define NT 3000      
#define NU 0.05     
#define DX (2.0 / (NX - 1)) 
#define DY (2.0 / (NY - 1)) 
#define DT 0.0005   

// --- Função Principal ---
int main() {
    static float u[NY][NX];
    static float v[NY][NX];
    static float un[NY][NX]; 
    static float vn[NY][NX]; 
    
    // Variáveis para medição de tempo
    double start_time, end_time;

   

    // --- Condição Inicial: Perturbação ---
    int p_start_x = NX / 2 - NX / 8;
    int p_end_x   = NX / 2 + NX / 8;
    int p_start_y = NY / 2 - NY / 8;
    int p_end_y   = NY / 2 + NY / 8;

    for (int j = p_start_y; j < p_end_y; j++) {
        for (int i = p_start_x; i < p_end_x; i++) {
            u[j][i] = 2.0f;
            v[j][i] = 2.0f;
        }
    }
    
    printf("Iniciando a simulação em C...\n");

    // ===================================================================
    // 2. CAPTURAR O TEMPO INICIAL
    start_time = omp_get_wtime();

     // --- Inicialização do Grid ---
    for (int j = 0; j < NY; j++) {
        for (int i = 0; i < NX; i++) {
            u[j][i] = 0.0f;
            v[j][i] = 0.0f;
        }
    }

    // --- Loop Principal da Simulação ---
    for (int n = 0; n < NT; n++) {
        memcpy(un, u, sizeof(u));
        memcpy(vn, v, sizeof(v));

        for (int j = 1; j < NY - 1; j++) {
            for (int i = 1; i < NX - 1; i++) {
                u[j][i] = un[j][i] + NU * DT / (DX * DX) * (un[j][i+1] - 2 * un[j][i] + un[j][i-1]) +
                                     NU * DT / (DY * DY) * (un[j+1][i] - 2 * un[j][i] + un[j-1][i]);
                
                v[j][i] = vn[j][i] + NU * DT / (DX * DX) * (vn[j][i+1] - 2 * vn[j][i] + vn[j][i-1]) +
                                     NU * DT / (DY * DY) * (vn[j+1][i] - 2 * vn[j][i] + vn[j-1][i]);
            }
        }

        // --- Condições de Contorno ---
        for (int i = 0; i < NX; i++) {
            u[0][i] = 0.0f; u[NY-1][i] = 0.0f;
            v[0][i] = 0.0f; v[NY-1][i] = 0.0f;
        }
        for (int j = 0; j < NY; j++) {
            u[j][0] = 0.0f; u[j][NX-1] = 0.0f;
            v[j][0] = 0.0f; v[j][NX-1] = 0.0f;
        }
    }
    
    // 2. CAPTURAR O TEMPO FINAL
    end_time = omp_get_wtime();
    // ===================================================================

    printf("Simulação concluída.\n");
    
    // 4. Exibir o tempo de processamento
    printf("---------------------------------------------------\n");
    printf("Tempo de processamento (loop principal): %f segundos\n", end_time - start_time);
    printf("---------------------------------------------------\n");

    // --- Salvando os resultados em arquivos ---
    printf("Salvando resultados em u_final.dat e v_final.dat...\n");
    FILE *file_u, *file_v;
    file_u = fopen("u_final.dat", "w");
    file_v = fopen("v_final.dat", "w");

    // (O resto do código para salvar os arquivos permanece o mesmo)
    // ...
    if (file_u == NULL || file_v == NULL) {
        printf("Erro ao abrir arquivos para escrita.\n");
        return 1;
    }

    for (int j = 0; j < NY; j++) {
        for (int i = 0; i < NX; i++) {
            fprintf(file_u, "%f ", u[j][i]);
            fprintf(file_v, "%f ", v[j][i]);
        }
        fprintf(file_u, "\n");
        fprintf(file_v, "\n");
    }

    fclose(file_u);
    fclose(file_v);
    
    printf("Arquivos salvos com sucesso.\n");

    return 0;
}