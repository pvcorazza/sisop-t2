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
    imprime_conteudo_fat();

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

    DIRENT2 *dentry = malloc(sizeof(DIRENT2));

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

    free(dentry);


    printf("\n======= CLOSEDIR2 =======\n\n");

    //Fecha diretório /dir1
    int close = closedir2(0);
    printf("Retorno do closedir: %d ", close);

    printf("\n======= MKDIR2 =======\n\n");

    mkdir2("/dir1/teste1");

    mkdir2("/dir1/teste2");

    mkdir2("/dir1/teste3");


    printf("\n======= RMDIR2 =======\n\n");

    rmdir2("/dir1/teste2");
    inicializa();
    imprime_conteudo_fat();
    le_diretorio(5);
    le_diretorio(11);

    printf("\n======= CHDIR2 =======\n\n");


    char nome[200];
    char pathname[] = "/dir1";
    getcwd2(nome, 200);
    printf("Diretório corrente antes do CHDIR2 é: %s\n", nome);

    chdir2(pathname);

    printf("\n======= GETCWD2 =======\n\n");

    getcwd2(nome, 200);
    printf("Diretório corrente é: %s\n", nome);

    return 0;



}
