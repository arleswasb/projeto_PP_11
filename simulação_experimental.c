#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <omp.h>

// --- Parâmetros da Simulação ---
#define NX 801
#define NY 801
#define NT 500
#define NU 0.05
#define DX (2.0 / (NX - 1))
#define DY (2.0 / (NY - 1))
#define DT 0.0001

int main() {
    float (*u)[NX] = malloc(sizeof(float[NY][NX]));
    float (*v)[NX] = malloc(sizeof(float[NY][NX]));
    float (*un)[NX] = malloc(sizeof(float[NY][NX]));
    float (*vn)[NX] = malloc(sizeof(float[NY][NX]));
    
    if (!u || !v || !un || !vn) {
        printf("Falha na alocação de memória!\n");
        return 1;
    }
    
    double start_time, end_time;

    // --- Inicialização do Grid (Paralelizada) ---
    #pragma omp parallel for
    for (int j = 0; j < NY; j++) {
        for (int i = 0; i < NX; i++) {
            u[j][i] = 1.0f; // Inicializa com velocidade constante
            v[j][i] = 1.0f;
        }
    }
    
    // --- Cria uma pequena perturbação para observar a difusão ---
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

    printf("Iniciando a simulação... Grid: %dx%d, Passos de tempo: %d\n", NX, NY, NT);
    
    start_time = omp_get_wtime();

    // --- Loop Principal da Simulação ---
    for (int n = 0; n < NT; n++) {
        memcpy(un, u, sizeof(float[NY][NX]));
        memcpy(vn, v, sizeof(float[NY][NX]));

        // ### MODIFIQUE A LINHA ABAIXO PARA OS TESTES ###
        #pragma omp parallel for collapse(2) schedule(static)
        for (int j = 1; j < NY - 1; j++) {
            for (int i = 1; i < NX - 1; i++) {
                u[j][i] = un[j][i] + NU * DT / (DX * DX) * (un[j][i+1] - 2 * un[j][i] + un[j][i-1]) +
                                     NU * DT / (DY * DY) * (un[j+1][i] - 2 * un[j][i] + un[j-1][i]);
                
                v[j][i] = vn[j][i] + NU * DT / (DX * DX) * (vn[j][i+1] - 2 * vn[j][i] + vn[j][i-1]) +
                                     NU * DT / (DY * DY) * (vn[j+1][i] - 2 * vn[j][i] + vn[j-1][i]);
            }
        }
    }
    
    end_time = omp_get_wtime();
    
    printf("Simulação concluída.\n");
    printf("---------------------------------------------------\n");
    printf("Tempo de processamento: %f segundos\n", end_time - start_time);
    printf("---------------------------------------------------\n");
    
    free(u); free(v); free(un); free(vn);
    return 0;
}