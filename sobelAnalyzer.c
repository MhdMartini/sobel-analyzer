/*
    Script to analyze the performance of the Sobel Edge Detection Filter on noisy image and noisy images after smoothing.
    Author  :   Mohamed Martini
    Date    :   04/04/2021
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "imgIo.h"
#include "imgOp.h"
#include "sobel.h"

char * path = "test_images/cake.pgm";
unsigned threshold = 55;  // default value. Optimized for mri
unsigned NOISE[4] = {8, 16, 32, 64};  // levels of noise to be applied
unsigned char * gTruth, * img, * imgSobelNorm, * imgSobelN1, * imgSobelN2, * imgSobelN3, * imgSobelN4;
unsigned * shape;
unsigned sizeX, sizeY;
unsigned SMOOTH[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};  // averaging filter
bool OUTPUT = 0;
bool SOBEL = 0;  // 0: apply sobel only, no analysis, 1: analyze


void print_help(){
    // print help menu
    printf("\n****************************************** SOBEL ANALYZER ******************************************\n\n");
    printf("Please run the sobelAnalyzer as follows:\n\n\t./sobelAnalyzer [--path IMAGE_PATH] [--threshold THRESHOLD] [-output] [-sobel]\n");
    printf("\n\t--path\t\t-str- Path to the Sobel filter input image. Default: 'test_images/cake.pgm'");
    printf("\n\t--threshold\t-unsigned- Threshold to binarize the Sobel filtered input image. Default: 55\n");
    printf("\t-output\t\tflag to save output images. Does not apply if -sobel is used.\n");
    printf("\t-sobel\t\tflag to apply the Sobel filter with no analysis. Sobel and thresholded Sobel images are saved.\n");
}

void analyze(unsigned char * imgNoisy, unsigned level, char kind[]){
    // apply sobel, and find best threshold for binarization

    unsigned char * imgSobelNoisy = imgSobel(imgNoisy, threshold, 0, sizeX, sizeY);
    unsigned char * imgSobelNoisyBin;
    unsigned char * imgSobelNoisyBinBest;
    unsigned threshTemp;
    unsigned threshTempBest = 0;
    float acc;
    float accBest = 0;
    char outName[100];  // to save noisy image
    char outNameBin[100];

    snprintf(outName, sizeof(outName), "output/%s_at_%u.pgm", kind, level);
    if (OUTPUT){
        writeImg(outName, imgNoisy, sizeX, sizeY);
    }
    for (unsigned threshTemp = 0; threshTemp < 255; threshTemp ++){
        imgSobelNoisyBin = imgBin(imgSobelNoisy, threshTemp, sizeX, sizeY);
        acc = imgsComp(gTruth, imgSobelNoisyBin, sizeX, sizeY);
        if (acc > accBest){
            accBest = acc;
            threshTempBest = threshTemp;
            imgSobelNoisyBinBest = imgSobelNoisyBin;
        }
    }
    printf("\tBest Accuracy:\t%.0f%%\n", accBest);
    printf("\tThreshold:\t%u\n", threshTempBest);

    snprintf(outNameBin, sizeof(outNameBin), "output/%s_Best_threshold_at_%u.pgm", kind, level);
    if (OUTPUT){
        writeImg(outNameBin, imgSobelNoisyBinBest, sizeX, sizeY);
    }
}

int main(int argc, char *argv[]){
    // parse input arguments
    for (unsigned i = 1; i < argc; i++){
        if (strcmp(argv[i], "--path") == 0){
            path = argv[i + 1];
        }
        else if (strcmp(argv[i], "--threshold") == 0){
            long temp = strtol(argv[i + 1], NULL, 10);
            threshold = (unsigned) temp;
        }

        else if (strcmp(argv[i], "-output") == 0){
            OUTPUT = 1;
        }
        else if (strcmp(argv[i], "-sobel") == 0){
            SOBEL = 1;
        }
        else if (strcmp(argv[i], "--help") == 0){
            print_help();
            return 0;
        }
    }

    // read image and get its size
    img = readImg(path);
    shape = get_size(path);
    sizeX = shape[0];
    sizeY = shape[1];

    if (SOBEL){
        // if -sobel is flagged, apply sobel to image, save it, and leave
        imgSobel(img, threshold, 1, sizeX, sizeY);
        return 0;
    }

    // save ground truth
    imgSobelNorm = imgSobel(img, threshold, OUTPUT, sizeX, sizeY);
    gTruth = imgBin(imgSobelNorm, threshold, sizeX, sizeY);

    // apply four levels of noise, and analyze the sobel filter at each level.
    for (unsigned i = 0; i < sizeof(NOISE)/sizeof(NOISE[0]); i++){

        unsigned char * temp = imgNoise(img, NOISE[i], sizeX, sizeY);
        printf("\n* Noise level '%u'. Original:\n", NOISE[i]);
        analyze(temp, NOISE[i], "Noisy");

        unsigned char * imgPadded = imgPad(temp, sizeX, sizeY);
        signed * imgSmoothed = imgConv(imgPadded, SMOOTH, sizeX + 2, sizeY + 2);  // result is not padded
        unsigned char * imgSmoothedN = normalize(imgSmoothed, sizeX, sizeY);
        printf("  Noise level '%u'. Smoothed:\n", NOISE[i]);
        analyze(imgSmoothedN, NOISE[i], "Smoothed");

        printf("%s", "=============================\n");
    }
}
