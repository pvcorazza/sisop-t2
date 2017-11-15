#include "../include/t2fs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main () {

    printf("==== TESTE DO IDENTIFY2 ====\n\n");
    char nomes[200];
    identify2(nomes,200);
    puts(nomes);

    printf("==== DADOS DO SUPERBLOCO ====\n\n");
    print_superbloco_info();

    printf("\n======= FAT =======\n\n");
    inicializa_fat(SUPERBLOCO.DataSectorStart - SUPERBLOCO.pFATSectorStart);
    imprime_conteudo_fat(FAT, ((SUPERBLOCO.NofSectors - SUPERBLOCO.DataSectorStart) / SUPERBLOCO.SectorsPerCluster));

    printf("\n======= DIRETÓRIO RAIZ =======\n\n");

    le_diretorio(SUPERBLOCO.RootDirCluster);

    printf("\n======= DIR1 =======\n\n");

    le_diretorio(5);

    printf("\n======= OPENDIR2 =======\n\n");

    //Dir1
    int retorno = opendir2("/dir1");
    printf("Handle de retorno: %d \n", retorno);

    //Raiz
    retorno = opendir2("/.");
    printf("Handle de retorno: %d \n", retorno);

    printf("\n======= READDIR2 =======\n\n");

    DIRENT2 *dentry;

    int retorno_readdir = readdir2(0, dentry);

    printf("Nome em dentry: %s\n", dentry->name);
    printf("Retorno da readdir2: %d\n", retorno_readdir);

    retorno_readdir = readdir2(0, dentry);

    printf("Nome em dentry: %s\n", dentry->name);
    printf("Retorno da readdir2: %d\n", retorno_readdir);

    retorno_readdir = readdir2(0, dentry);

    printf("Nome em dentry: %s\n", dentry->name);
    printf("Retorno da readdir2: %d\n", retorno_readdir);

    retorno_readdir = readdir2(0, dentry);

    printf("Nome em dentry: %s\n", dentry->name);
    printf("Retorno da readdir2: %d\n", retorno_readdir);

    retorno_readdir = readdir2(0, dentry);
    printf("Nome em dentry: %s\n", dentry->name);
    printf("Retorno da readdir2: %d\n", retorno_readdir);


    printf("\n======= CLOSEDIR2 =======\n\n");

    //Fecha diretório /dir1
    int close = closedir2(0);
    printf("Retorno do closedir: %d ", close);

    return 0;

}
