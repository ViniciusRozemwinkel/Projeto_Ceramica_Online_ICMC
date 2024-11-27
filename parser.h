#ifndef PARSER_H_
#define PARSER_H_

#define POINTS_PER_LINE 30.0
#define POINTS_PER_CIRCLE 84
#define DISTANCE 1
#define SUPPORT_SIZE 5
#define BOARD_HEIGHT 2

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <algorithm>

#include "structure.h"

using namespace std;

kiln *initKiln(string filename);

void printKilnInformation(kiln *k);

void calculateAllPossibleHeights(kiln *k);

void printAllPossibleHeights(kiln *k);

item *initItems(string filename, int *numberOfItems);

void printItemsInformation(item *items, int numberOfItems);

void boundingBox(item *items, int numberOfItems);

void printBoundingBox(item *items, int numberOfItems);

void calculateArea(item *items, int numberOfItems);

void printArea(item *items, int numberOfItems);

coord newCoord(double x, double y);

void lineToPoints(vector<coord> &vertices, coord a, coord b);

void printNFP(item *items, int i, int j);

void calculateNFP(item *items, int numberOfItems);

void drawNFP(item *items, int i, int j);


#endif