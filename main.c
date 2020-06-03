#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb-master/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb-master/stb_image_write.h"


typedef struct JPG{
	int width;		//img->data px width
	int height;		//img->data px height
	int channels;	//img->data canales (si es 3 no tiene Alpa)
	int size;		//img->data Total de px
	char* data; 	//NO USAR plox
} JPG;

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




int main(int argc, char** argv){


    //getOpt
	int opt;
	int nPNG = 1;
	char* archivoConv = malloc(sizeof(char) * 40);
	int umbralClass = 0;
    int umbralBin = 0;
	int bFlag = 0;

	//OPCIONES GETOPT
	while ((opt = getopt (argc, argv, "c:m:n:u:b")) != -1)
		switch (opt){
			case 'c':
			nPNG = atoi(optarg);
			break;
			case 'm':
			strcpy(archivoConv, optarg);
			break;
			case 'n':
			umbralClass = atoi(optarg);
			break;
            case 'u':
			umbralBin = atoi(optarg);
			break;
			case 'b':
			bFlag = 1;
			break;
			case '?':
			if (optopt == 'c')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (optopt == 'm')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint (optopt))
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf (stderr,
	   			"Unknown option character `\\x%x'.\n",
	   			optopt);
			return 1;
			default:
			abort ();
	}   


    char bufferNombre[50];
    char buffer[12];


    JPG* img;
    JPG* imgGray;
    JPG* imgMask;
    JPG* imgBin;

    //Lista de imagenes PNG
	JPG** listaImg = (JPG**)malloc(sizeof(JPG*) * nPNG);
	for (int i = 0; i < nPNG; ++i){listaImg[i] = (JPG*)malloc(sizeof(JPG));}


    //For cada imagen
	for (int i = 0; i < nPNG; ++i){	

        //Nombre de imagen
        strcpy(bufferNombre, "imagen_");
        sprintf(buffer, "%d",i+1);
        strcat(bufferNombre, buffer);
        strcat(bufferNombre, ".jpg");
        listaImg[i] = leerJPG(bufferNombre); //Leer imagen

        // GRAY
        imgGray = (JPG*)malloc(sizeof(JPG));
        imgGray->height = listaImg[i]->height;
        imgGray->width = listaImg[i]->width;
        imgGray->channels = listaImg[i]->channels == 4 ? 2 :1;
        imgGray->size = imgGray->height * imgGray->width * imgGray->channels;
        imgGray->data = (char*)malloc(sizeof(char) * imgGray->size);

        escalaGris(listaImg[i], imgGray);

        // MASK
        imgMask = (JPG*)malloc(sizeof(JPG));
        imgMask->height = imgGray->height;
        imgMask->width = imgGray->width;
        imgMask->channels = imgGray->channels == 4 ? 2 :1;
        imgMask->size = imgMask->height * imgMask->width * imgMask->channels;
        imgMask->data = (char*)malloc(sizeof(char) * imgMask->size);

        filtro(imgGray, imgMask, archivoConv);

        // Binarziacion
        imgBin = (JPG*)malloc(sizeof(JPG));
        imgBin->height = imgMask->height;
        imgBin->width = imgMask->width;
        imgBin->channels = imgMask->channels == 4 ? 2 :1;
        imgBin->size = imgBin->height * imgBin->width * imgBin->channels;
        imgBin->data = (char*)malloc(sizeof(char) * imgBin->size);

        binarizacion(imgMask, imgBin, umbralBin);

        //Classificacion
        int clas = clasificacion(imgBin, umbralClass);

        strcpy(bufferNombre, "imagenOut_");
        sprintf(buffer, "%d",i+1);
        strcat(bufferNombre, buffer);
        strcat(bufferNombre, ".jpg");

        
        if(bFlag == 1){
            if(i==0){
                printf("---------------------------\n");
                printf("|imagen\t\tnearly black|\n");
                printf("---------------------------\n");
            }
            if(clas == 1){
                printf("imagen_%d.jpg\t\tyes\n", i+1);
            }
            else if(clas == 0){
                printf("imagen_%d.jpg\t\tno\n", i+1);
            }
        }


        // WRITE
        // escribirJPG(imgGray, "imagenGray.jpg");
        // escribirJPG(imgMask, "imagenMask.jpg");
        escribirJPG(imgBin, bufferNombre);

    }



    // img = leerJPG("test2.jpg");

    // // GRAY
    // imgGray = (JPG*)malloc(sizeof(JPG));
    // imgGray->height = img->height;
    // imgGray->width = img->width;
    // imgGray->channels = img->channels == 4 ? 2 :1;
    // imgGray->size = imgGray->height * imgGray->width * imgGray->channels;

    // imgGray->data = (char*)malloc(sizeof(char) * imgGray->size);

    // escalaGris(img, imgGray);


    // // MASK

    // imgMask = (JPG*)malloc(sizeof(JPG));
    // imgMask->height = imgGray->height;
    // imgMask->width = imgGray->width;
    // imgMask->channels = imgGray->channels == 4 ? 2 :1;
    // imgMask->size = imgMask->height * imgMask->width * imgMask->channels;
    // imgMask->data = (char*)malloc(sizeof(char) * imgMask->size);

    // filtro(imgGray, imgMask, "mask");

    // // Binarziacion
    // imgBin = (JPG*)malloc(sizeof(JPG));
    // imgBin->height = imgMask->height;
    // imgBin->width = imgMask->width;
    // imgBin->channels = imgMask->channels == 4 ? 2 :1;
    // imgBin->size = imgBin->height * imgBin->width * imgBin->channels;
    // imgBin->data = (char*)malloc(sizeof(char) * imgBin->size);


    // binarizacion(imgMask, imgBin, 100);


    // //Classificacion
    // int clas = clasificacion(imgBin, 100);

    // if(clas == 1){
    //     printf("Imagen es nearly BLACC\n");
    // }
    // else{
    //     printf("No es nearly BLACC\n");
    // }


    // // WRITE
    // escribirJPG(imgGray, "imagenGray.jpg");
    // escribirJPG(imgMask, "imagenMask.jpg");
    // escribirJPG(imgBin, "imagenBin.jpg");
}