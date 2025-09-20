#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>

// Parâmetros da simulação
#define NX 256       // Tamanho do grid em X
#define NY 256       // Tamanho do grid em Y
#define NT 1000      // Número de passos de tempo
#define DT 0.001     // Passo de tempo
#define NU 0.01      // Viscosidade cinemática
#define OUTPUT_EVERY 100  // Frequência de output

// Estrutura para armazenar o campo de velocidade
typedef struct {
    double u[NX][NY];  // Componente x da velocidade
    double v[NX][NY];  // Componente y da velocidade
} VelocityField;

// Função para inicializar o campo de velocidade
void initialize_velocity(VelocityField *vel, int add_perturbation) {
    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            vel->u[i][j] = 1.0;  // Velocidade constante em x
            vel->v[i][j] = 0.0;  // Velocidade zero em y
            
            // Adicionar pequena perturbação no centro
            if (add_perturbation) {
                double dx = i - NX/2;
                double dy = j - NY/2;
                double dist = sqrt(dx*dx + dy*dy);
                
                if (dist < 10.0) {
                    vel->u[i][j] += 0.5 * exp(-dist*dist/25.0);
                    vel->v[i][j] += 0.3 * exp(-dist*dist/25.0);
                }
            }
        }
    }
}

// Função para aplicar condições de contorno (periódicas)
void apply_boundary_conditions(VelocityField *vel) {
    for (int i = 0; i < NX; i++) {
        vel->u[i][0] = vel->u[i][NY-2];
        vel->u[i][NY-1] = vel->u[i][1];
        vel->v[i][0] = vel->v[i][NY-2];
        vel->v[i][NY-1] = vel->v[i][1];
    }
    
    for (int j = 0; j < NY; j++) {
        vel->u[0][j] = vel->u[NX-2][j];
        vel->u[NX-1][j] = vel->u[1][j];
        vel->v[0][j] = vel->v[NX-2][j];
        vel->v[NX-1][j] = vel->v[1][j];
    }
}

// Função para calcular a evolução temporal usando diferenças finitas
void evolve_velocity(VelocityField *vel, VelocityField *vel_new) {
    double du_dx, du_dy, dv_dx, dv_dy;
    double d2u_dx2, d2u_dy2, d2v_dx2, d2v_dy2;
    
    for (int i = 1; i < NX-1; i++) {
        for (int j = 1; j < NY-1; j++) {
            // Derivadas espaciais de primeira ordem (diferenças centradas)
            du_dx = (vel->u[i+1][j] - vel->u[i-1][j]) / (2.0);
            du_dy = (vel->u[i][j+1] - vel->u[i][j-1]) / (2.0);
            dv_dx = (vel->v[i+1][j] - vel->v[i-1][j]) / (2.0);
            dv_dy = (vel->v[i][j+1] - vel->v[i][j-1]) / (2.0);
            
            // Derivadas espaciais de segunda ordem (diferenças centradas)
            d2u_dx2 = (vel->u[i+1][j] - 2.0*vel->u[i][j] + vel->u[i-1][j]) / (1.0);
            d2u_dy2 = (vel->u[i][j+1] - 2.0*vel->u[i][j] + vel->u[i][j-1]) / (1.0);
            d2v_dx2 = (vel->v[i+1][j] - 2.0*vel->v[i][j] + vel->v[i-1][j]) / (1.0);
            d2v_dy2 = (vel->v[i][j+1] - 2.0*vel->v[i][j] + vel->v[i][j-1]) / (1.0);
            
            // Equação de Navier-Stokes simplificada (apenas termo viscoso)
            vel_new->u[i][j] = vel->u[i][j] + DT * NU * (d2u_dx2 + d2u_dy2);
            vel_new->v[i][j] = vel->v[i][j] + DT * NU * (d2v_dx2 + d2v_dy2);
        }
    }
}

// Função para calcular a energia cinética total
double calculate_kinetic_energy(VelocityField *vel) {
    double energy = 0.0;
    
    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            energy += 0.5 * (vel->u[i][j]*vel->u[i][j] + vel->v[i][j]*vel->v[i][j]);
        }
    }
    
    return energy;
}

// Função para salvar campo 2D (matriz) para gráficos
void save_2D_field(double field[NX][NY], const char *filename, int step) {
    char full_filename[100];
    sprintf(full_filename, "%s_step%d.dat", filename, step);
    
    FILE *file = fopen(full_filename, "w");
    if (file == NULL) {
        printf("Erro ao abrir arquivo %s\n", full_filename);
        return;
    }
    
    fprintf(file, "X Y Value\n");
    for (int i = 0; i < NX; i++) {
        for (int j = 0; j < NY; j++) {
            fprintf(file, "%d %d %f\n", i, j, field[i][j]);
        }
        fprintf(file, "\n");  // Linha em branco para gnuplot
    }
    
    fclose(file);
}

// Função para salvar campo vetorial 2D para gráficos de setas
void save_vector_field(VelocityField *vel, const char *filename, int step) {
    char full_filename[100];
    sprintf(full_filename, "%s_step%d.dat", filename, step);
    
    FILE *file = fopen(full_filename, "w");
    if (file == NULL) {
        printf("Erro ao abrir arquivo %s\n", full_filename);
        return;
    }
    
    fprintf(file, "X Y U V Magnitude\n");
    for (int i = 0; i < NX; i += 4) {  // Amostrar a cada 4 pontos para não ficar muito denso
        for (int j = 0; j < NY; j += 4) {
            double magnitude = sqrt(vel->u[i][j]*vel->u[i][j] + vel->v[i][j]*vel->v[i][j]);
            fprintf(file, "%d %d %f %f %f\n", i, j, vel->u[i][j], vel->v[i][j], magnitude);
        }
    }
    
    fclose(file);
}

// Função para salvar perfil central (corte no meio do domínio)
void save_central_profile(VelocityField *vel, const char *filename, int step) {
    char full_filename[100];
    sprintf(full_filename, "%s_step%d.dat", filename, step);
    
    FILE *file = fopen(full_filename, "w");
    if (file == NULL) {
        printf("Erro ao abrir arquivo %s\n", full_filename);
        return;
    }
    
    fprintf(file, "Position U V Magnitude\n");
    int center_x = NX / 2;
    for (int j = 0; j < NY; j++) {
        double magnitude = sqrt(vel->u[center_x][j]*vel->u[center_x][j] + vel->v[center_x][j]*vel->v[center_x][j]);
        fprintf(file, "%d %f %f %f\n", j, vel->u[center_x][j], vel->v[center_x][j], magnitude);
    }
    
    fclose(file);
}

// Função para salvar evolução temporal da energia
void save_energy_history(double energy_history[], int steps, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Erro ao abrir arquivo %s\n", filename);
        return;
    }
    
    fprintf(file, "Step Energy\n");
    for (int step = 0; step < steps; step++) {
        if (step % OUTPUT_EVERY == 0) {
            fprintf(file, "%d %f\n", step, energy_history[step]);
        }
    }
    
    fclose(file);
}

int main() {
    VelocityField vel, vel_new;
    double start_time, end_time;
    double energy_history[NT/OUTPUT_EVERY + 1];
    int energy_index = 0;
    
    printf("Executando versão serializada com saída para gráficos\n");
    
    // Teste 2: Fluido com perturbação inicial (mais interessante para visualização)
    printf("Teste: Fluido com perturbação inicial\n");
    initialize_velocity(&vel, 1);
    apply_boundary_conditions(&vel);
    
    // Salvar estado inicial
    save_2D_field(vel.u, "u_field", 0);
    save_2D_field(vel.v, "v_field", 0);
    save_vector_field(&vel, "vector_field", 0);
    save_central_profile(&vel, "central_profile", 0);
    
    start_time = omp_get_wtime();
    
    for (int step = 0; step < NT; step++) {
        evolve_velocity(&vel, &vel_new);
        apply_boundary_conditions(&vel_new);
        
        // Trocar campos (vel vira vel_new)
        VelocityField temp = vel;
        vel = vel_new;
        vel_new = temp;
        
        if (step % OUTPUT_EVERY == 0) {
            double energy = calculate_kinetic_energy(&vel);
            printf("Step %d: Energia cinética = %f\n", step, energy);
            energy_history[energy_index++] = energy;
            
            // Salvar dados para gráficos em passos específicos
            if (step == 0 || step == NT/4 || step == NT/2 || step == 3*NT/4 || step == NT-1) {
                save_2D_field(vel.u, "u_field", step);
                save_2D_field(vel.v, "v_field", step);
                save_vector_field(&vel, "vector_field", step);
                save_central_profile(&vel, "central_profile", step);
            }
        }
    }
    
    // Salvar estado final
    save_2D_field(vel.u, "u_field", NT-1);
    save_2D_field(vel.v, "v_field", NT-1);
    save_vector_field(&vel, "vector_field", NT-1);
    save_central_profile(&vel, "central_profile", NT-1);
    save_energy_history(energy_history, energy_index, "energy_history.dat");
    
    end_time = omp_get_wtime();
    printf("Tempo de execução: %.3f segundos\n", end_time - start_time);
    
    printf("\nArquivos gerados para visualização:\n");
    printf("- u_field_step*.dat: Campo de velocidade U (componente x)\n");
    printf("- v_field_step*.dat: Campo de velocidade V (componente y)\n");
    printf("- vector_field_step*.dat: Campo vetorial para gráficos de setas\n");
    printf("- central_profile_step*.dat: Perfil central do campo\n");
    printf("- energy_history.dat: Evolução temporal da energia\n");
    
    return 0;
}