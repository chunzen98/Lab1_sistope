#include "funciones.h"

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