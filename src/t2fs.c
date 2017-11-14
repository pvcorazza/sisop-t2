/*
 * Universidade Federal do Rio Grande do Sul (UFRGS)
 * Sistemas Operacionais I - Trabalho 2
 *
 * Giovani Tirello, Marcelo Wille, Paulo Victor Corazza
 */

#include "../include/apidisk.h"
#include "../include/t2fs.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Variável que indica se é a primeira vez que tenta executar a API (se sim, o superbloco ainda não foi lido) */
int first_time = 1;

void print_superbloco_info() {

    if(first_time) {
        read_superblock();
    }

    printf("Dados do superbloco:\n");
    printf("Identificacao do sistema de arquivos: %c%c%c%c\n", SUPERBLOCO.id[0], SUPERBLOCO.id[1], SUPERBLOCO.id[2], SUPERBLOCO.id[3]);
    printf("Versao do Sistema de Arquivos: %X\n", SUPERBLOCO.version);
    printf("Numero de setores logicos do superbloco: %d\n", SUPERBLOCO.SuperBlockSize);
    printf("Tamanho total da particao (em bytes): %d\n", SUPERBLOCO.DiskSize);
    printf("Numero de setores logicos na particao: %d\n", SUPERBLOCO.NofSectors);
    printf("Numero de setores logicos por cluster: %d\n", SUPERBLOCO.SectorsPerCluster);
    printf("Primeiro setor logico da FAT: %d\n", SUPERBLOCO.pFATSectorStart);
    printf("Cluster onde inicia o arquivo correspondente ao diretorio raiz: %d\n", SUPERBLOCO.RootDirCluster);
    printf("Primeiro setor logico da area de blocos de dados ou cluster: %d\n\n", SUPERBLOCO.DataSectorStart);
    printf("OUTROS DADOS SOBRE O SISTEMA DE ARQUIVOS:\n\n");
    printf("Numero total de clusters: %d\n", ((SUPERBLOCO.NofSectors - SUPERBLOCO.DataSectorStart) / SUPERBLOCO.SectorsPerCluster));
    //printf("Numero de setores logicos ocupados pelo diretorio raiz: %d\n\n", (SUPERBLOCO.DataSectorStart - SUPERBLOCO.RootDirCluster));
    printf("Numero de setores lógicos ocupados pela FAT: %d\n", SUPERBLOCO.DataSectorStart - SUPERBLOCO.pFATSectorStart);

}

int read_superblock() {

    BYTE buffer[SECTOR_SIZE];

    if (read_sector(0, (unsigned char *) &buffer) != 0) {
        printf("ERRO: Leitura do superbloco não foi feita com sucesso!\n\n");
        return -1;
    }
    else {
        memcpy(&SUPERBLOCO, buffer, 32);
        first_time = 0;
        return 0;
    }
}

int inicializa_fat(int totalSetoresFat)
{
    if(first_time) {
        read_superblock();
    }
    int i,j=0;
    for (i=SUPERBLOCO.pFATSectorStart;i<=totalSetoresFat+1;i++)
    {
        if (read_sector((unsigned int) i, (unsigned char *) &FAT[j]) != 0)
        {
            printf("Não foi possível fazer a leitura da FAT. \n");
            return -1;
        }
        else
        {
            j=j+64;
        }
    }
    return 0;
}

void imprime_conteudo_fat(DWORD *fat, int clusters)
{
    int i;
    for (i = 0; i < clusters; i++)
    {
        if (fat[i] != 0) {
            printf("Cluster %d: %X\n",i, fat[i]);
        }

    }
}

void le_diretorio(int cluster)
{

    struct t2fs_record raiz;

    int i;

    for (i=0; i<4; i++) {

        unsigned char buffer[SECTOR_SIZE];
        read_sector(SUPERBLOCO.DataSectorStart + cluster * SUPERBLOCO.SectorsPerCluster + i, &buffer[0]);

        int j=0;
        for (j=0; j<SECTOR_SIZE;j=j+64)  {
            memcpy(&raiz, &buffer[j], 64);
            if (raiz.TypeVal!=0) {
                printf("Tipo da entrada: Inválido=0, Arquivo = 1, Diretorio = 2: %d", raiz.TypeVal);
                printf("\nNome do arquivo: %s", raiz.name);
                printf("\nTamanho do arquivo: %d", raiz.bytesFileSize);
                printf("\nPrimeiro cluster de dados: %d\n\n", raiz.firstCluster);
            }
        }
    }
}

struct lista_caminho_absoluto *cria_lista_caminhos(char *filename) {
    struct lista_caminho_absoluto *ultimo;

    ultimo = malloc(sizeof(struct lista_caminho_absoluto));

    ultimo->anterior = NULL;
    ultimo->proximo = NULL;

    if (filename[0] == '/') {

        int i = 1;
        int j = 0;
        while (filename[i] != '\0') {

            if (filename[i] == '/') {
                ultimo->nome_cliente[j] = '\0';
                j = 0;
                ultimo->proximo = malloc(sizeof(struct lista_caminho_absoluto));
                (ultimo->proximo)->anterior = ultimo;
                ultimo = ultimo->proximo;
                ultimo->proximo = NULL;
                i++;
            }
            ultimo->nome_cliente[j] = filename[i];
            i++;
            j++;
        }
        ultimo->nome_cliente[j] = '\0';

        if (ultimo->nome_cliente[i - 1] == '\0') {
            return ultimo;
        }
    }
    return NULL;
}

FILE2 create2(char *filename) {

    if(first_time) {
        read_superblock();
    }

}

int delete2(char *filename);

FILE2 open2(char *filename);

int close2(FILE2 handle);

int read2(FILE2 handle, char *buffer, int size);

int write2(FILE2 handle, char *buffer, int size);

int truncate2(FILE2 handle);

int seek2(FILE2 handle, unsigned int offset);

int mkdir2(char *pathname);

int rmdir2(char *pathname);

int chdir2(char *pathname);

int getcwd2(char *pathname, int size);

DIR2 opendir2(char *pathname);

#define	END_OF_DIR	1
int readdir2(DIR2 handle, DIRENT2 *dentry);

int closedir2(DIR2 handle);

/* Informa a identificação dos desenvolvedores do t2fs. */
int identify2(char *name, int size) {

    char names[] = "Giovani Tirello (252741)\nMarcelo Wille (228991)\nPaulo Corazza (192172)\n";

    if (size < sizeof(names)/sizeof(char)) {
        return -1;        //Se o tamanho informado for menor do que o tamanho da string retorna -1, indicando erro
    }
    else {
        strcpy(name, names);
        return 0;         //Caso contrário copia a string para o endereço de memória indicada por name.
    }

}

