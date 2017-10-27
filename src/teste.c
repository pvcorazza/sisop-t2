//
// Created by aluno on 27/10/17.
//

#include <stdio.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include "../include/superblock.h"

int main () {

    BYTE buffer[SECTOR_SIZE];

    printf("==== TESTE DO IDENTIFY2 ====\n\n");

    char nomes[200];
    identify2(nomes,200);
    puts(nomes);

    printf("==== LEITURA DO SUPERBLOCO ====\n\n");

    //Estrutura para armazenar o superbloco lido.
    struct t2fs_superbloco superblocoinfo;

    if (read_sector(0, (unsigned char *) &buffer) != 0){
        printf("Erro na leitura do superbloco \n\n");
        return -1;
    } else {
        printf("DEBUG: Superbloco foi lido corretamente \n\n");
        memcpy(&superblocoinfo,buffer,32);
        print_debug_superblock(superblocoinfo);
        return 0;
    }










}