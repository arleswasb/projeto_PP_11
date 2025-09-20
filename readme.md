Claro. Aqui está um `README.md` completo, escrito em Markdown, com todas as orientações para compilar e utilizar o programa, baseado em nossas conversas.

Você pode salvar este texto em um arquivo chamado `README.md` no mesmo diretório do seu código-fonte.

-----

# Solucionador de Difusão 2D com OpenMP

Este programa realiza uma simulação da difusão de um campo de velocidade em uma grade 2D, utilizando uma implementação paralela com OpenMP para otimização de desempenho. O código resolve a equação da difusão (termo de viscosidade das equações de Navier-Stokes) pelo método das diferenças finitas.

O programa é projetado para ser executado a partir da linha de comando, permitindo a configuração flexível dos parâmetros da simulação, o que o torna ideal para uso em scripts de análise e ambientes de supercomputação.

## Pré-requisitos

Para compilar e executar este programa, você precisará de:

  * Um compilador C com suporte a OpenMP (como o **GCC**).
  * A biblioteca matemática (`math.h`).

## Compilação

Abra um terminal no diretório onde o arquivo `navier_stokes_otm.c` está localizado e execute o seguinte comando para compilar:

```bash
gcc -o navier_stokes_otm navier_stokes_otm.c -fopenmp -lm
```

  * `-o navier_stokes_otm`: Define o nome do arquivo executável de saída.
  * `-fopenmp`: Habilita o suporte à paralelização com OpenMP.
  * `-lm`: Vincula a biblioteca matemática, necessária para a função `exp()`.

## Uso (Execução)

O programa é executado via linha de comando, passando os parâmetros da perturbação inicial como argumentos.

### Sintaxe

```bash
./navier_stokes_otm <raio_sq> <suavidade> <amp_x> <amp_y>
```

### Parâmetros da Linha de Comando

O programa requer exatamente 4 argumentos:

1.  `raio_sq` (double): O **quadrado do raio** da área da perturbação. A perturbação será aplicada em todos os pontos cuja distância ao quadrado do centro da grade seja menor que este valor.

      * *Exemplo: `400.0` corresponde a um raio de 20.*

2.  `suavidade` (double): Controla a **dispersão (ou suavidade)** da perturbação gaussiana. Valores maiores resultam em uma perturbação mais suave e espalhada. Valores menores resultam em uma perturbação mais "pontuda" e concentrada.

      * *Exemplo: `100.0`.*

3.  `amp_x` (double): A **amplitude (força)** da perturbação no componente X do campo de velocidade (`u`).

      * *Exemplo: `2.0`.*

4.  `amp_y` (double): A **amplitude (força)** da perturbação no componente Y do campo de velocidade (`v`).

      * *Exemplo: `1.5`.*

## Exemplos de Uso

**1. Executando com os valores padrão (similares aos originais do código):**

```bash
./navier_stokes_otm 400.0 100.0 2.0 1.5
```

**2. Executando uma simulação com uma perturbação maior, mais suave e com maior intensidade na direção Y:**

```bash
./navier_stokes_otm 900.0 250.0 0.5 3.0
```

Se o programa for executado com um número incorreto de argumentos, ele exibirá uma mensagem de ajuda.

## Saída (Output)

Após a conclusão da simulação, o programa imprime no terminal um único valor de ponto flutuante. Este número representa o **tempo de execução em segundos** do laço principal de cálculo, medido com `omp_get_wtime()`.

Exemplo de saída:

```
1.234567
```