import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# --- Parâmetros (devem ser consistentes com o código C) ---
NX = 81
NY = 81
X_MAX = 2.0
Y_MAX = 2.0

print("Lendo os dados dos arquivos u_final.dat e v_final.dat...")

try:
    # Carrega os dados dos arquivos de texto para arrays NumPy
    u = np.loadtxt('u_final.dat')
    v = np.loadtxt('v_final.dat')
    print("Dados carregados com sucesso.")
except IOError:
    print("Erro: Arquivos 'u_final.dat' ou 'v_final.dat' não encontrados.")
    print("Certifique-se de compilar e executar o programa em C primeiro.")
    exit()

# Verifica se as dimensões estão corretas
if u.shape != (NY, NX) or v.shape != (NY, NX):
    print(f"Erro: As dimensões dos dados lidos {u.shape} não correspondem aos parâmetros esperados ({NY}, {NX}).")
    exit()

# Cria as coordenadas X e Y para o gráfico
x = np.linspace(0, X_MAX, NX)
y = np.linspace(0, Y_MAX, NY)
X, Y = np.meshgrid(x, y)

# Calcula a magnitude do vetor velocidade em cada ponto do grid
# Magnitude = sqrt(u^2 + v^2)
magnitude = np.sqrt(u**2 + v**2)

# --- Criação do Gráfico 3D ---
print("Gerando o gráfico 3D...")
fig = plt.figure(figsize=(12, 8))
ax = fig.add_subplot(111, projection='3d')

# Plota os dados como uma superfície
# A altura (Z) é a magnitude da velocidade.
# cmap='viridis' é um esquema de cores popular.
surf = ax.plot_surface(X, Y, magnitude, cmap='viridis', edgecolor='none')

# Configurações do gráfico
ax.set_title('Magnitude da Velocidade do Fluido no Tempo Final', fontsize=16)
ax.set_xlabel('Eixo X', fontsize=12)
ax.set_ylabel('Eixo Y', fontsize=12)
ax.set_zlabel('Magnitude da Velocidade', fontsize=12)

# Adiciona uma barra de cores para mapear cores para valores
fig.colorbar(surf, shrink=0.5, aspect=5, label='Magnitude')

plt.tight_layout()
plt.show()
print("Gráfico exibido.")