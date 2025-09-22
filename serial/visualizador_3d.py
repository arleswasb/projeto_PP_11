import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import os

# --- Parâmetros ---
# A lógica em C está gerando 6 snapshots nestes passos:
SNAPSHOT_STEPS = [0, 2000, 4000, 6000, 8000, 10000] # Exemplo para NT=10000

# Escala fixa para os gráficos
SCALE_U_MIN, SCALE_U_MAX = 1.0, 3.0
SCALE_V_MIN, SCALE_V_MAX = 0.0, 1.6
SCALE_MAGNITUDE_MIN, SCALE_MAGNITUDE_MAX = 1.0, 3.0


def load_and_reshape(filename):
    """Carrega um arquivo .dat e redimensiona os dados para formato de grade."""
    try:
        data = np.loadtxt(filename, skiprows=1)
        x, y, u, v = data[:,0], data[:,1], data[:,2], data[:,3]
        nx, ny = len(np.unique(x)), len(np.unique(y))
        return (x.reshape((ny, nx)), y.reshape((ny, nx)),
                u.reshape((ny, nx)), v.reshape((ny, nx)))
    except FileNotFoundError:
        print(f"ERRO: Arquivo '{filename}' não foi encontrado. Pulando este passo.")
        return None, None, None, None


# --- Início da Geração dos Gráficos ---
print("Iniciando geração dos gráficos agrupados em grade 2x3...")

# --- Gráfico 1: Superfícies 3D Agrupadas para o Campo U ---
# MODIFICADO: Cria uma grade de subplots 2x3 e ajusta o tamanho da figura
fig_u, axes_u = plt.subplots(2, 3, figsize=(18, 10), subplot_kw={'projection': '3d'})
fig_u.suptitle('Evolução do Campo de Velocidade U (Superfície 3D)', fontsize=16)

# MODIFICADO: Itera sobre os eixos achatados (de 2D para 1D)
for i, step in enumerate(SNAPSHOT_STEPS):
    ax = axes_u.flatten()[i]
    X, Y, U, V = load_and_reshape(f'vector_field_step{step}.dat')
    if X is not None:
        surf_u = ax.plot_surface(X, Y, U, cmap='viridis', vmin=SCALE_U_MIN, vmax=SCALE_U_MAX)
        ax.set_title(f'Step {step}')
        ax.set_zlim(SCALE_U_MIN, SCALE_U_MAX)

# Oculta eixos extras se houver (não é o nosso caso com 6 gráficos em 2x3)
for i in range(len(SNAPSHOT_STEPS), axes_u.size):
    axes_u.flatten()[i].axis('off')

fig_u.colorbar(surf_u, ax=axes_u.ravel().tolist(), orientation='vertical', fraction=0.015, pad=0.04, label='Velocidade U')
fig_u.savefig('surface_3d_u_grouped_grid.png', dpi=150, bbox_inches='tight')
print("-> Gráfico de superfície 3D para U (grade) foi salvo.")


# --- Gráfico 2: Superfícies 3D Agrupadas para o Campo V ---
# MODIFICADO: Cria uma grade de subplots 2x3
fig_v, axes_v = plt.subplots(2, 3, figsize=(18, 10), subplot_kw={'projection': '3d'})
fig_v.suptitle('Evolução do Campo de Velocidade V (Superfície 3D)', fontsize=16)

for i, step in enumerate(SNAPSHOT_STEPS):
    ax = axes_v.flatten()[i]
    X, Y, U, V = load_and_reshape(f'vector_field_step{step}.dat')
    if X is not None:
        surf_v = ax.plot_surface(X, Y, V, cmap='plasma', vmin=SCALE_V_MIN, vmax=SCALE_V_MAX)
        ax.set_title(f'Step {step}')
        ax.set_zlim(SCALE_V_MIN, SCALE_V_MAX)

for i in range(len(SNAPSHOT_STEPS), axes_v.size):
    axes_v.flatten()[i].axis('off')

fig_v.colorbar(surf_v, ax=axes_v.ravel().tolist(), orientation='vertical', fraction=0.015, pad=0.04, label='Velocidade V')
fig_v.savefig('surface_3d_v_grouped_grid.png', dpi=150, bbox_inches='tight')
print("-> Gráfico de superfície 3D para V (grade) foi salvo.")


# --- Gráfico 3: Mapas de Calor Agrupados ---
# MODIFICADO: Cria uma grade de subplots 2x3
fig_h, axes_h = plt.subplots(2, 3, figsize=(18, 10))
fig_h.suptitle('Evolução da Magnitude da Velocidade (Mapa de Calor)', fontsize=16)

for i, step in enumerate(SNAPSHOT_STEPS):
    ax = axes_h.flatten()[i]
    X, Y, U, V = load_and_reshape(f'vector_field_step{step}.dat')
    if X is not None:
        magnitude = np.sqrt(U**2 + V**2)
        im = ax.pcolormesh(X, Y, magnitude, cmap='hot', vmin=SCALE_MAGNITUDE_MIN, vmax=SCALE_MAGNITUDE_MAX, shading='auto')
        ax.set_title(f'Step {step}')
        ax.axis('scaled')
        ax.set_xlabel('X')
        ax.set_ylabel('Y')

for i in range(len(SNAPSHOT_STEPS), axes_h.size):
    axes_h.flatten()[i].axis('off')

fig_h.colorbar(im, ax=axes_h.ravel().tolist(), orientation='vertical', fraction=0.015, pad=0.04, label='Magnitude')
fig_h.savefig('heatmap_grouped_grid.png', dpi=150, bbox_inches='tight')
print("-> Gráfico de mapa de calor agrupado (grade) foi salvo.")

plt.close('all')
print("\nProcesso concluído.")