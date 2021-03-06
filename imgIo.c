/*
    functions readImg and writeImg to read and write pgm images
    Author  :   Mohamed Martini
    Date    :   03/10/2021
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


unsigned char * readImg(char imgName[])
    /*  input: image_path
        output: pointer to image 1D array  */
{
    unsigned sizeX; //image width
    unsigned sizeY; //image height
    unsigned char *image; //image array
    unsigned levels;

    /* Read Image */
    FILE *iFile = fopen(imgName,"r");
    if(iFile==0) return NULL;
    if (3 != fscanf(iFile, "P5 %d %d %d ", &sizeX, &sizeY, &levels)) return NULL;
    image=(unsigned char *) malloc(sizeX * sizeY);

    fread(image, sizeof(unsigned char), sizeX * sizeY, iFile); // dst, chunk size, num chunks, src
    fclose(iFile);

    return image;
}

void writeImg(char imgName[], unsigned char *image, unsigned sizeX, unsigned sizeY)
{
    /*  input: imagename, image 1D array, width, height
        output: image_name.pgm  */

    // turn image from 1D to 2D array
    unsigned char image2D [sizeX][sizeY];
    memcpy(image2D, image, sizeX * sizeY * sizeof(unsigned char));  // dst, src, size


    FILE *iFile = fopen(imgName,"w");
    if(iFile==0) return; //error handling
    fprintf(iFile, "P5 %d %d %d ", sizeX, sizeY, 255);//write header
    fwrite(image2D, sizeof(unsigned char), sizeX * sizeY, iFile); //write binary image
    fclose(iFile);
    return;
}
