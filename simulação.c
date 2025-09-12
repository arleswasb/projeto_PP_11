#include <stdio.h>
#include <stdlib.h>
#include <string.h> // Para memcpy

// --- Parâmetros da Simulação ---
#define NX 81       // Número de pontos no grid em x (maior para melhor visualização 3D)
#define NY 81       // Número de pontos no grid em y
#define NT 200      // Número de passos no tempo
#define NU 0.05     // Viscosidade cinemática
#define DX (2.0 / (NX - 1)) // Espaçamento do grid em x
#define DY (2.0 / (NY - 1)) // Espaçamento do grid em y
#define DT 0.0005   // Passo de tempo (menor para garantir estabilidade com grid maior)

// --- Função Principal ---
int main() {
    // Alocação dos grids de velocidade. Usamos alocação estática para simplicidade.
    // Para grids muito grandes, usaríamos malloc.
    static float u[NY][NX];
    static float v[NY][NX];
    static float un[NY][NX]; // Grid temporário para o passo anterior
    static float vn[NY][NX]; // Grid temporário para o passo anterior

    // --- Inicialização do Grid ---
    // Inicializa tudo com velocidade zero
    for (int j = 0; j < NY; j++) {
        for (int i = 0; i < NX; i++) {
            u[j][i] = 0.0f;
            v[j][i] = 0.0f;
        }
    }

    // --- Condição Inicial: Perturbação ---
    // Uma região quadrada no centro recebe uma velocidade inicial.
    int p_start_x = NX / 2 - NX / 8;
    int p_end_x   = NX / 2 + NX / 8;
    int p_start_y = NY / 2 - NY / 8;
    int p_end_y   = NY / 2 + NY / 8;

    for (int j = p_start_y; j < p_end_y; j++) {
        for (int i = p_start_x; i < p_end_x; i++) {
            u[j][i] = 15.0f;//Para uma interferência mais forte (mais energia inicial) 2 para 5
            v[j][i] = 15.0f;//Para uma interferência mais forte (mais energia inicial) 2 para 5
        }
    }
    
    printf("Iniciando a simulação em C...\n");

    // --- Loop Principal da Simulação ---
    for (int n = 0; n < NT; n++) {
        // Copia os valores atuais para os grids temporários
        memcpy(un, u, sizeof(u));
        memcpy(vn, v, sizeof(v));

        // Loop de cálculo principal (aqui é onde a paralelização será aplicada)
        for (int j = 1; j < NY - 1; j++) {
            for (int i = 1; i < NX - 1; i++) {
                u[j][i] = un[j][i] + NU * DT / (DX * DX) * (un[j][i+1] - 2 * un[j][i] + un[j][i-1]) +
                                     NU * DT / (DY * DY) * (un[j+1][i] - 2 * un[j][i] + un[j-1][i]);
                
                v[j][i] = vn[j][i] + NU * DT / (DX * DX) * (vn[j][i+1] - 2 * vn[j][i] + vn[j][i-1]) +
                                     NU * DT / (DY * DY) * (vn[j+1][i] - 2 * vn[j][i] + vn[j-1][i]);
            }
        }

        // --- Condições de Contorno (paredes com velocidade zero) ---
        for (int i = 0; i < NX; i++) {
            u[0][i] = 0.0f; u[NY-1][i] = 0.0f;
            v[0][i] = 0.0f; v[NY-1][i] = 0.0f;
        }
        for (int j = 0; j < NY; j++) {
            u[j][0] = 0.0f; u[j][NX-1] = 0.0f;
            v[j][0] = 0.0f; v[j][NX-1] = 0.0f;
        }
    }

    printf("Simulação concluída.\n");

    // --- Salvando os resultados em arquivos ---
    printf("Salvando resultados em u_final.dat e v_final.dat...\n");
    FILE *file_u, *file_v;
    file_u = fopen("u_final.dat", "w");
    file_v = fopen("v_final.dat", "w");

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