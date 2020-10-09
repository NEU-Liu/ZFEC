#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "./fec/fec.h"

#define ECC_K 4
#define ECC_N 8
#define ECC_C (ECC_N-ECC_K)
#define BLK_SZ 6


int main(int argv, char *argc[]){
    fec_t *code = fec_new(ECC_K, ECC_N);
    unsigned char * src[ECC_K];
    for (int i = 0; i < ECC_K; ++i) {
        src[i] = calloc(BLK_SZ,sizeof(char));
    }
    strcpy((void *)src[0],"abcdef");
    strcpy((void *)src[1],"ghiklm");
    strcpy((void *)src[2],"nopqrs");
    strcpy((void *)src[3],"uvwxyz");

    printf("----------start src----------\n");
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 6; ++j) {
            printf("%4d ", src[i][j]);
        }
        printf("\n");
    }
    printf("----------end   src----------\n");
    unsigned char *fecs[ECC_C];
    for (int i = 0; i < ECC_C; ++i) {
        fecs[i] = malloc(BLK_SZ);
        memset(fecs[i], 0, BLK_SZ);
    }
    int blocks[4] = {4, 5, 6, 7};

    printf("----------start fecs---------\n");
    fec_encode(code,  (void *)src, (gf *const *) fecs, (void *)blocks, ECC_C, BLK_SZ);
    for (int k = 0; k < 4; ++k) {
        for (int j = 0; j < 6; ++j) {
            printf("%4d ", fecs[k][j]);
        }
        printf("\n");
    }
    printf("----------end   fecs---------\n");





    printf("---------src and fec---------\n");
    unsigned char *inpkts[ECC_N];
    for (int i = 0; i < ECC_N; ++i) {
        inpkts[i] = calloc(BLK_SZ,0);
    }

    for (int i = 0; i < ECC_K; ++i) {
        inpkts[i] = src[i];
    }
    for (int i = 0; i < ECC_C; ++i) {
        inpkts[i + ECC_K] = fecs[i];
    }

    for (int i = 0; i < ECC_N; ++i) {
        for (int j = 0; j < 6; ++j) {
            printf("%4d ", inpkts[i][j]);
        }
        printf("\n");
    }
    printf("--------> transport <--------\n");
    memset(inpkts[0],0,BLK_SZ);
    memset(inpkts[1],0,BLK_SZ);
    printf("----传输过程中丢失了包1、 包2----\n");
    printf("---------src and fec---------\n");
    for (int i = 0; i < ECC_N; ++i) {
        for (int j = 0; j < 6; ++j) {
            printf("%4d ", inpkts[i][j]);
        }
        printf("\n");
    }


    printf("-------------FEC-------------\n");

    inpkts[0] = fecs[3];
    inpkts[1] = fecs[0];
    inpkts[2] = src[2];
    inpkts[3] = src[3];
    int index[4] ={7,4,2,3};
    //特别注意:这里index一定要和这{fecs[3],fecs[0],src[2],src[3]}几个包在原始包中的序号对应!


    unsigned char * restrict out[2];
    out[0] = calloc(BLK_SZ,sizeof(unsigned char ));
    out[1] = malloc(BLK_SZ);



    fec_decode(code,  (void *)inpkts,  out, index, BLK_SZ);

    for (int i = 0; i < ECC_N; ++i) {
        for (int j = 0; j < 6; ++j) {
            printf("%4d ", inpkts[i][j]);
        }
        printf("\n");
    }

    printf("=======恢复出丢失的包1、包2======\n");

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 6; ++j) {
            printf("%4d ", out[i][j]);
        }
        printf("\n");
    }

    free(code);
    return 0;
}

