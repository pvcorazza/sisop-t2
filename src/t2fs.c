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

/* Array para armazenar os diretórios abertos */
struct dir_aberto diretorios_abertos[MAX_ABERTOS];

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


struct t2fs_record compara_nomes(int cluster, char *filename) {

    char string[MAX_FILE_NAME_SIZE];
    strcpy(string, filename);
    char *array[((SUPERBLOCO.NofSectors - SUPERBLOCO.DataSectorStart) /
                 SUPERBLOCO.SectorsPerCluster)]; //C-1 clusters possíveis para o diretório
    int i = 0;

    array[i] = strtok(string, "/");

    while (array[i] != NULL) {
        array[++i] = strtok(NULL, "/");
    }

    int k = 0;
    int encontrou = 0;
    int temp_cluster = cluster;

    struct t2fs_record vazio;
    strcpy(vazio.name, "");
    vazio.firstCluster = NULL;
    vazio.TypeVal = NULL;
    vazio.bytesFileSize = NULL;

    struct t2fs_record record;
    struct t2fs_record record_retorno;


    while (array[k] != NULL) {
        for (i = 0; i < 4; i++) {

            unsigned char buffer[SECTOR_SIZE];
            read_sector(SUPERBLOCO.DataSectorStart + temp_cluster * SUPERBLOCO.SectorsPerCluster + i, &buffer[0]);

            int j = 0;
            for (j = 0; j < SECTOR_SIZE; j = j + 64) {
                memcpy(&record, &buffer[j], 64);
                if (!strcmp(record.name, array[k])) {
                    printf("ENCONTROU no CLUSTER %d: ", record.firstCluster);
                    puts(record.name);
                    encontrou = 1;
                    temp_cluster = record.firstCluster;
                    record_retorno = record;


                }
            }
        }
        if (encontrou == 0) {
            return vazio;
        }
        encontrou = 0;
        k++;
    }

    return record_retorno;

}


int encontra_posicao() {
    int i = 0;
    while (i < MAX_ABERTOS) {
        if (diretorios_abertos[i].aberto != 1) {
            return i;
        }
        i++;
    }
    return -1;
}

/* Informa a identificação dos desenvolvedores do t2fs. */
int identify2(char *name, int size) {

    char names[] = "Giovani Tirello (252741)\nMarcelo Wille (228991)\nPaulo Corazza (192172)\n";

    if (size < sizeof(names) / sizeof(char)) {
        return -1;        //Se o tamanho informado for menor do que o tamanho da string retorna -1, indicando erro
    } else {
        strcpy(name, names);
        return 0;         //Caso contrário copia a string para o endereço de memória indicada por name.
    }

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

DIR2 opendir2(char *pathname) {

    //FALTA: (b) deve posicionar o ponteiro de entradas (current entry) na primeira posição válida do diretório "pathname".

    if (first_time) {
        read_superblock();
    }

    DIR2 handle = encontra_posicao();
    struct t2fs_record record;

    if (handle >= 0 && handle <= MAX_ABERTOS) {
        record = compara_nomes(SUPERBLOCO.RootDirCluster, pathname);
    }

    if (record.name != NULL && record.TypeVal == TYPEVAL_DIRETORIO) {
        diretorios_abertos[handle].diretorio = record;
        diretorios_abertos[handle].aberto = 1;
        return handle;
    }
    return -1;

}


#define	END_OF_DIR	1
int readdir2(DIR2 handle, DIRENT2 *dentry);

int closedir2(DIR2 handle) {

    if (first_time) {
        read_superblock();
    }

    if (handle >= 0 && handle <= MAX_ABERTOS) {
        if (diretorios_abertos[handle].aberto == 1) {
            diretorios_abertos[handle].aberto = 0;
            return 0;
        }
    }

    return -1;

}


