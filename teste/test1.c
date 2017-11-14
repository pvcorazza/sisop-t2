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

    printf("\n======= CRIA LISTA COM CAMINHO ABSOLUTO =======\n\n");

    int retorno = opendir2("/dir1");

    printf("Handle de retorno: %d ", retorno);

    return 0;

}
