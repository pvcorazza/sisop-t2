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

/* Array para armazenar os diretórios abertos */
struct dir_aberto diretorios_abertos[MAX_ABERTOS];
/* Variável que indica se é a primeira vez que tenta executar a API (se sim, o superbloco ainda não foi lido) */
int first_time = 1;

int inicializa() {

    if (read_superblock() == 0) {


        TOTAL_SETORES_FAT = SUPERBLOCO.DataSectorStart - SUPERBLOCO.pFATSectorStart;
        NUM_CLUSTERS = ((SUPERBLOCO.NofSectors - SUPERBLOCO.DataSectorStart) / SUPERBLOCO.SectorsPerCluster);

        first_time = 0;
        inicializa_fat();
        return 0;

    }

    exit(1);
}

void print_superbloco_info() {

    if(first_time) {
        inicializa();
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
    printf("Numero total de clusters: %d\n", NUM_CLUSTERS);
    //printf("Numero de setores logicos ocupados pelo diretorio raiz: %d\n\n", (SUPERBLOCO.DataSectorStart - SUPERBLOCO.RootDirCluster));
    printf("Numero de setores lógicos ocupados pela FAT: %d\n", TOTAL_SETORES_FAT);

}

static int read_superblock() {

    BYTE buffer[SECTOR_SIZE];

    if (read_sector(0, (unsigned char *) &buffer) != 0) {
        printf("ERRO: Leitura do superbloco não foi feita com sucesso!\n\n");
        return -1;
    }
    else {
        memcpy(&SUPERBLOCO, buffer, 32);
        return 0;
    }
}

static int inicializa_fat()
{
    int i,j=0;
    for (i = SUPERBLOCO.pFATSectorStart; i <= TOTAL_SETORES_FAT + 1; i++)
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


void imprime_conteudo_fat() {
    if (first_time) {
        inicializa();
    }
    int i;
    for (i = 0; i < NUM_CLUSTERS; i++)
    {
        if (FAT[i] != 0) {
            printf("Cluster %d: %X\n", i, FAT[i]);
        }

    }
}

void le_diretorio(int cluster)
{
    if (first_time) {
        inicializa();
    }

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

void divide_caminho(char *string, char *array[]) {

    if (first_time) {
        inicializa();
    }
    int i = 0;

    array[i] = strtok(string, "/");

    while (array[i] != NULL) {
        array[++i] = strtok(NULL, "/");
    }
}

struct t2fs_record compara_nomes(int cluster, char *filename) {

    if (first_time) {
        inicializa();
    }
    char string[MAX_FILE_NAME_SIZE];
    strcpy(string, filename);

    char *array[NUM_CLUSTERS]; //C-1 clusters possíveis para o diretório

    divide_caminho(string, array);

    int i = 0;

    int k = 0;
    int encontrou = 0;
    int temp_cluster = cluster;

    struct t2fs_record vazio;
    strcpy(vazio.name, "");
    vazio.firstCluster = (DWORD) NULL;
    vazio.TypeVal = (int) NULL;
    vazio.bytesFileSize = (DWORD) NULL;

    struct t2fs_record record = vazio;
    struct t2fs_record record_retorno = vazio;


    while (array[k] != NULL) {
        for (i = 0; i < 4; i++) {

            unsigned char buffer[SECTOR_SIZE];
            read_sector(SUPERBLOCO.DataSectorStart + temp_cluster * SUPERBLOCO.SectorsPerCluster + i, &buffer[0]);

            int j = 0;
            for (j = 0; j < SECTOR_SIZE; j = j + 64) {
                memcpy(&record, &buffer[j], 64);
                if (!strcmp(record.name, array[k])) {
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


DIR2 encontra_posicao() {
    if (first_time) {
        inicializa();
    }
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

    if (first_time) {
        inicializa();
    }

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
        inicializa();
    }
    return -1;

}

int delete2(char *filename) {
    return -1;
}

FILE2 open2(char *filename) {
    return -1;
}

int close2(FILE2 handle) {
    return -1;
}

int read2(FILE2 handle, char *buffer, int size) {
    return -1;
}

int write2(FILE2 handle, char *buffer, int size) {
    return -1;
}

int truncate2(FILE2 handle) {
    return -1;
}

int seek2(FILE2 handle, unsigned int offset) {
    return -1;
}

int mkdir2(char *pathname) {

    if (first_time) {
        inicializa();
    }


    return -1;


}

int rmdir2(char *pathname) {
    return -1;
}

int chdir2(char *pathname) {
    return -1;
}

int getcwd2(char *pathname, int size) {
    return -1;
}

DIR2 opendir2(char *pathname) {

    if (first_time) {
        inicializa();
    }

    DIR2 handle = encontra_posicao();
    struct t2fs_record record;

    if (handle >= 0 && handle <= MAX_ABERTOS) {
        record = compara_nomes(SUPERBLOCO.RootDirCluster, pathname);
        if (record.name != NULL && record.TypeVal == TYPEVAL_DIRETORIO) {
            diretorios_abertos[handle].diretorio = record;
            diretorios_abertos[handle].aberto = 1;
            diretorios_abertos[handle].current_entry = 0; //Conferir se é isso mesmo.
            return handle;
        }
    }

    return -1;

}

#define	END_OF_DIR	1

int readdir2(DIR2 handle, DIRENT2 *dentry) {
    if (first_time) {
        inicializa();
    }

    if (handle >= 0 && handle <= MAX_ABERTOS) {
        if (diretorios_abertos[handle].diretorio.TypeVal == TYPEVAL_DIRETORIO) {

            struct t2fs_record record;
            int current_entry = diretorios_abertos[handle].current_entry;
            int i;

            /* Faz uma busca pelo cluster de diretório pela entrada de número "current_entry",
             * caso encontre, copia os dados para a estrutura dentry, incrementa a "current_entry
             * do diretório e retorna indicando sucesso */
            for (i = 0; i < 4; i++) {
                unsigned char buffer[SECTOR_SIZE];
                read_sector(SUPERBLOCO.DataSectorStart +
                            diretorios_abertos[handle].diretorio.firstCluster * SUPERBLOCO.SectorsPerCluster + i,
                            &buffer[0]);

                int j = 0;
                for (j = 0; j < SECTOR_SIZE; j = j + 64) {
                    memcpy(&record, &buffer[j], 64);
                    if (record.TypeVal == 1 || record.TypeVal == 2) {
                        if (current_entry == 0) {

                            dentry->fileType = record.TypeVal;
                            dentry->fileSize = record.bytesFileSize;
                            strcpy(dentry->name, record.name);

                            diretorios_abertos[handle].current_entry++;

                            return 0;
                        }
                        current_entry--;
                    }
                }
            }

            /* Caso ja tenha chegado na última entrada válida do diretório
             * retorna erro */
            current_entry++;
            if (current_entry > 0) {
                return -END_OF_DIR;
            }
        }
    }
    return -2;
}

int closedir2(DIR2 handle) {

    if (first_time) {
        inicializa();
    }

    if (handle >= 0 && handle <= MAX_ABERTOS) {
        if (diretorios_abertos[handle].aberto == 1) {
            diretorios_abertos[handle].aberto = 0;
            return 0;
        }
    }

    return -1;

}


