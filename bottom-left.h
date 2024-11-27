#ifndef BOTTOM_LEFT_H_
#define BOTTOM_LEFT_H_

#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include <vector>

#include "structure.h"
#include "parser.h"

using namespace std;

int d_function(coord a, coord b, coord p);

vector<int> generateOrder(item *items, int numberOfItems);

void printOrder(item *items, vector<int> order);

int minimumFittingHeight(double itemHeight, kiln *k);

solutionPool solve(item *items, int numberOfItems, vector<int> order, kiln *k);

int checkBoundingBox(item *items, solution sol, kiln *k);

int checkBoundingBox(item *items, int id, double x, double y, int layer, kiln *k);

int checkPastSolutions(double x, double y, int layer, solutionPool sp);

int checkSolution(item *items, int id, double x, double y, int layer, solutionPool sp);

void drawSolution(item *items, solutionPool sp, kiln *k, char *fileName, int numberOfItems);

void drawStep(item *items, solutionPool sp, vector<coord> pointsNFP, int newItem);

#endif