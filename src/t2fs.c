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
#include <locale.h>

/* Chama as funções disponíveis para leitura do primeiro setor lógico do disco e armazena no superbloco. */
int read_superblock (struct t2fs_superbloco *SUPERBLOCO) {

    BYTE buffer[SECTOR_SIZE];

    if (read_sector(0, (unsigned char *) &buffer) != 0) {
        printf("ERRO: Leitura do superbloco não foi feita com sucesso!\n\n");
        return -1;
    } else {
        memcpy(SUPERBLOCO, buffer, 32);
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

    char names[] = "Giovani Tirello (252741)\nMarcelo Wille (228991)\nPaulo Corazza (192172)\n";

    if (size < sizeof(names)/sizeof(char)) {
        return -1;        //Se o tamanho informado for menor do que o tamanho da string retorna -1, indicando erro
    }
    else {
        strcpy(name, names);
        return 0;         //Caso contrário copia a string para o endereço de memória indicada por name.
    }

}

/* Imprime as informações do superbloco. */
void print_debug_superblock (struct t2fs_superbloco SUPERBLOCO)
{

    setlocale(LC_ALL, "");      // para permitir acentuação da língua portuguesa

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
    printf("Numero de setores logicos ocupados pelo diretorio raiz: %d\n\n", (SUPERBLOCO.DataSectorStart - SUPERBLOCO.RootDirCluster));

}
