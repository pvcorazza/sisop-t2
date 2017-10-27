//
// Created by Paulo Victor on 27/10/17.
//

#include <string.h>
#include "stdio.h"
#include "../include/t2fs.h"
#include "../include/apidisk.h"

/* Chama as funções disponíveis para leitura do primeiro setor lógico
 * do disco e armazena as informações necessárias no superbloco */

int read_superblock (struct t2fs_superbloco *superbloco) {

    BYTE buffer[SECTOR_SIZE];

    if (read_sector(0, (unsigned char *) &buffer) != 0){
        return -1;
    } else {
        memcpy(superbloco,buffer,32);
        return 0;
    }
}

/* Imprime informações sobre o superbloco */

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
