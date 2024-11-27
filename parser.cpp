#include "parser.h"

using namespace std;

kiln *initKiln(string filename) {

	kiln *newKiln;
	newKiln = (kiln *)malloc(sizeof(kiln));
	if(!newKiln) {
		cout << "Error: Internal error while allocating memory for the kiln. \n";
		exit(EXIT_FAILURE);
	}

	ifstream file;
	file.open(filename.c_str(), ios::in);
	if (!file.is_open()) {
		cout << "Error: Cannot open file " << filename << ". Please check the file name and try again. \n";
		exit(EXIT_FAILURE);
	}

	file >> newKiln->length;
	file >> newKiln->width;
	file >> newKiln->height;
	
	if (newKiln->length < (2*(SUPPORT_SIZE+DISTANCE)))
		newKiln->length = (2*(SUPPORT_SIZE+DISTANCE));

	if (newKiln->width < (2*(SUPPORT_SIZE+DISTANCE)))
		newKiln->width = (2*(SUPPORT_SIZE+DISTANCE));

	int numberOfDifferentHeights;
	double aux;

	file >> numberOfDifferentHeights;

	for (int i=0; i< numberOfDifferentHeights ; i++) {
		file >> aux;
		newKiln->differentHeights.push_back(aux);
	}

	file.close();

	return newKiln;

}

void printKilnInformation(kiln *k) {

	if (!k) {
		cout << "Error: Cannot print kiln information. Invalid pointer. \n";
		exit(EXIT_FAILURE);
	}

	cout << "Kiln: \n - Length: " << k->length << "\n - Width: " << k->width << 
			"\n - Height: " << k->height << "\n - Different layer heights: ";
	
	for (int i = 0; i < k->differentHeights.size() ; i++) {
		cout << k->differentHeights[i] << "  ";
	}
	cout << "\n";

}

void calculateAllPossibleHeights(kiln *k) {

	if (!k) {
		cout << "Error: Cannot calculate possible heights. Invalid pointer. \n";
		exit(EXIT_FAILURE);
	}

	int h = (int) (k->height);


	int max, aux;
	k->possibleHeights.clear();
	k->possibleHeights.reserve(2*h);
	k->possibleHeights.push_back(0);

    for (int w=1; w<= k->height; w++) {

        max = 0;
        for (int i=0 ; i< k->differentHeights.size(); i++ ) {
            aux = w - k->differentHeights.at(i);
            if (aux >= 0) {
                if (k->possibleHeights.at(aux) + k->differentHeights.at(i) > max)
                    max = k->possibleHeights.at(aux) + k->differentHeights.at(i);
            }
        }
        k->possibleHeights.push_back(max);
    }

    //deleting duplicates
    sort( k->possibleHeights.begin(), k->possibleHeights.end() );
	k->possibleHeights.erase(unique( k->possibleHeights.begin(), k->possibleHeights.end() ), k->possibleHeights.end());
}

void printAllPossibleHeights(kiln *k) {

	if (!k) {
		cout << "Error: Cannot print possible heights. Invalid pointer. \n";
		exit(EXIT_FAILURE);
	}

	cout << " - Possible heights: ";
	
	for (int i = 0; i < k->possibleHeights.size() ; i++) {
		cout << k->possibleHeights[i] << "  ";
	}
	cout << "\n";

}

item *initItems(string filename, int *numberOfItems) {

	ifstream file;
	file.open(filename.c_str(), ios::in);
	if (!file.is_open()) {
		cout << "Error: Cannot open file " << filename << ". Please check the file name and try again. \n";
		exit(EXIT_FAILURE);
	}

	file >> *numberOfItems;
	if (*numberOfItems <= 0) {
		cout << "Error: Please check the number of items and try again. \n ";
		exit(EXIT_FAILURE);
	}

	item *items = (item *)malloc(((*numberOfItems)+1) * sizeof(item));
	if(!items) {
		cout << "Error: Internal error while allocating memory for the items. \n";
		exit(EXIT_FAILURE);
	}

	items[0].id = 0;
	items[0].type = RECTANGLE;
	items[0].quantity = 1;
	items[0].length = SUPPORT_SIZE + DISTANCE;
	items[0].width = SUPPORT_SIZE + DISTANCE;
	items[0].height = 1;
	strcpy(items[0].description, "Suporte");

	for (int i = 1 ; i <= (*numberOfItems) ; i++) {

		file >> items[i].id;
		file >> items[i].type;
		file >> items[i].quantity;

		if (items[i].type == RECTANGLE) {
			file >> items[i].length;
			file >> items[i].width;
			items[i].length += DISTANCE;
			items[i].width += DISTANCE;
		} else if (items[i].type == TRIANGLE) {
			file >> items[i].side;
			items[i].side += DISTANCE;
		} else if (items[i].type == CIRCLE) {
			file >> items[i].radius;
			items[i].radius += DISTANCE/2.0;
		}

		file >> items[i].height;


		string aux;
		file.ignore(1,' ');
		getline(file, aux);
		strcpy(items[i].description, aux.c_str());

	}

	file.close();

	return items;
}

void printItemsInformation(item *items, int numberOfItems) {

	if (!items) {
		cout << "Error: Cannot print items information. Invalid pointer. \n";
		exit(EXIT_FAILURE);
	}

	cout << "Items: \n - Number of items: " << numberOfItems << "\n";
	for (int i = 1; i <= numberOfItems ; i++) {
		cout << "   - Item #" << i << ": ";
		cout << items[i].id << " - '" << items[i].description << "' - Type "<< items[i].type << " - Qt. "<< items[i].quantity;
		if (items[i].type == RECTANGLE) {
			cout << " - " << items[i].length << "x" << items[i].width << " (LxW)";
		} else if (items[i].type == TRIANGLE) {
			cout << " -  " << items[i].side << " (side)";
		} else if (items[i].type == CIRCLE) {
			cout << " -  " << items[i].radius << " (radius)";
		}
		cout << " - " << items[i].height << " (height)";
		cout << "\n";
	}

}

void boundingBox(item *items, int numberOfItems) {

	if (!items) {
		cout << "Error: Cannot calculate the bounding box of each item. Invalid pointer. \n";
		exit(EXIT_FAILURE);
	}

	for (int i=1 ; i<=numberOfItems ; i++) {

		//lu: left upper
		//rb: right bottom

		if (items[i].type == RECTANGLE) {
			items[i].boundingBox.lu.x = 0.0;
			items[i].boundingBox.lu.y = items[i].width;
			items[i].boundingBox.rb.x = items[i].length;
			items[i].boundingBox.rb.y = 0.0;	
		} else if (items[i].type == TRIANGLE) {
			items[i].boundingBox.lu.x = 0.0;
			items[i].boundingBox.lu.y = (sqrt(3.0)/(2.0)) * items[i].side;
			items[i].boundingBox.rb.x = items[i].side;
			items[i].boundingBox.rb.y = 0.0;
		} else if (items[i].type == CIRCLE) {
			items[i].boundingBox.lu.x = - items[i].radius;
			items[i].boundingBox.lu.y = items[i].radius;
			items[i].boundingBox.rb.x = items[i].radius;
			items[i].boundingBox.rb.y = - items[i].radius;	
		}
		
	}

}

void printBoundingBox(item *items, int numberOfItems) {

	if (!items) {
		cout << "Error: Cannot calculate the bounding box of each item. Invalid pointer. \n";
		exit(EXIT_FAILURE);
	}

	cout << "Items: \n - Number of items: " << numberOfItems << "\n";
	for (int i=1 ; i<=numberOfItems ; i++) {

		cout << "   - Item #" << i << ": ";
		cout << "lu: (" << items[i].boundingBox.lu.x << ", " << items[i].boundingBox.lu.y << ") ";
		cout << "rb: (" << items[i].boundingBox.rb.x << ", " << items[i].boundingBox.rb.y << ") \n"; 
		
	}

}

void calculateArea(item *items, int numberOfItems) {

	if (!items) {
		cout << "Error: Cannot calculate the area of each item. Invalid pointer. \n";
		exit(EXIT_FAILURE);
	}

	for (int i=0; i<=numberOfItems ; i++) {

		if (items[i].type == RECTANGLE) {
			items[i].area = items[i].length * items[i].width;
		} else if (items[i].type == TRIANGLE) {
			items[i].area = items[i].side * (sqrt(3.0)/(2.0)) * items[i].side * 0.5;
		} else if (items[i].type == CIRCLE) {
			items[i].area = PI * items[i].radius * items[i].radius;
		}
		
	}

}

void printArea(item *items, int numberOfItems) {

	if (!items) {
		cout << "Error: Cannot calculate the area of each item. Invalid pointer. \n";
		exit(EXIT_FAILURE);
	}

	cout << "Area: \n - Number of items: " << numberOfItems << "\n";
	for (int i=0 ; i<=numberOfItems ; i++) {
		cout << "   - Item #" << i << ": " << items[i].area << "\n";
	}

}

coord newCoord(double x, double y) {
	
	coord c;
	
	if (fabs(x) < EPS)
		c.x = 0.0;
	else
		c.x = x;
	if (fabs(y) < EPS)
		c.y = 0.0;
	else
		c.y = y;
	
	return c;

}

void lineToPoints(vector<coord> &vertices, coord a, coord b) {

	double length_x = b.x - a.x;
	double length_y = b.y - a.y;

	double inc_x = length_x / (POINTS_PER_LINE -1 );
	double inc_y = length_y / (POINTS_PER_LINE -1 );

	//exclude first and last one
	for (int i=1 ; i<(POINTS_PER_LINE -1) ; i++) {
		vertices.push_back(newCoord(a.x + i*inc_x, a.y+i*inc_y));
	}

}

void printNFP(item *items, int i, int j) {

	if (!items) {
		cout << "Error: Cannot print the NFP of pair "<< i << ", " << j << ". Invalid pointer. \n";
		exit(EXIT_FAILURE);
	}

	cout << "\n\n - NFP " << i << ", " << j << ":\n";
	for (int k=0; k<items[i].NFP[j].numberOfVertices ; k++) {
		cout << "   (" << items[i].NFP[j].vertices[k].x << ", " << items[i].NFP[j].vertices[k].y << ")\n";
	}


}

void calculateNFP(item *items, int numberOfItems) {

	if (!items) {
		cout << "Error: Cannot calculate the NPF of each pair of items. Invalid pointer. \n";
		exit(EXIT_FAILURE);
	}

	for (int i=0; i<=numberOfItems ; i++) {

		items[i].NFP.clear();
		items[i].NFP.resize(numberOfItems+1);
		
		//NFP of item i and item j
		for (int j=0; j<=numberOfItems ; j++) {			

			if (items[i].type == RECTANGLE ) {
			
				if (items[j].type == RECTANGLE) {
			
					//insert the vertices
					items[i].NFP[j].vertices.push_back(newCoord(-items[j].length, -items[j].width));
					lineToPoints(items[i].NFP[j].vertices, newCoord(-items[j].length, -items[j].width), newCoord(0, -items[j].width));					
					items[i].NFP[j].vertices.push_back(newCoord(0, -items[j].width));
					lineToPoints(items[i].NFP[j].vertices, newCoord(0, -items[j].width), newCoord(items[i].length, -items[j].width));
					items[i].NFP[j].vertices.push_back(newCoord(items[i].length, -items[j].width));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[i].length, -items[j].width), newCoord(items[i].length, 0));
					items[i].NFP[j].vertices.push_back(newCoord(items[i].length, 0));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[i].length, 0), newCoord(items[i].length, items[i].width));
					items[i].NFP[j].vertices.push_back(newCoord(items[i].length, items[i].width));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[i].length, items[i].width), newCoord(0, items[i].width));
					items[i].NFP[j].vertices.push_back(newCoord(0, items[i].width));
					lineToPoints(items[i].NFP[j].vertices, newCoord(0, items[i].width), newCoord(-items[j].length, items[i].width));
					items[i].NFP[j].vertices.push_back(newCoord(-items[j].length, items[i].width));
					lineToPoints(items[i].NFP[j].vertices, newCoord(-items[j].length, items[i].width), newCoord(-items[j].length, 0));
					items[i].NFP[j].vertices.push_back(newCoord(-items[j].length, 0));
					lineToPoints(items[i].NFP[j].vertices, newCoord(-items[j].length, 0), newCoord(-items[j].length, -items[j].width));
					
					//update the number of vertices of the final NFP between i and j
					items[i].NFP[j].numberOfVertices = items[i].NFP[j].vertices.size();


				} else if (items[j].type == TRIANGLE) {
			
					//insert the vertices
					items[i].NFP[j].vertices.push_back(newCoord(-items[j].side, items[i].width));
					lineToPoints(items[i].NFP[j].vertices, newCoord(-items[j].side, items[i].width), newCoord(-items[j].side, 0));
					items[i].NFP[j].vertices.push_back(newCoord(-items[j].side, 0));
					lineToPoints(items[i].NFP[j].vertices, newCoord(-items[j].side, 0), newCoord(-(items[j].side/(2.0)), - (sqrt(3.0)/(2.0))*items[j].side));
					items[i].NFP[j].vertices.push_back(newCoord(-(items[j].side/(2.0)), - (sqrt(3.0)/(2.0))*items[j].side));
					lineToPoints(items[i].NFP[j].vertices, newCoord(-(items[j].side/(2.0)), - (sqrt(3.0)/(2.0))*items[j].side), newCoord(items[i].length - (items[j].side/(2.0)), - (sqrt(3.0)/(2.0))*items[j].side));
					items[i].NFP[j].vertices.push_back(newCoord(items[i].length - (items[j].side/(2.0)), - (sqrt(3.0)/(2.0))*items[j].side));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[i].length - (items[j].side/(2.0)), - (sqrt(3.0)/(2.0))*items[j].side), newCoord(items[i].length, 0));
					items[i].NFP[j].vertices.push_back(newCoord(items[i].length, 0));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[i].length, 0), newCoord(items[i].length, items[i].width));
					items[i].NFP[j].vertices.push_back(newCoord(items[i].length, items[i].width));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[i].length, items[i].width), newCoord(0, items[i].width));
					items[i].NFP[j].vertices.push_back(newCoord(0, items[i].width));
					lineToPoints(items[i].NFP[j].vertices, newCoord(0, items[i].width), newCoord(-items[j].side, items[i].width));

					//update the number of vertices of the final NFP between i and j
					items[i].NFP[j].numberOfVertices = items[i].NFP[j].vertices.size();

				} else if (items[j].type == CIRCLE) {

					double inc, t;
					inc = 2*PI/(double)POINTS_PER_CIRCLE;

					t = 0;
					for(int k = 0; k<(POINTS_PER_CIRCLE / 4); k++){
						items[i].NFP[j].vertices.push_back(newCoord(items[j].radius * cos(t + (POINTS_PER_CIRCLE/4 * inc)),
							items[i].width + items[j].radius * sin(t + (POINTS_PER_CIRCLE/4 * inc)) ));
						t = (k+1) * inc;
					}

					items[i].NFP[j].vertices.push_back(newCoord(-items[j].radius, items[i].width));
					lineToPoints(items[i].NFP[j].vertices, newCoord(-items[j].radius, items[i].width), newCoord(-items[j].radius, 0));
					items[i].NFP[j].vertices.push_back(newCoord(-items[j].radius, 0));
					
					t = 0;
					for(int k = 0; k<(POINTS_PER_CIRCLE / 4); k++){
						items[i].NFP[j].vertices.push_back(newCoord(items[j].radius * cos(t + (2*POINTS_PER_CIRCLE/4 * inc)),
							items[j].radius * sin(t + (2*POINTS_PER_CIRCLE/4 * inc)) ));
						t = (k+1) * inc;
					}

					items[i].NFP[j].vertices.push_back(newCoord(0, -items[j].radius));
					lineToPoints(items[i].NFP[j].vertices, newCoord(0, -items[j].radius), newCoord(items[i].length, -items[j].radius));
					items[i].NFP[j].vertices.push_back(newCoord(items[i].length, -items[j].radius));
					
					t = 0;
					for(int k = 0; k<(POINTS_PER_CIRCLE / 4); k++){
						items[i].NFP[j].vertices.push_back(newCoord(items[i].length + items[j].radius * cos(t + (3*POINTS_PER_CIRCLE/4 * inc)),
							items[j].radius * sin(t + (3*POINTS_PER_CIRCLE/4 * inc)) ));
						t = (k+1) * inc;
					}

					items[i].NFP[j].vertices.push_back(newCoord(items[i].length + items[j].radius, 0));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[i].length + items[j].radius, 0), newCoord(items[i].length + items[j].radius, items[i].width));
					items[i].NFP[j].vertices.push_back(newCoord(items[i].length + items[j].radius, items[i].width));
					
					t = 0;
					for(int k = 0; k<(POINTS_PER_CIRCLE / 4); k++){
						items[i].NFP[j].vertices.push_back(newCoord(items[i].length + items[j].radius * cos(t),
							items[i].width + items[j].radius * sin(t) ));
						t = (k+1) * inc;
					}

					items[i].NFP[j].vertices.push_back(newCoord(items[i].length, items[i].width+items[j].radius));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[i].length, items[i].width+items[j].radius),newCoord(0, items[i].width+items[j].radius));
					items[i].NFP[j].vertices.push_back(newCoord(0, items[i].width+items[j].radius));					

					//update the number of vertices of the final NFP between i and j
					items[i].NFP[j].numberOfVertices = items[i].NFP[j].vertices.size();

				}
			
			} else if (items[i].type == TRIANGLE ) {
			
				if (items[j].type == RECTANGLE) {
			
					//insert the vertices
					items[i].NFP[j].vertices.push_back(newCoord(items[i].side/(2.0), (sqrt(3.0)/(2.0))*items[i].side));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[i].side/(2.0), (sqrt(3.0)/(2.0))*items[i].side), newCoord(items[i].side/(2.0) - items[j].length, (sqrt(3.0)/(2.0))*items[i].side));
					items[i].NFP[j].vertices.push_back(newCoord(items[i].side/(2.0) - items[j].length, (sqrt(3.0)/(2.0))*items[i].side));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[i].side/(2.0) - items[j].length, (sqrt(3.0)/(2.0))*items[i].side), newCoord(-items[j].length, 0));
					items[i].NFP[j].vertices.push_back(newCoord(-items[j].length, 0));
					lineToPoints(items[i].NFP[j].vertices, newCoord(-items[j].length, 0), newCoord(-items[j].length, -items[j].width));
					items[i].NFP[j].vertices.push_back(newCoord(-items[j].length, -items[j].width));
					lineToPoints(items[i].NFP[j].vertices, newCoord(-items[j].length, -items[j].width) , newCoord(items[i].side, -items[j].width));
					items[i].NFP[j].vertices.push_back(newCoord(items[i].side, -items[j].width));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[i].side, -items[j].width), newCoord(items[i].side, 0));
					items[i].NFP[j].vertices.push_back(newCoord(items[i].side, 0));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[i].side, 0), newCoord(items[i].side/(2.0), (sqrt(3.0)/(2.0))*items[i].side));

					//update the number of vertices of the final NFP between i and j
					items[i].NFP[j].numberOfVertices = items[i].NFP[j].vertices.size();

				} else if (items[j].type == TRIANGLE) {
			
					//insert the vertices
					items[i].NFP[j].vertices.push_back(newCoord((items[i].side/(2.0)), (sqrt(3.0)/(2.0))*items[i].side));
					lineToPoints(items[i].NFP[j].vertices, newCoord((items[i].side/(2.0)), (sqrt(3.0)/(2.0))*items[i].side), newCoord((items[i].side/(2.0)) - items[j].side, (sqrt(3.0)/(2.0))*items[i].side));
					items[i].NFP[j].vertices.push_back(newCoord((items[i].side/(2.0)) - items[j].side, (sqrt(3.0)/(2.0))*items[i].side));
					lineToPoints(items[i].NFP[j].vertices, newCoord((items[i].side/(2.0)) - items[j].side, (sqrt(3.0)/(2.0))*items[i].side), newCoord(-items[j].side, 0));
					items[i].NFP[j].vertices.push_back(newCoord(-items[j].side, 0));
					lineToPoints(items[i].NFP[j].vertices, newCoord(-items[j].side, 0), newCoord(-(items[j].side/(2.0)), -(sqrt(3.0)/(2.0))*items[j].side));
					items[i].NFP[j].vertices.push_back(newCoord(-(items[j].side/(2.0)), -(sqrt(3.0)/(2.0))*items[j].side));
					lineToPoints(items[i].NFP[j].vertices, newCoord(-(items[j].side/(2.0)), -(sqrt(3.0)/(2.0))*items[j].side), newCoord(items[i].side - (items[j].side/(2.0)), -(sqrt(3.0)/(2.0))*items[j].side));
					items[i].NFP[j].vertices.push_back(newCoord(items[i].side - (items[j].side/(2.0)), -(sqrt(3.0)/(2.0))*items[j].side));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[i].side - (items[j].side/(2.0)), -(sqrt(3.0)/(2.0))*items[j].side), newCoord(items[i].side, 0));
					items[i].NFP[j].vertices.push_back(newCoord(items[i].side, 0));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[i].side, 0), newCoord((items[i].side/(2.0)), (sqrt(3.0)/(2.0))*items[i].side));
					
					//update the number of vertices of the final NFP between i and j
					items[i].NFP[j].numberOfVertices = items[i].NFP[j].vertices.size();

				} else if (items[j].type == CIRCLE) {

					double inc, t;
					inc = 2*PI/(double)POINTS_PER_CIRCLE;

					items[i].NFP[j].vertices.push_back(newCoord(0, -items[j].radius));
					lineToPoints(items[i].NFP[j].vertices, newCoord(0, -items[j].radius), newCoord(items[i].side, -items[j].radius));
					items[i].NFP[j].vertices.push_back(newCoord(items[i].side, -items[j].radius));

					t = 0;
					for(int k = 3*POINTS_PER_CIRCLE/4 ; k<POINTS_PER_CIRCLE; k++){
						t = k * inc;
						items[i].NFP[j].vertices.push_back(newCoord(items[i].side + items[j].radius * cos(t),
							items[j].radius * sin(t) ));
					}

					t = 0;
					for(int k = 0 ; k<POINTS_PER_CIRCLE/12; k++){
						items[i].NFP[j].vertices.push_back(newCoord(items[i].side + items[j].radius * cos(t),
							items[j].radius * sin(t) ));
						t = (k+1) * inc;
					}

					items[i].NFP[j].vertices.push_back(newCoord(items[i].side + items[j].radius * (sqrt(3.0)/(2.0)), items[j].radius * 0.5));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[i].side + items[j].radius * (sqrt(3.0)/(2.0)), items[j].radius * 0.5), newCoord(items[i].side/(2.0) + items[j].radius * (sqrt(3.0)/(2.0)), (sqrt(3.0)/(2.0))*items[i].side + items[j].radius * 0.5));
					items[i].NFP[j].vertices.push_back(newCoord(items[i].side/(2.0) + items[j].radius * (sqrt(3.0)/(2.0)), (sqrt(3.0)/(2.0))*items[i].side + items[j].radius * 0.5));

					t = 0;
					for(int k = 3*POINTS_PER_CIRCLE/4 ; k<POINTS_PER_CIRCLE; k++){
						t = k * inc;
						items[i].NFP[j].vertices.push_back(newCoord(items[i].side/(2.0) + items[j].radius * cos(t + (POINTS_PER_CIRCLE/3 * inc)),
							sqrt(3.0)/(2.0)*items[i].side + items[j].radius * sin(t + (POINTS_PER_CIRCLE/3 * inc)) ));
					}
					t = 0;
					for(int k = 0 ; k<POINTS_PER_CIRCLE/12; k++){
						items[i].NFP[j].vertices.push_back(newCoord(items[i].side/(2.0) + items[j].radius * cos(t + (POINTS_PER_CIRCLE/3 * inc)),
							sqrt(3.0)/(2.0)*items[i].side + items[j].radius * sin(t + (POINTS_PER_CIRCLE/3 * inc)) ));
						t = (k+1) * inc;
					}

					items[i].NFP[j].vertices.push_back(newCoord(items[i].side/(2.0) - items[j].radius * (sqrt(3.0)/(2.0)), (sqrt(3.0)/(2.0))*items[i].side + items[j].radius * 0.5));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[i].side/(2.0) - items[j].radius * (sqrt(3.0)/(2.0)), (sqrt(3.0)/(2.0))*items[i].side + items[j].radius * 0.5), newCoord(-items[j].radius * (sqrt(3.0)/(2.0)), items[j].radius * 0.5));
					items[i].NFP[j].vertices.push_back(newCoord(-items[j].radius * (sqrt(3.0)/(2.0)), items[j].radius * 0.5));

					t = 0;
					for(int k = 3*POINTS_PER_CIRCLE/4 ; k<POINTS_PER_CIRCLE; k++){
						t = k * inc;
						items[i].NFP[j].vertices.push_back(newCoord(items[j].radius * cos(t + (2*POINTS_PER_CIRCLE/3 * inc)),
							items[j].radius * sin(t + (2*POINTS_PER_CIRCLE/3 * inc)) ));
					}
					t = 0;
					for(int k = 0 ; k<POINTS_PER_CIRCLE/12; k++){
						items[i].NFP[j].vertices.push_back(newCoord(items[j].radius * cos(t + (2*POINTS_PER_CIRCLE/3 * inc)),
							items[j].radius * sin(t + (2*POINTS_PER_CIRCLE/3 * inc)) ));
						t = (k+1) * inc;
					}

					//update the number of vertices of the final NFP between i and j
					items[i].NFP[j].numberOfVertices = items[i].NFP[j].vertices.size();

				}
			
			} else if (items[i].type == CIRCLE) {

				if (items[j].type == RECTANGLE) {

					//we're using the same idea of the NFP between RECTANGLE and CIRCLE
					//all we need to do is change the reference point at the end

					double inc, t;
					inc = 2*PI/(double)POINTS_PER_CIRCLE;

					t = 0;
					for(int k = 0; k<(POINTS_PER_CIRCLE / 4); k++){
						items[i].NFP[j].vertices.push_back(newCoord(items[i].radius * cos(t + (POINTS_PER_CIRCLE/4 * inc)),
							items[j].width + items[i].radius * sin(t + (POINTS_PER_CIRCLE/4 * inc)) ));
						t = (k+1) * inc;
					}

					items[i].NFP[j].vertices.push_back(newCoord(-items[i].radius, items[j].width));
					lineToPoints(items[i].NFP[j].vertices, newCoord(-items[i].radius, items[j].width), newCoord(-items[i].radius, 0));
					items[i].NFP[j].vertices.push_back(newCoord(-items[i].radius, 0));
					
					t = 0;
					for(int k = 0; k<(POINTS_PER_CIRCLE / 4); k++){
						items[i].NFP[j].vertices.push_back(newCoord(items[i].radius * cos(t + (2*POINTS_PER_CIRCLE/4 * inc)),
							items[i].radius * sin(t + (2*POINTS_PER_CIRCLE/4 * inc)) ));
						t = (k+1) * inc;
					}

					items[i].NFP[j].vertices.push_back(newCoord(0, -items[i].radius));
					lineToPoints(items[i].NFP[j].vertices, newCoord(0, -items[i].radius), newCoord(items[j].length, -items[i].radius));
					items[i].NFP[j].vertices.push_back(newCoord(items[j].length, -items[i].radius));
					
					t = 0;
					for(int k = 0; k<(POINTS_PER_CIRCLE / 4); k++){
						items[i].NFP[j].vertices.push_back(newCoord(items[j].length + items[i].radius * cos(t + (3*POINTS_PER_CIRCLE/4 * inc)),
							items[i].radius * sin(t + (3*POINTS_PER_CIRCLE/4 * inc)) ));
						t = (k+1) * inc;
					}

					items[i].NFP[j].vertices.push_back(newCoord(items[j].length + items[i].radius, 0));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[j].length + items[i].radius, 0), newCoord(items[j].length + items[i].radius, items[j].width));
					items[i].NFP[j].vertices.push_back(newCoord(items[j].length + items[i].radius, items[j].width));
					
					t = 0;
					for(int k = 0; k<(POINTS_PER_CIRCLE / 4); k++){
						items[i].NFP[j].vertices.push_back(newCoord(items[j].length + items[i].radius * cos(t),
							items[j].width + items[i].radius * sin(t) ));
						t = (k+1) * inc;
					}

					items[i].NFP[j].vertices.push_back(newCoord(items[j].length, items[j].width+items[i].radius));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[j].length, items[j].width+items[i].radius),newCoord(0, items[j].width+items[i].radius));
					items[i].NFP[j].vertices.push_back(newCoord(0, items[j].width+items[i].radius));					

					//update the number of vertices of the final NFP between i and j
					items[i].NFP[j].numberOfVertices = items[i].NFP[j].vertices.size();

					//change the reference point 
					for (int k=0; k<items[i].NFP[j].numberOfVertices; k++) {
						items[i].NFP[j].vertices[k].x -= items[j].length;
						items[i].NFP[j].vertices[k].y -= items[j].width;
					}

				} else if (items[j].type == TRIANGLE) {

					//we're using the same idea of the NFP between TRIANGLE and CIRCLE
					//all we need to do is rotate the NFP 180º

					double inc, t;
					inc = 2*PI/(double)POINTS_PER_CIRCLE;

					items[i].NFP[j].vertices.push_back(newCoord(0, -items[i].radius));
					lineToPoints(items[i].NFP[j].vertices, newCoord(0, -items[i].radius), newCoord(items[j].side, -items[i].radius));
					items[i].NFP[j].vertices.push_back(newCoord(items[j].side, -items[i].radius));

					t = 0;
					for(int k = 3*POINTS_PER_CIRCLE/4 ; k<POINTS_PER_CIRCLE; k++){
						t = k * inc;
						items[i].NFP[j].vertices.push_back(newCoord(items[j].side + items[i].radius * cos(t),
							items[i].radius * sin(t) ));
					}

					t = 0;
					for(int k = 0 ; k<POINTS_PER_CIRCLE/12; k++){
						items[i].NFP[j].vertices.push_back(newCoord(items[j].side + items[i].radius * cos(t),
							items[i].radius * sin(t) ));
						t = (k+1) * inc;
					}

					items[i].NFP[j].vertices.push_back(newCoord(items[j].side + items[i].radius * (sqrt(3.0)/(2.0)), items[i].radius * 0.5));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[j].side + items[i].radius * (sqrt(3.0)/(2.0)), items[i].radius * 0.5), newCoord(items[j].side/(2.0) + items[i].radius * (sqrt(3.0)/(2.0)), (sqrt(3.0)/(2.0))*items[j].side + items[i].radius * 0.5));
					items[i].NFP[j].vertices.push_back(newCoord(items[j].side/(2.0) + items[i].radius * (sqrt(3.0)/(2.0)), (sqrt(3.0)/(2.0))*items[j].side + items[i].radius * 0.5));

					t = 0;
					for(int k = 3*POINTS_PER_CIRCLE/4 ; k<POINTS_PER_CIRCLE; k++){
						t = k * inc;
						items[i].NFP[j].vertices.push_back(newCoord(items[j].side/(2.0) + items[i].radius * cos(t + (POINTS_PER_CIRCLE/3 * inc)),
							sqrt(3.0)/(2.0)*items[j].side + items[i].radius * sin(t + (POINTS_PER_CIRCLE/3 * inc)) ));
					}
					t = 0;
					for(int k = 0 ; k<POINTS_PER_CIRCLE/12; k++){
						items[i].NFP[j].vertices.push_back(newCoord(items[j].side/(2.0) + items[i].radius * cos(t + (POINTS_PER_CIRCLE/3 * inc)),
							sqrt(3.0)/(2.0)*items[j].side + items[i].radius * sin(t + (POINTS_PER_CIRCLE/3 * inc)) ));
						t = (k+1) * inc;
					}

					items[i].NFP[j].vertices.push_back(newCoord(items[j].side/(2.0) - items[i].radius * (sqrt(3.0)/(2.0)), (sqrt(3.0)/(2.0))*items[j].side + items[i].radius * 0.5));
					lineToPoints(items[i].NFP[j].vertices, newCoord(items[j].side/(2.0) - items[i].radius * (sqrt(3.0)/(2.0)), (sqrt(3.0)/(2.0))*items[j].side + items[i].radius * 0.5), newCoord(-items[i].radius * (sqrt(3.0)/(2.0)), items[i].radius * 0.5));
					items[i].NFP[j].vertices.push_back(newCoord(-items[i].radius * (sqrt(3.0)/(2.0)), items[i].radius * 0.5));

					t = 0;
					for(int k = 3*POINTS_PER_CIRCLE/4 ; k<POINTS_PER_CIRCLE; k++){
						t = k * inc;
						items[i].NFP[j].vertices.push_back(newCoord(items[i].radius * cos(t + (2*POINTS_PER_CIRCLE/3 * inc)),
							items[i].radius * sin(t + (2*POINTS_PER_CIRCLE/3 * inc)) ));
					}
					t = 0;
					for(int k = 0 ; k<POINTS_PER_CIRCLE/12; k++){
						items[i].NFP[j].vertices.push_back(newCoord(items[i].radius * cos(t + (2*POINTS_PER_CIRCLE/3 * inc)),
							items[i].radius * sin(t + (2*POINTS_PER_CIRCLE/3 * inc)) ));
						t = (k+1) * inc;
					}

					//update the number of vertices of the final NFP between i and j
					items[i].NFP[j].numberOfVertices = items[i].NFP[j].vertices.size();

					double s = 0.0, c = -1.0;
					//sin 180º = 0;
					//cos 180º = -1;

					//change the reference point 
					for (int k=0; k<items[i].NFP[j].numberOfVertices; k++) {
						items[i].NFP[j].vertices[k].x = items[i].NFP[j].vertices[k].x * c - items[i].NFP[j].vertices[k].y * s;
						items[i].NFP[j].vertices[k].y = items[i].NFP[j].vertices[k].x * s + items[i].NFP[j].vertices[k].y * c;
					}

				} else if (items[j].type == CIRCLE) {

					double inc, t;
					items[i].NFP[j].numberOfVertices = POINTS_PER_CIRCLE;
					inc = 2*PI/(double)items[i].NFP[j].numberOfVertices;

					t = 0;
					for(int k = 0; k<items[i].NFP[j].numberOfVertices; k++){
						items[i].NFP[j].vertices.push_back(newCoord((items[i].radius + items[j].radius)*cos(t),
							(items[i].radius + items[j].radius)*sin(t)));
						t = (k+1) * inc;
					}

				}

			}

			//cout << "\n\nDuplicates NFP "<< i << ", " << j << ": \n";
			//remove duplicates
			for (int k=0 ; k<items[i].NFP[j].vertices.size() -1; k++) {
				//cout << "Comparing ( " << items[i].NFP[j].vertices[k].x << ", " << items[i].NFP[j].vertices[k].y << ") and ( " << items[i].NFP[j].vertices[k+1].x << ", " << items[i].NFP[j].vertices[k+1].y << "): ";
				if (fabs(items[i].NFP[j].vertices[k].x - items[i].NFP[j].vertices[k+1].x) < EPS) {
					if (fabs(items[i].NFP[j].vertices[k].y - items[i].NFP[j].vertices[k+1].y) < EPS) {
						//cout << " -----> Same \n";
						items[i].NFP[j].vertices.erase(items[i].NFP[j].vertices.begin()+k);
						k--;
					} //else {
						//cout << "Y Differente: fabs = " <<  fabs(items[i].NFP[j].vertices[k].y - items[i].NFP[j].vertices[k+1].y) << " \n";
					//}
				} //else {
					//cout << "X Differente: fabs = " <<  fabs(items[i].NFP[j].vertices[k].x - items[i].NFP[j].vertices[k+1].x) << " \n";
				//}
			}

			//cout << "Comparing ( " << items[i].NFP[j].vertices[0].x << ", " << items[i].NFP[j].vertices[0].y << ") and ( " << items[i].NFP[j].vertices[items[i].NFP[j].vertices.size()-1].x << ", " << items[i].NFP[j].vertices[items[i].NFP[j].vertices.size()-1].y << "): ";
			//if first and last points are the same, remove them
			if (fabs(items[i].NFP[j].vertices[0].x - items[i].NFP[j].vertices[items[i].NFP[j].vertices.size()-1].x) < EPS) {
				if (fabs(items[i].NFP[j].vertices[0].y - items[i].NFP[j].vertices[items[i].NFP[j].vertices.size()-1].y) < EPS) {
					//cout << " -----> Same \n";
					items[i].NFP[j].vertices.erase(items[i].NFP[j].vertices.begin() + items[i].NFP[j].vertices.size()-1);
				} //else {
						//cout << "Y Differente: fabs = " <<  fabs(items[i].NFP[j].vertices[0].y - items[i].NFP[j].vertices[items[i].NFP[j].vertices.size()-1].y) << " \n";
				//}
			} //else {
				//cout << "X Differente: fabs = " <<  fabs(items[i].NFP[j].vertices[0].x - items[i].NFP[j].vertices[items[i].NFP[j].vertices.size()-1].x) << " \n";
			//}

			items[i].NFP[j].numberOfVertices = items[i].NFP[j].vertices.size();

			//printNFP(items, i, j);
			//drawNFP(items, i, j);
			
		}

	}

}

void drawNFP(item *items, int i, int j) {

	if (!items) {
		cout << "Error: Cannot draw the NFP of pair "<< i << ", " << j << ". Invalid pointer. \n";
		exit(EXIT_FAILURE);
	}

	char *fileName = new char[20];
	sprintf (fileName,"NFP_%d_%d.tex",i,j);
	ofstream file (fileName, ofstream::out);
	
	file << "\\documentclass{article}\n\\usepackage{tikz}\n\\begin{document}\n";
	file << "\\begin{tikzpicture}[scale=0.3]\n\\begin{scope}[shift={(0.000, 0)}]\n";

	if (items[i].type == RECTANGLE)
		file << "\n\\draw[black, fill=lightgray] (0,0) rectangle (" << items[i].length << ", " << items[i].width << ");";
	
	if (items[i].type == TRIANGLE)
		file << "\n\\draw[black, fill=lightgray] (0,0) -- (" << items[i].side << ", 0) -- (" << items[i].side / (2.0) << ", "<< sqrt(3.0)/(2.0) * items[i].side << ") -- cycle;";
	
	if (items[i].type == CIRCLE)
		file << "\n\\draw[black, fill=lightgray] (0,0) circle (" << items[i].radius << ");";

	for (int k=0; k<items[i].NFP[j].numberOfVertices ; k++) {
		file << "\n\\draw[fill=darkgray] (" << items[i].NFP[j].vertices[k].x << ", " << items[i].NFP[j].vertices[k].y << ") circle (10.000pt);";
	}

	


	file << "\\end{scope}\n\\end{tikzpicture}\n\\end{document}";

	file.close();

}



