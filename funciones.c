
#include "funciones.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb-master/stb_image_write.h"

//Entradas: Cadena de caracteres que representa el nombre del archivo JPG a leer
//Funcionamiento: Lee el JPG especificado y lo guarda en variable img.
//Salidas: Retorna estructura JPG que representa la imagen leida.
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

//Entradas: Puntero a estructura JPG que contiene los datos de la imagen y cadena de caracteres con el nombre de la imagen.
//Funcionamiento: Se genera data de la imagen a partir de RGB y se escribe con la funcion stb_write.
//Salidas: Sin salida.
void escribirJPG(JPG* imagen, char* nombreArchivo){

	//Escribir imagen
	stbi_write_png(nombreArchivo, imagen->width, imagen->height, imagen->channels, imagen->data, imagen->width * imagen->channels);
}

//Entradas: Dos punteros a estructuras JPG que representan la imagen y una copia para convertirla a escala de grises.
//Funcionamiento: Se recorre la lista de datos de la imagen y se aplica la funcion para convertirla en escala de grises.
//Salidas: Sin salida.
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

//Entradas: Dos punteros a estructuras JPG que representan la imagen y una copia de ella, cadena de caracteres con el nombre de la mascara.
//Funcionamiento: Se lee la mascara, se recorre la imagen y se aplica la mascara.
//Salidas: Sin salida.
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
//Entradas: Dos punteros a estructuras JPG que representan la imagen y una copia de ella, entero que representa el umbral para convertir los pixeles a blanco o negro
//Funcionamiento: Se recorre imagen y si el pixel pasa el umbral se pasa el valor a 255, si no se cambia a 0.
//Salidas: Sin salida
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



//Entradas: Puntero a estructura JPG que representa la imagen, entero que representa al umbral.
//Funcionamiento: Se cuentan pixeles blancos y negros, si hay mas pixeles negros la imagen es nearly black, si no es not nearly black
//Salidas: Entero 1 si es nearly black 0 si no lo es.
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
    // 1 : nearly blacc
    // 0 : not nearly blacc
    if(pixNegros > pixNoNegros){
        return 1;
    }
    else{
        return 0;
    }

}