#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <omp.h>

// --- Parâmetros da Simulação ---
// Aumentei os parâmetros para que a diferença de tempo seja mais perceptível
#define NX 801       
#define NY 801       
#define NT 500      
#define NU 0.05     
#define DX (2.0 / (NX - 1)) 
#define DY (2.0 / (NY - 1)) 
#define DT 0.0001 // Diminuí o DT para manter a estabilidade com o grid maior

// --- Função Principal ---
int main() {
    // Usando alocação dinâmica para grids grandes para evitar estouro de pilha (stack overflow)
    float (*u)[NX] = malloc(sizeof(float[NY][NX]));
    float (*v)[NX] = malloc(sizeof(float[NY][NX]));
    float (*un)[NX] = malloc(sizeof(float[NY][NX]));
    float (*vn)[NX] = malloc(sizeof(float[NY][NX]));
    
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

    start_time = omp_get_wtime();

    // --- Inicialização (pode ser paralelizada também!) ---
    #pragma omp parallel for collapse(2) schedule(dynamic)
    for (int j = 0; j < NY; j++) {
        for (int i = 0; i < NX; i++) {
            u[j][i] = 0.0f;
            v[j][i] = 0.0f;
        }
    }

    for (int n = 0; n < NT; n++) {
        memcpy(un, u, sizeof(float[NY][NX]));
        memcpy(vn, v, sizeof(float[NY][NX]));

        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (int j = 1; j < NY - 1; j++) {
            for (int i = 1; i < NX - 1; i++) {
                u[j][i] = un[j][i] + NU * DT / (DX * DX) * (un[j][i+1] - 2 * un[j][i] + un[j][i-1]) +
                                     NU * DT / (DY * DY) * (un[j+1][i] - 2 * un[j][i] + un[j-1][i]);
                
                v[j][i] = vn[j][i] + NU * DT / (DX * DX) * (vn[j][i+1] - 2 * vn[j][i] + vn[j][i-1]) +
                                     NU * DT / (DY * DY) * (vn[j+1][i] - 2 * vn[j][i] + vn[j-1][i]);
            }
        }

        // --- Condições de Contorno (permanece sequencial, pois é muito rápido) ---
        for (int i = 0; i < NX; i++) {
            u[0][i] = 0.0f; u[NY-1][i] = 0.0f;
            v[0][i] = 0.0f; v[NY-1][i] = 0.0f;
        }
        for (int j = 0; j < NY; j++) {
            u[j][0] = 0.0f; u[j][NX-1] = 0.0f;
            v[j][0] = 0.0f; v[j][NX-1] = 0.0f;
        }
    }
    
    end_time = omp_get_wtime();
    // (O resto do código permanece igual)
    // ...
    printf("Simulação concluída.\n");
    printf("---------------------------------------------------\n");
    printf("Tempo de processamento (loop principal): %f segundos\n", end_time - start_time);
    printf("---------------------------------------------------\n");
    
    // Libera a memória alocada
    free(u); free(v); free(un); free(vn);

    return 0; // Removi a parte de salvar em arquivo para focar no tempo
}