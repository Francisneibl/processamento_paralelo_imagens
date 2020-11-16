#include<stdlib.h>
#include<stdio.h>
#include<omp.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include"stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

#define MAX 512


typedef struct img{
    int altura;
    int largura;
    int canais;
    unsigned char *data;
}Imagem;

int carregaImagemFiltro(int x, int y);
int media(int v[8]);

int v[8];
int matriz[MAX][MAX];
int matriz2[MAX][MAX];

int main(){

    Imagem img;

    img.data = stbi_load("img/lena_gray.gif",&img.largura, &img.altura, &img.canais, 1);

    int buffer =img.altura * img.largura;

    int x,y,c;
    #pragma omp parallel private(c)
    {

        #pragma omp for ordered
        for(c=0; c<buffer; c++){
            #pragma omp ordered
            if(x == img.largura){
                y++;
                x = 0;
            }
            matriz[x][y] = img.data[c];
            x++;

        }
    }
    #pragma omp parallel private(x,y)
    {
        #pragma omp for ordered
        for(x=0;x<MAX;x++){
            #pragma omp ordered
            for(y=0;y<MAX;y++){
                carregaImagemFiltro(x,y);
            }
        }

    }
    c=0;
    #pragma omp parallel private(x,y)
    {
        #pragma omp for ordered
        for(x=0;x<MAX;x++){
            #pragma omp ordered
            for(y=0;y<MAX;y++){
                img.data[c] = matriz2[y][x];
                c++;
            }

        }
    }

    stbi_write_bmp("suavisada.bmp",MAX,MAX,1,img.data);
    return 0;
}

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

    matriz2[x][y] = media(v);

}

int media(int v[8]){

    int c;
    int soma = 0;

    for(c=0;c<8;c++){
        soma = soma + v[c];
    }

    int res;

    res = soma/9;

    return res;
}
