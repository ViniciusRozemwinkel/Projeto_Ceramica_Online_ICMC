#ifndef STRUCTURE_H_
#define STRUCTURE_H_

#include <string.h>
#include <iostream>
#include <vector>

#define ZERO 1E-8
#define EPS 1E-4
#define PI 3.14159265359

using namespace std;

typedef struct {

    double length;
    double width;
    double height;
    
} layer;

typedef struct {

    double length;
    double width;
    double height;

    vector<layer> layers;
    vector<double> differentHeights;
    vector<double> possibleHeights;

} kiln;

typedef struct {

    double x;
    double y;

} coord;

typedef enum {

    RECTANGLE = 1,
    TRIANGLE = 2,
    CIRCLE = 3

} itemType;

typedef struct {

    int numberOfVertices;
    vector<coord> vertices;
    
} polygon;

typedef struct {
    
    int id;
    char description[100];
    int type;
    int quantity;
    double height;

    // Circle
    double radius;
    
    // Rectangle
    double length;
    double width;
    
    // Equilateral triangle
    double side;
    
    struct {
        coord lu, rb;
    } boundingBox;
    
    vector<polygon> NFP;
    
    double area;

} item;

typedef struct {

    int id;
    coord p;
    int layer;

} solution;

typedef struct {

    vector<solution> solutions;

    //stores the length of each layer (i.e., the solution value)
    vector <double> layerLengths;

} solutionPool;

#endif