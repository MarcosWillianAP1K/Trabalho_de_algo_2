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

typedef struct
{
    char palavra;
    bool palavra_bool;
} Texto;

FILE *ent1, *ent2, *sai;
char comando[5], caracter;
int num_seg, cont, cont_de_exemplos = 1;
long int cont_de_desfazer;

void jogar_no_arquivo_saida(Texto *texto)
{
    printf("\n");
    for (int i = 0; i < cont; i++)
    {
        if (texto[i].palavra_bool == true)
        {
            fprintf(sai, "%c", texto[i].palavra);
            printf("%c", texto[i].palavra);
        }
    }
    char aux = '\n';
    fprintf(sai, "%c", aux);
    printf("\n");
}

void mover_tras(FILE *ponteiro)
{
    char aux;

    fseek(ponteiro, sizeof(char) * -2, SEEK_CUR);

    while ((aux = fgetc(ponteiro)) && aux != '}' && aux != '{' && aux != ',' && aux != '\n' && !feof(ponteiro))
    {
        fseek(ponteiro, sizeof(char) * -2, SEEK_CUR);
    }
}

void configurar_ponteiros()
{
    char aux;

    aux = fgetc(ent1);
    aux = fgetc(ent2);
    while ((aux = fgetc(ent2)) && aux != '{' && !feof(ent2))
    {
    }
}

void configurar_proximo()
{
    char aux;

    while ((aux = fgetc(ent2)) && aux != '{' && !feof(ent2))
    {
        if (aux == '\n')
        {
            cont_de_exemplos++;
        }
    }

    while ((aux = fgetc(ent1)) && aux != '{' && !feof(ent1))
    {
    }

    fseek(ent1, sizeof(char) * -1, SEEK_CUR);
    fseek(ent2, sizeof(char) * -1, SEEK_CUR);
}

void pegar_caracter(Texto *texto, FILE *ponteiro)
{
    char caracter;

    caracter = fgetc(ponteiro);
    caracter = fgetc(ponteiro);

    if (cont % 10 == 0)
    {
        bool teste = true;

        do
        {
            Texto *aux = realloc(texto, sizeof(Texto) * (cont + 10));

            if (aux)
            {
                texto = aux;
                teste = false;
            }
            else
            {
                printf("\nrealoc Deu ruim\n");
            }
        } while (teste);
    }

    texto[cont].palavra = caracter;
    texto[cont].palavra_bool = true;
    caracter = fgetc(ponteiro);

    cont++;
}

void pegar_seg_da_posicao(int *num, FILE *ponteiro)
{
    char aux;
    *num = 0;

    while ((aux = fgetc(ponteiro)) && isdigit(aux))
    {
        *num *= 10;
        *num += aux - '0';
    }
}

void desfazer(Texto *texto, int *num_undo, int *seg_da_posicao, bool c, int cont_bool)
{
    cont_de_desfazer++;
    printf("\r%d", cont_de_desfazer);
    fflush(stdout);
    int num_atual, posicao_atual1, posicao_atual2;

    mover_tras(ent2);
    mover_tras(ent2);
    pegar_seg_da_posicao(&num_atual, ent2);
    mover_tras(ent2);

    mover_tras(ent1);
    mover_tras(ent1);
    fgets(comando, sizeof(comando), ent1);
    mover_tras(ent1);

    posicao_atual1 = ftell(ent1);

    for (int i = 0; i < *num_undo; i++)
    {
        if ((*seg_da_posicao - i - 1) == num_atual)
        {
            posicao_atual1 = ftell(ent1);

            if (strstr(comando, type))
            {
                if (cont - cont_bool >= 0)
                {
                    if (c)
                    {
                        texto[cont - cont_bool++].palavra_bool = true;
                    }
                    else
                    {
                        texto[cont - cont_bool++].palavra_bool = false;
                    }
                }
            }

            if (strstr(comando, undo))
            {
                int novo_num_undo = 0;
                posicao_atual2 = ftell(ent2);

                fgets(comando, sizeof(comando), ent1);
                caracter = fgetc(ent1);

                pegar_seg_da_posicao(&novo_num_undo, ent1);

                while ((caracter = fgetc(ent2)) && isdigit(caracter))
                {
                }

                if (c)
                {
                    desfazer(texto, &novo_num_undo, &num_atual, false, cont_bool);
                }
                else
                {
                    desfazer(texto, &novo_num_undo, &num_atual, true, cont_bool);
                }

                fseek(ent1, sizeof(char) * (posicao_atual1 - ftell(ent1)), SEEK_CUR);
                fseek(ent2, sizeof(char) * (posicao_atual2 - ftell(ent2)), SEEK_CUR);
            }
            mover_tras(ent2);
            pegar_seg_da_posicao(&num_atual, ent2);
            mover_tras(ent2);

            // esquisito verificar depois
            mover_tras(ent1);
            fgets(comando, sizeof(comando), ent1);
            mover_tras(ent1);
        }
    }
}

void pecorrer_arquivo(Texto *texto)
{
    cont_de_desfazer = 0;
    int posicao_atual1, posicao_atual2;
    cont = 0;

    while (!feof(ent1) && fgets(comando, sizeof(comando), ent1) && (strstr(comando, type) || strstr(comando, undo)))
    {
        pegar_seg_da_posicao(&num_seg, ent2);

        posicao_atual2 = ftell(ent2);

        if (strstr(comando, type))
        {
            pegar_caracter(texto, ent1);
        }

        if (strstr(comando, undo))
        {
            caracter = fgetc(ent1);

            int num_undo = 0;

            pegar_seg_da_posicao(&num_undo, ent1);

            posicao_atual1 = ftell(ent1);

            desfazer(texto, &num_undo, &num_seg, false, 1);

            fseek(ent1, sizeof(char) * (posicao_atual1 - ftell(ent1)), SEEK_CUR);
            fseek(ent2, sizeof(char) * (posicao_atual2 - ftell(ent2)), SEEK_CUR);
        }
    }
}

void ler_arquivo(Texto *texto)
{
    if (!feof(ent1) && !feof(ent2))
    {
        configurar_ponteiros();

        pecorrer_arquivo(texto);

        jogar_no_arquivo_saida(texto);

        configurar_proximo();
    }
}

int main(int argv, char *argc[])
{
    ent1 = fopen(argc[1], "r");
    ent2 = fopen(argc[1], "r");
    sai = fopen(argc[2], "w");
    Texto *texto = malloc(sizeof(Texto) * 10);

    if (!ent1 || !ent2 || !sai)
    {
        printf("\nDeu ruim\n");
    }
    else
    {
        for (int i = 0; i < cont_de_exemplos; i++)
        {
            clock_t start = clock();

            if (!(texto))
            {
                printf("\nDeu ruim\n");
            }
            else
            {
                printf("\n");
                ler_arquivo(texto);

                clock_t end = clock();

                float seconds = (float)(end - start) / CLOCKS_PER_SEC;
                printf("\n");

                printf("Tempo de execucao: %f\n", seconds);

                char *aux = realloc(texto, sizeof(texto) * 10);

                if (aux)
                {
                    texto = aux;
                }
                else
                {
                    printf("\nrealoc final Deu ruim\n");
                }
            }
        }
    }
    free(texto);
    fclose(ent1);
    fclose(ent2);
    fclose(sai);

    return 0;
}