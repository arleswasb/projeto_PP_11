#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define NX 256
#define NY 256
#define NT 10000
#define DT 0.01
#define NU 1.00

// NOVO: Função dedicada para salvar o estado do campo vetorial
void save_vector_field(int step, double **u, double **v) {
    char filename[50];
    sprintf(filename, "vector_field_step%d.dat", step);
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Erro ao criar o arquivo %s\n", filename);
        return;
    }
    
    // MODIFICADO: Cabeçalho com as 4 colunas
    fprintf(file, "X Y U_Value V_Value\n");
    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            // MODIFICADO: Escreve as coordenadas, u e v na mesma linha
            fprintf(file, "%d %d %.6f %.6f\n", i, j, u[i][j], v[i][j]);
        }
    }
    fclose(file);
    printf("Salvo o arquivo: %s\n", filename);
}


int main() {
    // Alocar arrays 2D (sem alterações)
    double **u = (double**)malloc(NX * sizeof(double*));
    double **v = (double**)malloc(NX * sizeof(double*));
    double **u_new = (double**)malloc(NX * sizeof(double*));
    double **v_new = (double**)malloc(NX * sizeof(double*));
    
    for (int i = 0; i < NX; i++) {
        u[i] = (double*)malloc(NY * sizeof(double));
        v[i] = (double*)malloc(NY * sizeof(double));
        u_new[i] = (double*)malloc(NY * sizeof(double));
        v_new[i] = (double*)malloc(NY * sizeof(double));
    }
    
    // Inicialização com perturbação (sem alterações)
    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            u[i][j] = 1.0;
            v[i][j] = 0.0;
            double dx = i - NX/2, dy = j - NY/2;
            double dist = sqrt(dx*dx + dy*dy);
            if (dist < 20.0) {
                u[i][j] += 2.0 * exp(-dist*dist/100.0);
                v[i][j] += 1.5 * exp(-dist*dist/100.0);
            }
        }
    }
    
    // MODIFICADO: Salva o estado inicial usando a nova função
    save_vector_field(0, u, v);
    
    double start_time = omp_get_wtime();
    
    // Simulação principal
    for (int step = 1; step <= NT; step++) { // MODIFICADO: laço vai até <= NT
        // Cálculo do Laplaciano (sem alterações)
        for (int i = 1; i < NX-1; i++) {
            for (int j = 1; j < NY-1; j++) {
                double d2u_dx2 = (u[i+1][j] - 2.0*u[i][j] + u[i-1][j]);
                double d2u_dy2 = (u[i][j+1] - 2.0*u[i][j] + u[i][j-1]);
                double d2v_dx2 = (v[i+1][j] - 2.0*v[i][j] + v[i-1][j]);
                double d2v_dy2 = (v[i][j+1] - 2.0*v[i][j] + v[i][j-1]);
                
                u_new[i][j] = u[i][j] + DT * NU * (d2u_dx2 + d2u_dy2);
                v_new[i][j] = v[i][j] + DT * NU * (d2v_dx2 + d2v_dy2);
            }
        }
        
        // Condições de contorno periódicas (sem alterações)
        for (int i = 0; i < NX; i++) {
            u_new[i][0] = u_new[i][NY-2];
            u_new[i][NY-1] = u_new[i][1];
            v_new[i][0] = v_new[i][NY-2];
            v_new[i][NY-1] = v_new[i][1];
        }
        for (int j = 0; j < NY; j++) {
            u_new[0][j] = u_new[NX-2][j];
            u_new[NX-1][j] = u_new[1][j];
            v_new[0][j] = v_new[NX-2][j];
            v_new[NX-1][j] = v_new[1][j];
        }
        
        // Trocar arrays (sem alterações)
        double **temp_u = u;
        double **temp_v = v;
        u = u_new;
        v = v_new;
        u_new = temp_u;
        v_new = temp_v;
        
        // MODIFICADO: Lógica de salvamento simplificada para instantes chave
        if (step % (NT / 5) == 0 && step > 0) {
            save_vector_field(step, u, v);
        }
    }
    
    double end_time = omp_get_wtime();
    printf("Tempo de execução: %.6f segundos\n", end_time - start_time);
    
    // Liberar memória (sem alterações)
    for (int i = 0; i < NX; i++) {
        free(u[i]);
        free(v[i]);
        free(u_new[i]);
        free(v_new[i]);
    }
    free(u);
    free(v);
    free(u_new);
    free(v_new);
    
    return 0;
}