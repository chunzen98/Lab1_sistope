
#include "funciones.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb-master/stb_image_write.h"
JPG* leerJPG(char* nombreArchivo){

	JPG* img = (JPG*)malloc(sizeof(JPG));


	img->data = stbi_load(nombreArchivo, &img->width, &img->height, &img->channels, 0);

	if(img->data == NULL) {
        // printf("Unable to allocate memory for the image.\n");
        exit(1);
    }

	
	img->size = img->width * img->height * img->channels;
	

    return img;
}

// Se genera data de la imagen a partir de RGB y se escribe con la funcion stb_write
void escribirJPG(JPG* imagen, char* nombreArchivo){

	//Escribir imagen
	stbi_write_png(nombreArchivo, imagen->width, imagen->height, imagen->channels, imagen->data, imagen->width * imagen->channels);
}

void escalaGris(JPG* img, JPG* imgGray){

    //Parse img data to unsigned char
    unsigned char* data = (unsigned char*)malloc(sizeof(unsigned char) * img->size);
    unsigned char* dataGray = (unsigned char*)malloc(sizeof(unsigned char) * imgGray->size);
    for (int i = 0; i < img->size; i++){
        data[i] = img->data[i];
    }
    for (int i = 0; i < imgGray->size; i++){
        dataGray[i] = imgGray->data[i];
    }

    //Apply filter
    for(unsigned char *p = data, *pg = dataGray; p != data + img->size; p += img->channels, pg += imgGray->channels) {
        *pg = (unsigned char)(*p*0.3 + *(p + 1)*0.59 + *(p + 2)*0.11);
        if(img->channels == 4) {
            *(pg + 1) = *(p + 3);
        }
    }

    //Parse unsigned char data into char
    for (int i = 0; i < imgGray->size; i++){
        imgGray->data[i] = dataGray[i];
    }
}

void filtro(JPG* img, JPG* imgMask, char* maskName){

    int* mask = (int*)malloc(sizeof(int) * 9);


    FILE* maskFile = fopen(maskName,"r");

    if (maskFile==NULL) 
    { 
        printf("No such file.\n"); 
        exit(1);
    } 
  
    char num1[9];
    char num2[9];
    char num3[9];
    
    int z = 0;

    while(fscanf(maskFile, "%s %s %s", num1, num2, num3) == 3){
        if(z == 0){
            mask[0] = atoi(num1);
            mask[1] = atoi(num2);
            mask[2] = atoi(num3);
        }
        else if( z == 1){
            mask[3] = atoi(num1);
            mask[4] = atoi(num2);
            mask[5] = atoi(num3);
        }
        else if(z == 2){
            mask[6] = atoi(num1);
            mask[7] = atoi(num2);
            mask[8] = atoi(num3);       
        }
        z++;
    }
   int x;
    x = 0;
    for (int i = 0; i < img->height; i++){
        for (int j = 0; j < img->width; j++){
            if(i == 0 || i == img->height - 1 || j == 0 || j == img->width - 1){
                imgMask->data[x] = 0;
            }
            else{
                imgMask->data[x] =  (img->data[x - img->width - 1] * mask[0]) + (img->data[x - img->width] * mask[1]) + (img->data[x - img->width + 1] * mask[2]) +  // top 3
                                (img->data[x-1] * mask[3]) + (img->data[x] * mask[4]) + (img->data[x+1] * mask[5]) +                                 // mid 3
                                (img->data[x + img->width - 1] * mask[6]) + (img->data[x + img->width] * mask[7]) + (img->data[x + img->width + 1] * mask[8]);   // bot 3
            }

            if(imgMask->data[x] > 255){
                imgMask->data[x] = 255;
            }
            else if(imgMask->data[x] < 0){
                imgMask->data[x] = 0;
            }

            x++;
        }
    }
}

void binarizacion(JPG* img, JPG* imgBin, int umbral){
    int x = 0;
    for (int i = 0; i < img->height; i++){
        for (int j = 0; j < img->width; j++){
            if(img->data[x] > umbral){
                imgBin->data[x] = 255;
            }
            else{
                imgBin->data[x] = 0;
            }
            x++;
        }    
    }
}

// 1 : nearly blacc
// 0 : not nearly blacc
int clasificacion(JPG* img, int umbral){
    int x = 0;

    int pixNegros = 0;
    int pixNoNegros = 0;

    for (int i = 0; i < img->height; i++){
        for (int j = 0; j < img->width; j++){
            if(img->data[x] < umbral){
                pixNegros += 1;
            }
            else{
                pixNoNegros += 1;
            }
            x++;
        }    
    }

    if(pixNegros > pixNoNegros){
        return 1;
    }
    else{
        return 0;
    }

}