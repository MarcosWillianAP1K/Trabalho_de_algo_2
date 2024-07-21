#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

#define type "type"
#define undo "undo"
#define nome_ent "entrada.txt"
#define nome_sai "saida.txt"
#define numero_de_alocacao 50

FILE *ent1, *ent2, *sai;
char comando[5], caracter;
int cont_exemplos = 1;

void inverter(int inicio, int fim, char *string)
{
    char aux;

    for (int i = 0; i < ((fim - inicio)); i++)
    {
        aux = string[inicio];
        string[inicio] = string[fim - i - 1];
        string[fim - i - 1] = aux;
        inicio++;
    }
}

void jogar_no_arq_saida(char *texto)
{
    inverter(0, strlen(texto), texto);

    fprintf(sai, "%s\n", texto);
    printf("%s\n", texto);
}

void realocacao(char *texto, int n)
{
    bool teste = true;

    do
    {
        char *aux = realloc(texto, (sizeof(char) * (n * numero_de_alocacao)));

        if (aux)
        {
            texto = aux;
            teste = false;
        }
        else
        {
            printf("\nrealoc final Deu ruim\n");
        }
    } while (teste);
}

bool verificar_se_esta_no_final(FILE *ponteiro)
{
    char aux;

    fseek(ponteiro, sizeof(char) * -1, SEEK_CUR);

    if ((aux = fgetc(ponteiro)) != EOF && aux == '{')
    {
        return true;
    }

    return false;
}

void mover_tras(FILE *ponteiro)
{
    char aux;

    fseek(ponteiro, sizeof(char) * -2, SEEK_CUR);

    while ((aux = fgetc(ponteiro)) != EOF && aux != '{' && aux != ',')
    {
        fseek(ponteiro, sizeof(char) * -2, SEEK_CUR);
    }
}

void pegar_caracter(char *texto)
{
    char aux;
    int inicio = strlen(texto);

    caracter = fgetc(ent1);

    while ((caracter = fgetc(ent1)) != EOF && caracter != ',' && caracter != '}')
    {
        int tam = strlen(texto);

        if (tam % numero_de_alocacao == 0 && tam > 0)
        {
            realocacao(texto, (tam / numero_de_alocacao) + 1);
        }

        texto[tam] = caracter;
        texto[tam + 1] = '\0';
    }

    if (inicio != strlen(texto) - 1)
    {
        inverter(inicio, strlen(texto), texto);
    }
}

void pegar_numero(FILE *ponteiro, int *num)
{
    *num = 0;

    while ((caracter = fgetc(ponteiro)) != EOF && isdigit(caracter))
    {
        *num *= 10;
        *num += caracter - '0';
    }
}

void configurar_ponteiros()
{
    char aux;

    while ((aux = fgetc(ent1)) != EOF && aux != '}')
        ;

    while ((aux = fgetc(ent2)) != EOF && aux != '}')
        ;

    while ((aux = fgetc(ent2)) != EOF && aux != '}')
        ;

    mover_tras(ent1);
    mover_tras(ent2);
}

void configurar_proximo()
{
    char aux;

    while ((aux = fgetc(ent1)) != EOF && aux != '{')
        ;

    while ((aux = fgetc(ent1)) != EOF && aux != '{')
        ;

    while ((aux = fgetc(ent2)) != EOF && aux != '{')
    {
        if (aux == '\n')
        {
            cont_exemplos++;
        }
    }

    fseek(ent1, sizeof(char) * -1, SEEK_CUR);
    fseek(ent2, sizeof(char) * -1, SEEK_CUR);
}

bool desfazer(int *num_undo, int *num_seg)
{
    int num_atual = 0;
    mover_tras(ent1);
    mover_tras(ent2);
    pegar_numero(ent2, &num_atual);
    mover_tras(ent2);

    while (*num_seg - *num_undo <= num_atual)
    {
        if (verificar_se_esta_no_final(ent1) == true)
        {
            return true;
        }
        mover_tras(ent1);
        mover_tras(ent2);
        pegar_numero(ent2, &num_atual);
        mover_tras(ent2);
    }

    return false;
}

void pecorrer_arquivo(char *texto)
{
    while (fgets(comando, sizeof(comando), ent1) && (strcmp(comando, type) == 0 || strcmp(comando, undo) == 0))
    {
        int num_seg = 0;

        pegar_numero(ent2, &num_seg);

        if (strcmp(comando, type) == 0)
        {
            pegar_caracter(texto);

            mover_tras(ent1);
            mover_tras(ent2);

            if (verificar_se_esta_no_final(ent1) == true)
            {
                return;
            }
            mover_tras(ent1);
            mover_tras(ent2);
        }
        if (strcmp(comando, undo) == 0)
        {
            int num_undo = 0;
            caracter = fgetc(ent1);
            pegar_numero(ent1, &num_undo);

            mover_tras(ent1);
            mover_tras(ent2);

            if (verificar_se_esta_no_final(ent1) == true)
            {
                return;
            }

            if (desfazer(&num_undo, &num_seg) == true)
            {
                return;
            }
        }
    }
}

void ler_arquivo(char *texto)
{
    texto[0] = '\0';
    configurar_ponteiros();

    pecorrer_arquivo(texto);

    jogar_no_arq_saida(texto);

    configurar_proximo();

    if ((sizeof(texto) / sizeof(texto[0])) > numero_de_alocacao)
    {
        realocacao(texto, 1);
    }
}

int main(int *argv, char *argc[])
{
    ent1 = fopen(argc[1], "r");
    ent2 = fopen(argc[1], "r");
    sai = fopen(argc[2], "w");

    char *texto = malloc(sizeof(char) * numero_de_alocacao);

    if (!(ent1 || ent2 || sai) && texto)
    {
        printf("Erro ao abrir os arquivos ou malloc\n");
    }
    else
    {
        for (int i = 0; i < cont_exemplos; i++)
        {
            ler_arquivo(texto);
        }
    }

    fclose(ent1);
    fclose(ent2);
    fclose(sai);
    free(texto);

    return 0;
}