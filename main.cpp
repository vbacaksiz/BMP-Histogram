#include <iostream>
#include <conio.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <array>
#include <math.h>

using namespace std;

int valueRecords[256][2] = {0};
int pictureWidth = 0;
int pictureHeight = 0;
int maxHistogramGraphicValueBasamak2 = 0;

void createForHistogramFile()
{
    int i=0;
    ofstream writeFileForHistogram("histogram.txt", ofstream::trunc);
    for(i=0;i<256;i++)
    {
        writeFileForHistogram << valueRecords[i][0] << "\t" << valueRecords[i][1] << endl;
    }
}

void printYSpaces(int maxHistogramGraphicValueBasamak, ofstream &histogramGraphic)
{
    int j=0;
    for(j=0;j<(maxHistogramGraphicValueBasamak2-maxHistogramGraphicValueBasamak)+1;j++)
    {
        histogramGraphic << "-";
    }
}

void printXSpaces(int maxHistogramGraphicValueBasamak, ofstream &histogramGraphic)
{
    int j=0;
    for(j=0;j<(maxHistogramGraphicValueBasamak2-maxHistogramGraphicValueBasamak)+1;j++)
    {
        histogramGraphic << " ";
    }
}

void printYAxis(int maxHistogramGraphicValue, int i, int maxHistogramGraphicValueBasamak, ofstream &histogramGraphic)
{
    histogramGraphic << maxHistogramGraphicValue-i;
    printYSpaces(maxHistogramGraphicValueBasamak, histogramGraphic);
    histogramGraphic << "|";
    int j = 0;
    for(j=0;j<=256;j++)
    {
        if(valueRecords[j][1] >= maxHistogramGraphicValue-i)
            histogramGraphic << ".";
        else
            histogramGraphic << " ";
    }
}

void printXAxis5(int maxHistogramGraphicValueBasamak, ofstream &histogramGraphic)
{
    histogramGraphic << "-";
    int j=0;
    for(j=0;j<256;j++)
        histogramGraphic<< "-";
    histogramGraphic << endl;
    histogramGraphic << "   ";
    printXSpaces(maxHistogramGraphicValueBasamak, histogramGraphic);
    for(j=0;j<256;j++)
        histogramGraphic << "|";
    histogramGraphic << endl;
    histogramGraphic << "   ";
    printXSpaces(maxHistogramGraphicValueBasamak, histogramGraphic);
    for(j=0;j<10;j+=5)
        histogramGraphic << j << "   ";
    for(j=10;j<100;j+=5)
        histogramGraphic << j << "   ";
    for(j=100; j<256;j+=5)
        histogramGraphic << j << "  "; 
}

void createGraphic()
{
    int i=0;
    int maxHistogramGraphicValue = 0;
    for(i=0;i<256;i++)
    {
        if(valueRecords[i][1] > maxHistogramGraphicValue)
            maxHistogramGraphicValue = valueRecords[i][1];
    }

    int maxHistogramGraphicValueForOperations = maxHistogramGraphicValue;
    int maxHistogramGraphicValueBasamak = 1;
    while(maxHistogramGraphicValueForOperations>=10)
    {
        maxHistogramGraphicValueForOperations = maxHistogramGraphicValueForOperations/10;
        maxHistogramGraphicValueBasamak++;
    }
    maxHistogramGraphicValueBasamak2 = maxHistogramGraphicValueBasamak;

    ofstream histogramGraphic("histogramGraphic.txt", ofstream::trunc);
    int j=0;
    for(i=0;i<maxHistogramGraphicValue;i++)
    {
        int bolum = pow(10, maxHistogramGraphicValueBasamak-1);
        if(maxHistogramGraphicValue-i == bolum-1)
        {
            maxHistogramGraphicValueBasamak -= 1;
        }
        for(j=0;j<=256;j++)
        {
            if(maxHistogramGraphicValue-i == valueRecords[j][1])
            {
                printYAxis(maxHistogramGraphicValue, i, maxHistogramGraphicValueBasamak, histogramGraphic);
                histogramGraphic << endl;
                j=256;
            }
        }
    }

    histogramGraphic << "0";
    printYSpaces(maxHistogramGraphicValueBasamak, histogramGraphic);
    histogramGraphic << "|";

    printXAxis5(maxHistogramGraphicValueBasamak, histogramGraphic);
}

unsigned char* readBMP(char* filename)
{
    int i;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];
    unsigned char header[4*256];

    fread(info, sizeof(unsigned char), 54, f); 

    ofstream writeFile("imageDetails.txt", ofstream::trunc);
    pictureWidth = *(int*)&info[18];
    pictureHeight = *(int*)&info[22];
    writeFile << "Width     : " << pictureWidth << endl;
    writeFile << "Height    : " << pictureHeight << endl;

    fread(header, sizeof(unsigned char), 4*256, f);

    int size = pictureWidth * pictureHeight;
    unsigned char* data = new unsigned char[size];

    fread(data, sizeof(unsigned char), size, f); 
    fclose(f);
    
    for(i = 0; i < 256; i++)
    {
        valueRecords[i][0] = i; 
    }
    int j=0;
    int k = 0;
    for(i = 0; i < pictureHeight; i++)
    {
        for(j=0; j < pictureWidth; j++)
        {
            for(k=0; k<pictureWidth*pictureHeight; k++)
                if((unsigned int)data[i*pictureWidth+j] == (unsigned int)data[k])
                {
                    valueRecords[(unsigned int)data[i*pictureWidth+j]][1]++;
                    k = pictureWidth*pictureHeight;
                }
            writeFile << (unsigned int)data[i*pictureWidth + j] << "\t" ;
        }
    }
    return data;
}

float calculateHistogramAverage(int pictureWidth, int pictureHeight)
{
    int i=0;
    float histogramAverage=0;
    float numOfValues = pictureWidth*pictureHeight;
    for(i=0;i<256;i++)
    {
        histogramAverage += valueRecords[i][0]*valueRecords[i][1];
    }
    histogramAverage /= numOfValues;
    return histogramAverage;
}

float calculateHistogramVariance(int pictureWidth, int pictureHeight, float histogramAverage)
{
    int i=0;
    float histogramVariance = 0;
    float numOfValues = pictureWidth*pictureHeight;
    for(i=0;i<256;i++)
    {
        histogramVariance += pow((valueRecords[i][0]-histogramAverage), 2)*valueRecords[i][1];
    }
    histogramVariance /= (numOfValues-1);
    return histogramVariance;
}

float calculateHistogramStandardDeviation(float histogramVariance)
{
    float histogramStandardDeviation = sqrt(histogramVariance);
    return histogramStandardDeviation;
}

void printResultsTerminalAndFile(float average, float variance, float standartDeviation)
{
    ofstream programResultFile("programResult.txt", ofstream::trunc);
    programResultFile << endl;
    programResultFile << "Histogram Average              : " << average << endl << endl;
    programResultFile << "Histogram Variance             : " << variance << endl << endl;
    programResultFile << "Histogram Standard Deviation   : " << standartDeviation << endl << endl;
    cout << endl;
    cout << "\t" << "Histogram Average              : " << average << endl << endl;
    cout << "\t" << "Histogram Variance             : " << variance << endl << endl;
    cout << "\t" << "Histogram Standard Deviation   : " << standartDeviation << endl << endl;
}

void printProgramRules()
{
    cout << "**********************************************************************************" << endl << endl;
    cout << "\tImage details              => 'imageDetails.txt' " << endl << endl;
    cout << "\tHistogram                  => 'histogram.txt' " << endl << endl;
    cout << "\tHistogram Graphics         => 'histogramGraphics.txt" << endl << endl;
    cout << "\tProgram Result             => 'programResult.txt'" << endl << endl; 
}

int main()
{
    char filename[17]= "image.bmp";
    unsigned char* data = readBMP(filename);
    createForHistogramFile();
    createGraphic();
    float histogramAverage = calculateHistogramAverage(pictureWidth, pictureHeight);
    float histogramVariance = calculateHistogramVariance(pictureWidth, pictureHeight, histogramAverage);
    float histogramStandardDeviation = calculateHistogramStandardDeviation(histogramVariance);
    printResultsTerminalAndFile(histogramAverage, histogramVariance, histogramStandardDeviation);
    printProgramRules();
    getch();
    return 0;
}
