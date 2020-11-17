#include<stdlib.h>
#include<stdio.h>
#include<omp.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include"stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

#define MAX 512

/*
**Author: Francisnei Bernardes Lima
**Data: 17/11/20
**Projeto de processamento de imagens em paralelo
**utilizando OpenMP e stb_lib
*/
typedef struct img{
    int altura;
    int largura;
    int canais;
    unsigned char *data;
}Imagem;

int carregaImagemFiltro(int x, int y);
int realce(int v[8]);

int v[8];
int matriz[MAX][MAX];

int main(){

    Imagem img;

    //função carrega pixels da imagem
    img.data = stbi_load("img/lena_gray.gif",&img.largura, &img.altura, &img.canais, 1);

    //calcula o total de pixels da imagem
    int buffer =img.altura * img.largura;
    int x,y,c;

    //cria regiao paralela do codigo (fork)
    #pragma omp parallel private(c)
    {
    #pragma omp for ordered //diretiva 'ordered' necessaria para que a imagem nao fique destorcida
    for(c=0; c<buffer; c++){
        #pragma omp ordered
        if(x == img.largura){
            y++;                //carrega os pixels em uma matriz de inteiros
            x = 0;
        }
        #pragma omp flush(c)
        matriz[x][y] = img.data[c];
        x++;

    }
    }
    //cria regiao paralela do codigo (fork)
    #pragma omp parallel private(x,y)
    {
    #pragma omp for ordered
    for(x=0;x<MAX;x++){
        #pragma omp ordered
        for(y=0;y<MAX;y++){
            carregaImagemFiltro(x,y); //percorre todos os pixels da matriz e envia para o filtro
        }
    }
    }

    c=0;
    //cria regiao paralela do codigo (fork)
    #pragma omp parallel private(x,y)
    {
    #pragma omp for ordered
    for(x=0;x<MAX;x++){
        #pragma omp ordered
        for(y=0;y<MAX;y++){
            #pragma omp flush(c)
            img.data[c] = matriz[y][x]; //monta os pixel de volta ao array
            c++;
        }

    }
    }

    stbi_write_bmp("realcada.bmp",MAX,MAX,1,img.data); //monta a imagem no formato bmp.


    return 0;

}

/*
função captura os pixels vizinhos do pixel passado
pela coornada x,y por parametros para a função
o tamanho do filtro é 3x3
*/
int carregaImagemFiltro(int x, int y){

    v[0] = matriz[x-1][y-1];
    v[1] = matriz[x-1][y-0];
    v[2] = matriz[x-1][y+1];
    v[3] = matriz[x-0][y-1];
    v[4] = matriz[x-0][y-0];
    v[5] = matriz[x-0][y+1];
    v[6] = matriz[x+1][y-1];
    v[7] = matriz[x+1][y-0];
    v[8] = matriz[x+1][y+1];

    if(x == 0){
        v[0] = 0;
        v[1] = 0;
        v[2] = 0;
    }

    if(x == MAX -1){
        v[6] = 0;
        v[7] = 0;
        v[8] = 0;
    }

    if(y == 0){
       v[0] = 0;
       v[3] = 0;
       v[6] = 0;
    }

    if(y == MAX - 1){
        v[2] = 0;
        v[5] = 0;
        v[8] = 0;
    }

    matriz[x][y] = realce(v);

}

//função que executo o processamento do pixels de acordo com o filtro
int realce(int v[8]){

    int c;
    int soma = 0;
    int res = 0;
    int filtro[8];
    int s[8];

    filtro[0] = 0;
    filtro[1] = 0;
    filtro[2] = 0;
    filtro[3] = 0;
    filtro[4] = 2;
    filtro[5] = 0;
    filtro[6] = 0;
    filtro[7] = 0;
    filtro[8] = 0;


    for(c=0;c<8;c++){
        s[c] = v[c]*filtro[c];
    }

    for(c=0;c<8;c++){
        res = res + s[c];
    }
    return res;

}
