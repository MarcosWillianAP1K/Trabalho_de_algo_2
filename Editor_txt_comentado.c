#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

// defines
#define type "type"
#define undo "undo"
#define nome_ent "entrada.txt"
#define nome_sai "saida.txt"
#define numero_de_alocacao 50

// variaveis globais
FILE *ent1, *ent2, *sai;
char comando[5], caracter;
int cont_exemplos = 1;

// funcao para inverter o texto
void inverter(int inicio, int fim, char *string)
{
    // aux
    char aux;

    // loop para inverter o texto
    for (int i = 0; i < ((fim - inicio)); i++)
    {
        // troca de posicao
        aux = string[inicio];
        string[inicio] = string[fim - i - 1];
        string[fim - i - 1] = aux;
        // incremento do inicio
        inicio++;
    }
}

// funcao para jogar no arquivo de saida
void jogar_no_arq_saida(char *texto)
{
    // inverter o texto
    inverter(0, strlen(texto), texto);

    // jogar no arquivo de saida
    fprintf(sai, "%s\n", texto);
    printf("%s\n", texto);
}

// funcao para realocar memoria
void realocacao(char *texto, int n)
{
    // variavel para verificar se a realocacao foi feita
    bool teste = true;

    // loop para realocar memoria
    do
    {
        // realocar memoria
        char *aux = realloc(texto, (sizeof(char) * (n * numero_de_alocacao)));

        // verificar se a realocacao foi feita
        if (aux)
        {
            // atribuir o texto
            texto = aux;
            teste = false;
        }
        else
        {
            printf("\nrealoc final Deu ruim\n");
        }
    } while (teste);
}

// funcao para verificar se o ponteiro esta no final do comandos
bool verificar_se_esta_no_final(FILE *ponteiro)
{
    // buffer
    char aux;

    // mover o ponteiro para tras 1 caracter
    fseek(ponteiro, sizeof(char) * -1, SEEK_CUR);

    // verificar se o caracter é um {
    if ((aux = fgetc(ponteiro)) != EOF && aux == '{')
    {
        // se for, retorna true
        return true;
    }

    // se nao, retorna false
    return false;
}

// funcao para mover o ponteiro para tras
void mover_tras(FILE *ponteiro)
{
    // buffer
    char aux;

    // mover o ponteiro 2 caracters para tras
    fseek(ponteiro, sizeof(char) * -2, SEEK_CUR);

    // loop para verificar se o caracter é um { ou , simbolizando o inicio de um comando
    while ((aux = fgetc(ponteiro)) != EOF && aux != '{' && aux != ',')
    {
        // mover o ponteiro 2 caracters para tras
        fseek(ponteiro, sizeof(char) * -2, SEEK_CUR);
    }
}

// funcao para pegar o caracter
void pegar_caracter(char *texto)
{
    // buffer
    char aux;
    // guarda o inicio do texto
    int inicio = strlen(texto);

    // mover para frente um caracter
    caracter = fgetc(ent1);

    // loop para pegar o caracter, enquanto nao for o final do texto ira pegar o caracter
    while ((caracter = fgetc(ent1)) != EOF && caracter != ',' && caracter != '}')
    {
        // tamanho do texto
        int tam = strlen(texto);

        // se o tamanho do texto for multiplo do numero de alocacao, ele devera alocar mais memoria
        if (tam % numero_de_alocacao == 0 && tam > 0)
        {
            // realocar memoria
            realocacao(texto, (tam / numero_de_alocacao) + 1);
        }

        // adicionar o caracter no texto
        texto[tam] = caracter;
        // adicionar o final do texto
        texto[tam + 1] = '\0';
    }

    // se o inicio for diferente do final do texto menos um, ou seja, foi colocado mais de um caracter
    if (inicio != strlen(texto) - 1)
    {
        // inverter apenas os caracteres que foram adicionados
        inverter(inicio, strlen(texto), texto);
    }
}

// pegar o segundo do comando
void pegar_numero(FILE *ponteiro, int *num)
{
    // zerar o numero
    *num = 0;

    // pega caracter por caracter, faz o loop enquanto for um digito
    while ((caracter = fgetc(ponteiro)) != EOF && isdigit(caracter))
    {
        // faz a conversao do caracter para inteiro
        *num *= 10;
        *num += caracter - '0';
    }
}

// funcao para configurar a posilção iniciais dos ponteiros
void configurar_ponteiros()
{
    // buffer
    char aux;

    // pecorrer o arquivo ate achar o primeiro }
    while ((aux = fgetc(ent1)) != EOF && aux != '}')
        ;

    // pecorrer o arquivo ate achar o primeiro }
    while ((aux = fgetc(ent2)) != EOF && aux != '}')
        ;

    // pecorrer o arquivo ate achar o primeiro }
    while ((aux = fgetc(ent2)) != EOF && aux != '}')
        ;

    mover_tras(ent1);
    mover_tras(ent2);
}

// funcao para configurar os ponteiros proximo exemplo
void configurar_proximo()
{
    // buffer
    char aux;

    // pecorrer o arquivo ate achar o primeiro {
    while ((aux = fgetc(ent1)) != EOF && aux != '{')
        ;

    // pecorrer o arquivo ate achar o primeiro {
    while ((aux = fgetc(ent1)) != EOF && aux != '{')
        ;

    // pecorrer o arquivo ate achar o primeiro {
    while ((aux = fgetc(ent2)) != EOF && aux != '{')
    {
        // verificar se o caracter é um \n
        if (aux == '\n')
        {
            // incrementar o contador de exemplos
            cont_exemplos++;
        }
    }

    // mover os ponteiros para tras 1 caracter
    fseek(ent1, sizeof(char) * -1, SEEK_CUR);
    fseek(ent2, sizeof(char) * -1, SEEK_CUR);
}

// funcao para desfazer o comando
bool desfazer(int *num_undo, int *num_seg)
{
    // variavel para o segundo do comando atual
    int num_atual = 0;
    // mover os ponteiros para tras
    mover_tras(ent1);
    mover_tras(ent2);
    // pegar o numero do comando
    pegar_numero(ent2, &num_atual);
    mover_tras(ent2);

    // verificar se o segundo do comando atual for menor ou igual ao segundo do comando undo menos o numero do undo, o loop acontece
    while (*num_seg - num_atual - 1 <= num_atual)
    {
        // verificar se esta no final do arquivo
        if (verificar_se_esta_no_final(ent1) == true)
        {
            // se estiver no final dos comandos, retorna, fechando a funcao
            return true;
        }
        // mover os ponteiros para tras
        mover_tras(ent1);
        mover_tras(ent2);
        // pegar o numero do comando
        pegar_numero(ent2, &num_atual);
        mover_tras(ent2);
    }

    return false;
}

// funcao para pecorrer o arquivo
void pecorrer_arquivo(char *texto)
{
    // loop para pecorrer o arquivo e ler o comando
    while (fgets(comando, sizeof(comando), ent1) && (strcmp(comando, type) == 0 || strcmp(comando, undo) == 0))
    {
        // variavel do segundo do comando
        int num_seg = 0;

        // pegar o segundo do comando
        pegar_numero(ent2, &num_seg);

        // verificar se o comando é type
        if (strcmp(comando, type) == 0)
        {
            // pegar o caracter do type
            pegar_caracter(texto);

            // mover os ponteiros para tras
            mover_tras(ent1);
            mover_tras(ent2);

            // verificar se esta no final do arquivo
            if (verificar_se_esta_no_final(ent1) == true)
            {
                // se estiver no final dos comandos, retorna, fechando a funcao
                return;
            }
            // mover os ponteiros para tras
            mover_tras(ent1);
            mover_tras(ent2);
        }
        // verificar se o comando é undo
        if (strcmp(comando, undo) == 0)
        {
            // variavel para o numero do comando undo
            int num_undo = 0;
            // move o ponteiro para frente um caracter
            caracter = fgetc(ent1);
            // pega o numero do comando undo
            pegar_numero(ent1, &num_undo);

            // mover os ponteiros para tras
            mover_tras(ent1);
            mover_tras(ent2);

            // verificar se esta no final do arquivo
            if (verificar_se_esta_no_final(ent1) == true)
            {
                // se estiver no final dos comandos, retorna, fechando a funcao
                return;
            }

            // desfazer o comando
            if (desfazer(&num_undo, &num_seg) == true)
            {
                // se o desfazer, retorna true, ele diz que esta no final dos comandos, fechando a funcao
                return;
            }
        }
    }
}

// funcao para ler o arquivo
void ler_arquivo(char *texto)
{
    // zerar o texto
    texto[0] = '\0';
    // funcao para configurar a posilção iniciais dos ponteiros
    configurar_ponteiros();

    // funcao para pecorrer o arquivo
    pecorrer_arquivo(texto);

    // funcao para jogar no arquivo de saida
    jogar_no_arq_saida(texto);

    // funcao para configurar os ponteiros proximo exemplo
    configurar_proximo();

    // Voltar o tamanho do texto para a alocação inicial
    if ((sizeof(texto) / sizeof(texto[0])) > numero_de_alocacao)
    {
        // realocar memoria
        realocacao(texto, 1);
    }
}

// Inicio do programa
int main(int *argv, char *argc[])
{
    // abertura dos arquivos
    ent1 = fopen(argc[1], "r");
    ent2 = fopen(argc[1], "r");
    sai = fopen(argc[2], "w");

    // alocacao do texto
    char *texto = malloc(sizeof(char) * numero_de_alocacao);

    // verificacao se os arquivos foram abertos e se o malloc foi feito
    if (!(ent1 || ent2 || sai) && texto)
    {
        printf("Erro ao abrir os arquivos ou malloc\n");
    }
    else
    {
        // loop para leitura dos exemplos
        for (int i = 0; i < cont_exemplos; i++)
        {
            // ler o arquivo
            ler_arquivo(texto);
        }
    }

    // fechamento dos arquivos e liberacao da memoria
    fclose(ent1);
    fclose(ent2);
    fclose(sai);
    free(texto);

    return 0;
}