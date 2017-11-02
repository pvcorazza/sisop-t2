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

/* Chama as funções disponíveis para leitura do primeiro setor lógico do disco e armazena no superbloco. */
int read_superblock (struct t2fs_superbloco *superbloco) {

    BYTE buffer[SECTOR_SIZE];

    if (read_sector(0, (unsigned char *) &buffer) != 0){
        return -1;
    } else {
        memcpy(superbloco,buffer,32);
        return 0;
    }
}

FILE2 create2 (char *filename);

int delete2 (char *filename);

FILE2 open2 (char *filename);

int close2 (FILE2 handle);

int read2 (FILE2 handle, char *buffer, int size);

int write2 (FILE2 handle, char *buffer, int size);

int truncate2 (FILE2 handle);

int seek2 (FILE2 handle, unsigned int offset);

int mkdir2 (char *pathname);

int rmdir2 (char *pathname);

int chdir2 (char *pathname);

int getcwd2 (char *pathname, int size);

DIR2 opendir2 (char *pathname);

#define	END_OF_DIR	1
int readdir2 (DIR2 handle, DIRENT2 *dentry);

int closedir2 (DIR2 handle);

/* Informa a identificação dos desenvolvedores do t2fs. */
int identify2 (char *name, int size) {
    char names[] = "Marcelo Wille (228991)\nGiovani Tirello (252741)\nPaulo Corazza (192172)\n";
    if (size < sizeof(names)/sizeof(char)) {
        return -1; //Se o tamanho informado for menor do que o tamanho da string retorna -1, indicando erro
    }
    else {
        strcpy(name, names);
        return 0; //Caso contrário copia a string para o endereço de memória indicada por name.
    }
}

/* Imprime as informações do superbloco. */
void print_debug_superblock (struct t2fs_superbloco superbloco)
{
    printf("Identificação do sistema de arquivos: %c%c%c%c", superbloco.id[0], superbloco.id[1], superbloco.id[2], superbloco.id[3]);
    printf("\nVersão do sistema de arquivos: %X", superbloco.version);
    printf("\nNúmero de setores lógicos que formam o superbloco: %d", superbloco.SuperBlockSize);
    printf("\nTamanho total da partiçao (em bytes): %d", superbloco.DiskSize);
    printf("\nNúmero de setores logicos na partição: %d", superbloco.NofSectors);
    printf("\nNúmero de setores logicos por cluster: %d", superbloco.SectorsPerCluster);
    printf("\nPrimeiro setor logico da FAT: %d", superbloco.pFATSectorStart);
    printf("\nCluster onde inicia o arquivo correspondente ao diretório raiz: %d", superbloco.RootDirCluster);
    printf("\nPrimeiro setor lógico da área de blocos de dados ou cluster: %d", superbloco.DataSectorStart);

    printf("\nOUTROS DADOS SOBRE O SISTEMA DE ARQUIVOS: ");
    printf("\nO número total de clusters é: %d", ((superbloco.NofSectors - superbloco.DataSectorStart) / superbloco.SectorsPerCluster));
    printf("\nNúmero de setores lógicos ocupados pelo diretório raiz: %d\n\n", (superbloco.DataSectorStart - superbloco.RootDirCluster));
}
