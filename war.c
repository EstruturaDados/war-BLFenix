#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Definicoes de constantes
#define DADO_MAX 6
#define MENU_EXIBIR 1
#define MENU_ATACAR 2
#define MENU_VERIFICAR 3
#define MENU_SAIR 0

// Missoes
#define MISSAO_DESTRUIR_VERDE 1
#define MISSAO_CONQUISTAR_3 2
#define MISSAO_TROPAS_10 3
#define MISSAO_DESTRUIR_AZUL 4
#define MISSAO_MANTER_5 5

typedef struct
{
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

// ---------- Funcoes utilitarias ----------

// Limpa o buffer de entrada
void limparBufferEntrada()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

// Le um inteiro com validacao
int lerInt(const char *mensagem, int min, int max)
{
    int valor;
    printf("%s", mensagem);
    while (scanf("%d", &valor) != 1 || valor < min || valor > max)
    {
        printf("Entrada invalida! Digite um valor entre %d e %d: ", min, max);
        limparBufferEntrada();
    }
    limparBufferEntrada();
    return valor;
}

// Compara strings ignorando maiusculas/minusculas
int compararIgnorandoCase(const char *s1, const char *s2)
{
    while (*s1 && *s2)
    {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2))
            return 0;
        s1++;
        s2++;
    }
    return *s1 == *s2;
}

// Le uma string com remocao do '\n'
void lerString(const char *mensagem, char *buffer, int tamanho)
{
    printf("%s", mensagem);
    fgets(buffer, tamanho, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}

// ---------- Funcoes de Jogo ----------

// Aloca dinamicamente o mapa
Territorio *alocarMapa(int numTerritorios)
{
    Territorio *mapa = (Territorio *)calloc(numTerritorios, sizeof(Territorio));
    if (!mapa)
    {
        printf("Erro ao alocar memoria.\n");
        exit(1);
    }
    return mapa;
}

// Libera memoria do mapa
void liberarMemoria(Territorio *mapa)
{
    free(mapa);
}

// Inicializa territorios com dados do usuario
void inicializarTerritorios(Territorio *mapa, int numTerritorios)
{
    for (int i = 0; i < numTerritorios; i++)
    {
        printf("\nCadastro do territorio %d:\n", i + 1);
        lerString("Nome do territorio: ", mapa[i].nome, sizeof(mapa[i].nome));
        lerString("Cor do exercito: ", mapa[i].cor, sizeof(mapa[i].cor));
        mapa[i].tropas = lerInt("Quantidade de tropas: ", 1, 1000);
    }
}

// Exibe o mapa completo
void exibirMapa(const Territorio *mapa, int numTerritorios)
{
    printf("\n=== Mapa Atual ===\n");
    for (int i = 0; i < numTerritorios; i++)
    {
        printf("Territorio %d: %s | Cor: %s | Tropas: %d\n",
               i + 1, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

// Exibe a missao do jogador
void exibirMissao(int missao)
{
    printf("\n=== Missao Atual ===\n");
    switch (missao)
    {
    case MISSAO_DESTRUIR_VERDE:
        printf("Destruir o exercito Verde\n");
        break;
    case MISSAO_CONQUISTAR_3:
        printf("Conquistar 3 territorios\n");
        break;
    case MISSAO_TROPAS_10:
        printf("Ter pelo menos 10 tropas em um territorio\n");
        break;
    case MISSAO_DESTRUIR_AZUL:
        printf("Conquistar todos os territorios de cor Azul\n");
        break;
    case MISSAO_MANTER_5:
        printf("Manter pelo menos 5 territorios sob seu controle\n");
        break;
    }
}

// Sorteia missao aleatoria garantindo que nao seja contra a cor do jogador
int sortearMissao(const char *corJogador)
{
    int missao;
    do
    {
        missao = (rand() % 5) + 1;
    } while ((missao == MISSAO_DESTRUIR_VERDE && compararIgnorandoCase(corJogador, "Verde")) ||
             (missao == MISSAO_DESTRUIR_AZUL && compararIgnorandoCase(corJogador, "Azul")));
    return missao;
}

// Conta quantos territorios pertencem a determinada cor
int contarTerritoriosPorCor(const Territorio *mapa, int numTerritorios, const char *cor)
{
    int count = 0;
    for (int i = 0; i < numTerritorios; i++)
    {
        if (compararIgnorandoCase(mapa[i].cor, cor))
            count++;
    }
    return count;
}

// Exibe status de um territorio
void exibirStatusTerritorio(const Territorio *t)
{
    printf("%s | Cor: %s | Tropas: %d\n", t->nome, t->cor, t->tropas);
}

// Simula ataque entre dois territorios
void simularAtaque(Territorio *atacante, Territorio *defensor)
{
    if (atacante->tropas <= 0)
    {
        printf("O territorio %s nao tem tropas suficientes para atacar\n", atacante->nome);
        return;
    }

    int dadoAtacante = rand() % DADO_MAX + 1;
    int dadoDefensor = rand() % DADO_MAX + 1;

    printf("\n%s (atacante) rola %d\n", atacante->nome, dadoAtacante);
    printf("%s (defensor) rola %d\n", defensor->nome, dadoDefensor);

    if (dadoAtacante >= dadoDefensor)
    {
        printf("Atacante vence\n");
        defensor->tropas--;
        if (defensor->tropas <= 0)
        {
            printf("Territorio %s foi conquistado por %s! Nova cor: %s\n",
                   defensor->nome, atacante->nome, atacante->cor);
            strcpy(defensor->cor, atacante->cor);
            defensor->tropas = 1;
            atacante->tropas--;
        }
    }
    else
    {
        printf("Defensor resiste. Atacante perde 1 tropa\n");
        atacante->tropas--;
    }

    printf("\nStatus pos-ataque:\n");
    exibirStatusTerritorio(atacante);
    exibirStatusTerritorio(defensor);
}

// Fase de ataque
void faseDeAtaque(Territorio *mapa, int numTerritorios)
{
    int terAtacante = lerInt("Numero do territorio atacante: ", 1, numTerritorios);
    int terDefensor = lerInt("Numero do territorio defensor: ", 1, numTerritorios);

    if (terAtacante == terDefensor)
    {
        printf("Um territorio nao pode atacar a si mesmo\n");
    }
    else if (compararIgnorandoCase(mapa[terAtacante - 1].cor, mapa[terDefensor - 1].cor))
    {
        printf("Nao e possivel atacar um territorio da propria cor\n");
    }
    else
    {
        simularAtaque(&mapa[terAtacante - 1], &mapa[terDefensor - 1]);
    }
}

// Verifica se missao foi cumprida
int verificarVitoria(const Territorio *mapa, int numTerritorios, int missao, const char *corJogador)
{
    int i;
    switch (missao)
    {
    case MISSAO_DESTRUIR_VERDE:
        for (i = 0; i < numTerritorios; i++)
            if (compararIgnorandoCase(mapa[i].cor, "Verde") && mapa[i].tropas > 0)
                return 0;
        return 1;

    case MISSAO_CONQUISTAR_3:
        return contarTerritoriosPorCor(mapa, numTerritorios, corJogador) >= 3;

    case MISSAO_TROPAS_10:
        for (i = 0; i < numTerritorios; i++)
            if (compararIgnorandoCase(mapa[i].cor, corJogador) && mapa[i].tropas >= 10)
                return 1;
        return 0;

    case MISSAO_DESTRUIR_AZUL:
        for (i = 0; i < numTerritorios; i++)
            if (compararIgnorandoCase(mapa[i].cor, "Azul") && mapa[i].tropas > 0)
                return 0;
        return 1;

    case MISSAO_MANTER_5:
        return contarTerritoriosPorCor(mapa, numTerritorios, corJogador) >= 5;
    }
    return 0;
}

// Menu principal
int exibirMenu()
{
    printf("\n=== Menu Principal ===\n");
    printf("1 - Exibir Mapa\n2 - Atacar\n3 - Verificar Missao\n0 - Sair\n");
    int escolha = lerInt("Escolha uma opcao: ", 0, 3);
    return escolha;
}

// ---------- Main ----------
int main()
{
    srand(time(NULL));

    printf("=== Bem-vindo ao WAR - Nivel Mestre ===\n");

    int numTerritorios = lerInt("Quantos territorios deseja cadastrar? ", 3, 20);
    Territorio *mapa = alocarMapa(numTerritorios);

    // Cadastro dos territorios
    inicializarTerritorios(mapa, numTerritorios);

    // Jogador escolhe sua cor
    char corJogador[10];
    lerString("\nDigite a cor do jogador: ", corJogador, sizeof(corJogador));

    // Missao aleatoria
    int missao = sortearMissao(corJogador);
    exibirMissao(missao);

    int opcao;
    do
    {
        opcao = exibirMenu();
        switch (opcao)
        {
        case MENU_EXIBIR:
            exibirMapa(mapa, numTerritorios);
            break;
        case MENU_ATACAR:
            faseDeAtaque(mapa, numTerritorios);
            break;
        case MENU_VERIFICAR:
            if (verificarVitoria(mapa, numTerritorios, missao, corJogador))
            {
                printf("\nParabens! Voce cumpriu a missao\n");
                opcao = MENU_SAIR;
            }
            else
            {
                printf("\nMissao ainda nao cumprida\n");
            }
            break;
        case MENU_SAIR:
            printf("\nObrigado por jogar\n");
            break;
        }
    } while (opcao != MENU_SAIR);

    liberarMemoria(mapa);
    return 0;
}
