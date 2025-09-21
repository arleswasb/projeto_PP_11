from fileinput import filename
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from mpl_toolkits.mplot3d import Axes3D
import os
import glob

class FluidVisualizer:
    def __init__(self):
        self.fig = None
        self.ax = None
        
    def load_2D_field(self, filename):
        """Carrega dados de campo 2D - versão debug"""
        print(f"Carregando: {filename}")
        
        # Mostra as primeiras linhas do arquivo
        with open(filename, 'r') as f:
            for i, line in enumerate(f):
                if i < 5:  # Mostra primeiras 5 linhas
                    print(f"Linha {i}: {line.strip()}")
                if i == 5:
                    print("...")
                    break
        
        # Tenta carregar de forma robusta
        try:
            data = []
            with open(filename, 'r') as f:
                for line in f:
                    if line.strip() and not line.startswith('X'):
                        values = line.split()
                        if len(values) == 3:
                            data.append([float(v) for v in values])
            
            data = np.array(data)
            print(f"Dados carregados: shape {data.shape}")
            
            x = data[:, 0].reshape(NX, NY)
            y = data[:, 1].reshape(NX, NY)
            z = data[:, 2].reshape(NX, NY)
            return x, y, z
            
        except Exception as e:
            print(f"Erro detalhado: {e}")
            raise
    
    def load_vector_field(self, filename):
        """Carrega dados de campo vetorial"""
        data = np.loadtxt(filename, skiprows=1)
        return data
    
    def load_energy_history(self, filename):
        """Carrega histórico de energia"""
        data = np.loadtxt(filename, skiprows=1)
        return data[:, 0], data[:, 1]
    
    def plot_2D_field(self, step, field_type='u'):
        """Plota campo 2D com heatmap"""
        filename = f"{field_type}_field_step{step}.dat"
        if not os.path.exists(filename):
            print(f"Arquivo {filename} não encontrado!")
            return
        
        x, y, z = self.load_2D_field(filename)
        
        plt.figure(figsize=(10, 8))
        plt.imshow(z, cmap='hot', origin='lower', extent=[0, NX, 0, NY])
        plt.colorbar(label=f'Velocidade {field_type.upper()}')
        plt.title(f'Campo de Velocidade {field_type.upper()} - Step {step}')
        plt.xlabel('Posição X')
        plt.ylabel('Posição Y')
        plt.tight_layout()
        plt.savefig(f'plot_{field_type}_step{step}.png', dpi=300, bbox_inches='tight')
        plt.show()
    
    def plot_3D_surface(self, step, field_type='u'):
        """Plota superfície 3D do campo"""
        filename = f"{field_type}_field_step{step}.dat"
        if not os.path.exists(filename):
            print(f"Arquivo {filename} não encontrado!")
            return
        
        x, y, z = self.load_2D_field(filename)
        
        fig = plt.figure(figsize=(12, 8))
        ax = fig.add_subplot(111, projection='3d')
        
        surf = ax.plot_surface(x, y, z, cmap='viridis', alpha=0.8, 
                              rstride=2, cstride=2, linewidth=0)
        
        ax.set_xlabel('Posição X')
        ax.set_ylabel('Posição Y')
        ax.set_zlabel(f'Velocidade {field_type.upper()}')
        ax.set_title(f'Superfície 3D - {field_type.upper()} - Step {step}')
        
        fig.colorbar(surf, ax=ax, shrink=0.5, aspect=20)
        plt.savefig(f'3d_{field_type}_step{step}.png', dpi=300, bbox_inches='tight')
        plt.show()
    
    def plot_vector_field(self, step):
        """Plota campo vetorial com setas"""
        filename = f"vector_field_step{step}.dat"
        if not os.path.exists(filename):
            print(f"Arquivo {filename} não encontrado!")
            return
        
        data = self.load_vector_field(filename)
        x, y, u, v, magnitude = data.T
        
        plt.figure(figsize=(10, 8))
        plt.quiver(x, y, u, v, magnitude, cmap='jet', 
                  scale=20, scale_units='inches', width=0.002)
        plt.colorbar(label='Magnitude da Velocidade')
        plt.title(f'Campo Vetorial - Step {step}')
        plt.xlabel('Posição X')
        plt.ylabel('Posição Y')
        plt.xlim(0, NX)
        plt.ylim(0, NY)
        plt.tight_layout()
        plt.savefig(f'vector_field_step{step}.png', dpi=300, bbox_inches='tight')
        plt.show()
    
    def plot_central_profile(self, step):
        """Plota perfil central do campo"""
        filename = f"central_profile_step{step}.dat"
        if not os.path.exists(filename):
            print(f"Arquivo {filename} não encontrado!")
            return
        
        data = np.loadtxt(filename, skiprows=1)
        position, u, v, magnitude = data.T
        
        plt.figure(figsize=(12, 6))
        plt.plot(position, u, 'r-', label='Componente U (X)', linewidth=2)
        plt.plot(position, v, 'b-', label='Componente V (Y)', linewidth=2)
        plt.plot(position, magnitude, 'g-', label='Magnitude', linewidth=2)
        
        plt.title(f'Perfil Central - Step {step}')
        plt.xlabel('Posição Y (corte em X = NX/2)')
        plt.ylabel('Velocidade')
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        plt.savefig(f'central_profile_step{step}.png', dpi=300, bbox_inches='tight')
        plt.show()
    
    def plot_energy_history(self):
        """Plota evolução temporal da energia"""
        filename = "energy_history.dat"
        if not os.path.exists(filename):
            print(f"Arquivo {filename} não encontrado!")
            return
        
        steps, energy = self.load_energy_history(filename)
        
        plt.figure(figsize=(10, 6))
        plt.plot(steps, energy, 'b-o', linewidth=2, markersize=4)
        plt.title('Evolução Temporal da Energia Cinética')
        plt.xlabel('Step')
        plt.ylabel('Energia Cinética')
        plt.grid(True, alpha=0.3)
        plt.tight_layout()
        plt.savefig('energy_history.png', dpi=300, bbox_inches='tight')
        plt.show()
    
    def create_animation(self, field_type='u'):
        """Cria animação da evolução temporal"""
        files = sorted(glob.glob(f"{field_type}_field_step*.dat"))
        if not files:
            print(f"Nenhum arquivo encontrado para {field_type}_field_step*.dat")
            return
        
        # Carregar primeiro frame para inicializar
        x, y, z0 = self.load_2D_field(files[0])
        
        fig, ax = plt.subplots(figsize=(10, 8))
        im = ax.imshow(z0, cmap='hot', origin='lower', 
                      extent=[0, NX, 0, NY], animated=True)
        plt.colorbar(im, label=f'Velocidade {field_type.upper()}')
        ax.set_title(f'Evolução do Campo {field_type.upper()}')
        ax.set_xlabel('Posição X')
        ax.set_ylabel('Posição Y')
        
        def update(frame):
            x, y, z = self.load_2D_field(files[frame])
            im.set_array(z)
            ax.set_title(f'Evolução do Campo {field_type.upper()} - Step {frame*OUTPUT_EVERY}')
            return [im]
        
        ani = FuncAnimation(fig, update, frames=len(files), 
                          interval=200, blit=True)
        
        # Salvar animação
        ani.save(f'animation_{field_type}.gif', writer='pillow', fps=5)
        plt.close()
        print(f"Animação salva como animation_{field_type}.gif")
    
    def plot_comparison(self, steps_to_compare):
        """Plota comparação entre diferentes steps"""
        plt.figure(figsize=(15, 10))
        
        for i, step in enumerate(steps_to_compare, 1):
            filename = f"u_field_step{step}.dat"
            if not os.path.exists(filename):
                continue
                
            x, y, z = self.load_2D_field(filename)
            
            plt.subplot(2, 3, i)
            plt.imshow(z, cmap='viridis', origin='lower', extent=[0, NX, 0, NY])
            plt.colorbar()
            plt.title(f'Step {step}')
            plt.xlabel('X')
            plt.ylabel('Y')
        
        plt.suptitle('Comparação da Evolução Temporal', fontsize=16)
        plt.tight_layout()
        plt.savefig('comparison.png', dpi=300, bbox_inches='tight')
        plt.show()
    
    def run_all_visualizations(self):
        """Executa todas as visualizações"""
        print("Iniciando visualizações...")
        
        # Lista de steps disponíveis
        u_files = glob.glob("u_field_step*.dat")
        steps = sorted([int(f.split('step')[1].split('.')[0]) for f in u_files])
        
        if not steps:
            print("Nenhum arquivo de dados encontrado!")
            return
        
        print(f"Steps disponíveis: {steps}")
        
        # Plotar energia
        self.plot_energy_history()
        
        # Plotar para alguns steps representativos
        representative_steps = [steps[0], steps[len(steps)//2], steps[-1]]
        
        for step in representative_steps:
            print(f"Processando step {step}...")
            self.plot_2D_field(step, 'u')
            self.plot_2D_field(step, 'v')
            self.plot_3D_surface(step, 'u')
            self.plot_vector_field(step)
            self.plot_central_profile(step)
        
        # Comparação entre steps
        self.plot_comparison(representative_steps)
        
        # Criar animações
        print("Criando animações...")
        self.create_animation('u')
        self.create_animation('v')
        
        print("Visualizações concluídas!")

# Parâmetros (devem coincidir com o código C)
NX = 256
NY = 256
OUTPUT_EVERY = 100

# Executar visualizações
if __name__ == "__main__":
    visualizer = FluidVisualizer()
    
    # Menu interativo
    while True:
        print("\n" + "="*50)
        print("VISUALIZADOR DE SIMULAÇÃO DE FLUIDO")
        print("="*50)
        print("1 - Plotar campo 2D")
        print("2 - Plotar superfície 3D")
        print("3 - Plotar campo vetorial")
        print("4 - Plotar perfil central")
        print("5 - Plotar histórico de energia")
        print("6 - Criar animação")
        print("7 - Executar todas as visualizações")
        print("8 - Sair")
        
        choice = input("\nEscolha uma opção: ")
        
        try:
            if choice == '1':
                step = int(input("Digite o step: "))
                field = input("Campo (u/v): ").lower()
                visualizer.plot_2D_field(step, field)
                
            elif choice == '2':
                step = int(input("Digite o step: "))
                field = input("Campo (u/v): ").lower()
                visualizer.plot_3D_surface(step, field)
                
            elif choice == '3':
                step = int(input("Digite o step: "))
                visualizer.plot_vector_field(step)
                
            elif choice == '4':
                step = int(input("Digite o step: "))
                visualizer.plot_central_profile(step)
                
            elif choice == '5':
                visualizer.plot_energy_history()
                
            elif choice == '6':
                field = input("Campo para animação (u/v): ").lower()
                visualizer.create_animation(field)
                
            elif choice == '7':
                visualizer.run_all_visualizations()
                
            elif choice == '8':
                print("Saindo...")
                break
                
            else:
                print("Opção inválida!")
                
        except Exception as e:
            print(f"Erro: {e}")