#include "bottom-left.h"

using namespace std;

struct orderClass{
	bool operator()( const coord& a, const coord& b ) const {
		return ((a.x < b.x) || (fabs(a.x-b.x)<EPS && a.y < b.y) );
	}
} sortCoord;

struct orderHeightClass{
	item *items;
	orderHeightClass(item *items) {
		this->items = items;
	}
	bool operator()( const int& a, const int& b ) const {
		return ((items[a].height > items[b].height) || (fabs(items[a].height - items[b].height)<EPS && items[a].area > items[b].area));
	}
};

int d_function(coord a, coord b, coord p){
	
	double d = (a.x - b.x) * (a.y - p.y) - (a.y - b.y) * (a.x - p.x);
	if (fabs(d) < EPS)
		return (0);
	else if (d < 0)
		return (-1);
	else
		return (1);

}

vector<int> generateOrder(item *items, int numberOfItems) {

	if (!items) {
		cout << "Error: Cannot generate the order of the items. Invalid pointer. \n";
		exit(EXIT_FAILURE);
	}
	
	vector<int> order;

	for (int i=1 ; i<=numberOfItems ; i++) 
		for (int j=0 ; j<items[i].quantity ; j++)
			order.push_back(i);

	sort(order.begin(), order.end(), orderHeightClass(items));
	
	//srand(time(NULL));
	//random_shuffle(order.begin(), order.end());
	
	return order;
	
}

void printOrder(item *items, vector<int> order) {

	if (!items) {
		cout << "Error: Cannot print the order of the items. Invalid pointer. \n";
		exit(EXIT_FAILURE);
	}

	cout << "Order: \n - Number of pieces: " << order.size() << "\n";
	for (int i=0 ; i< order.size() ; i++)
		cout << "   - Item #" << order[i] << " - Height: " << items[order[i]].height << " - Area: " << items[order[i]].area << "\n";
	

}

int minimumFittingHeight(double itemHeight, kiln *k) {

	if (!k) {
		cout << "Error: Cannot calculate minimum fitting height. Invalid pointer. \n";
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i< k->possibleHeights.size(); i++) {
        if (k->possibleHeights[i] >= itemHeight)
            return k->possibleHeights[i];
    }
    return -1;

}

void addSupports(solutionPool *sp, int layer, kiln *k) {

	solution sol;

	sol.id = 0;
	sol.layer = layer;
	
	sol.p.x = 0;
	sol.p.y = 0;
	sp->solutions.push_back(sol);
			
	sol.p.x = 0;
	sol.p.y = k->width - (SUPPORT_SIZE + DISTANCE);
	sp->solutions.push_back(sol);

	sol.p.x = k->length - (SUPPORT_SIZE + DISTANCE);
	sol.p.y = 0;
	sp->solutions.push_back(sol);

	sol.p.x = k->length - (SUPPORT_SIZE + DISTANCE);
	sol.p.y = k->width - (SUPPORT_SIZE + DISTANCE);
	sp->solutions.push_back(sol);

}

solutionPool solve(item *items, int numberOfItems, vector<int> order, kiln *k) {

	if (!items) {
		cout << "Error: Cannot solve the problem. Invalid pointer. \n";
		exit(EXIT_FAILURE);
	}

	cout << "Solving BL: \n";

	//solution pool to be returned at the end of the function
	solutionPool sp;

	int numberOfPieces = order.size();

	vector<int> allocatedPieces;
	allocatedPieces.push_back(0);
	
	sp.layerLengths.push_back(0.0);

	double cumulativeHeight = 0;

	//create first layer
	if (numberOfPieces > 0) {
		for (int i=0; i<=numberOfPieces ; i++) {
			int mfh = minimumFittingHeight(items[order[i]].height, k);
			if (mfh != -1) {
				layer l;
				l.length = k->length;
				l.width = k->width;
				l.height = mfh;
				k->layers.push_back(l);

				cumulativeHeight += l.height + BOARD_HEIGHT;

				addSupports(&sp, 0, k);

				break;
			}
		}
	}

	if (k->layers.empty()) {
		//none of the pieces fit the kiln (height)
			cout << "\n --> WARNING: none of the pieces could be allocated. <--\n\n";
			return sp;
	}

	int allocated = 0;
	int expanded = 0;

	for (int i=0 ; i<numberOfPieces ; i++) {

		allocated = 0;
		expanded = 0;

		for (int current_layer=0 ; (current_layer < k->layers.size() && !allocated) ; current_layer++) {

			if (items[order[i]].height > k->layers[current_layer].height) {
				cout << " - Item #" << order[i] << " doesn't fit on layer #" << current_layer+1 << "\n";
				continue;
			}

			if (allocatedPieces[current_layer] == 0) {
				//first piece, don't need to check NFP
				//if rectangle or triangle, p=(0,0). if circle, p=(radius, radius)

				solution sol;
				sol.id = order[i];
				sol.layer = current_layer;

				if (items[order[i]].type == RECTANGLE || items[order[i]].type == TRIANGLE) {
					sol.p.x = 0;
					sol.p.y = SUPPORT_SIZE + DISTANCE;
				} else if (items[order[i]].type == CIRCLE) {
					if (items[order[i]].radius >= 20)
						sol.p.y = items[order[i]].radius;
					else
						sol.p.y = items[order[i]].radius + SUPPORT_SIZE + DISTANCE;
					sol.p.x = items[order[i]].radius;
				}

				if (checkBoundingBox(items, sol, k) && (checkSolution(items,sol.id,sol.p.x,sol.p.y,sol.layer,sp))) {

					sp.solutions.push_back(sol);
					allocatedPieces[current_layer] += 1;
					allocated = 1;

					cout << " - Allocated piece #" << sol.id+1 << " at (" << sol.p.x << ", " << sol.p.y << ") - layer #" << current_layer + 1 << " \n";

					//update solution value
					if ((sol.p.x + items[order[i]].boundingBox.rb.x) > sp.layerLengths[current_layer])
						sp.layerLengths[current_layer] = sol.p.x + items[order[i]].boundingBox.rb.x;

				} else
					cout << " - Couldn't allocate first item \n";
	 
			} else {


				vector<coord> pointsNFP;
				pointsNFP.clear();
				
				for (int j=0; j<sp.solutions.size() ; j++) {

					if (sp.solutions[j].id == 0)
						continue;

					// order[i] is the piece to be inserted
					// id = sp.solutions[j].id is the piece that has alreade been inserted

					if (sp.solutions[j].layer == current_layer) {

						int id = sp.solutions[j].id;
						double px = sp.solutions[j].p.x;
						double py = sp.solutions[j].p.y;

						pointsNFP.reserve(pointsNFP.size() + items[id].NFP[order[i]].vertices.size());

						//insert the NFP of id and i, considering the place where id has been positioned
						for (int l=0 ; l<items[id].NFP[order[i]].vertices.size() ; l++) {
							pointsNFP.push_back(newCoord(items[id].NFP[order[i]].vertices[l].x + px,  items[id].NFP[order[i]].vertices[l].y + py));
						}

					}

				}

				//sort all possible points (from all NFPs) according to the BL heuristic
				sort(pointsNFP.begin(), pointsNFP.end(), sortCoord);

				//drawStep(items, sp, pointsNFP, i);

				//cout << "NFP dots for #" << order[i]+1 << ":\n";

				//test all possible points until it find a valid one
				for (int j=0 ; j<pointsNFP.size(); j++) {

					//cout << "\n - (" << pointsNFP[j].x << ", " << pointsNFP[j].y << "): ";

					// check points out of layer
					if ((pointsNFP[j].x < 0) || (pointsNFP[j].x > k->layers[current_layer].length) || (pointsNFP[j].y < 0) || (pointsNFP[j].y > k->layers[current_layer].width)) {
						//cout << "Out of board";
						continue;
					}

					// check bounding box to ensure piece is inside the layer
					if (!checkBoundingBox(items, order[i], pointsNFP[j].x, pointsNFP[j].y, current_layer, k)) {
						//cout << "Bounding Box";
						continue;	
					}

					// check if solution isn't exactly the same as another one
					if (!checkPastSolutions(pointsNFP[j].x, pointsNFP[j].y, current_layer, sp)) {
						//cout << "Past Solutions";
						continue;	
					}

					// check if point is inside any NFP
					if (!checkSolution(items, order[i], pointsNFP[j].x, pointsNFP[j].y, current_layer, sp)) {
						//cout << "Inside NFP";
						continue;
					} else {

						// if we got to this point, pointsNFP[j] is a valid coordinate
						solution sol;
						
						sol.id = order[i];
						sol.layer = current_layer;
						sol.p.x = pointsNFP[j].x;
						sol.p.y = pointsNFP[j].y;

						sp.solutions.push_back(sol);
						allocatedPieces[current_layer] += 1;
						allocated = 1;

						cout << " - Allocated piece #" << sol.id << " at (" << sol.p.x << ", " << sol.p.y << ") - layer #" << current_layer + 1 << " \n";

						//update solution value
						if ((sol.p.x + items[order[i]].boundingBox.rb.x) > sp.layerLengths[current_layer])
							sp.layerLengths[current_layer] = sol.p.x + items[order[i]].boundingBox.rb.x;

						break;	

					}

				}

				if ((current_layer == (k->layers.size() - 1)) && !allocated && !expanded && allocatedPieces[current_layer] != 0) {
						
					int mfh = minimumFittingHeight(items[order[i]].height, k);
					//try to create another layer that fits this item
					if (cumulativeHeight + mfh + BOARD_HEIGHT <= k->height) {
						layer l;
						l.length = k->length;
						l.width = k->width;
						l.height = mfh;
						k->layers.push_back(l);

						cumulativeHeight += l.height + BOARD_HEIGHT;
						allocatedPieces.push_back(0);
						sp.layerLengths.push_back(0.0);

						addSupports(&sp, (k->layers.size() -1), k);

						//each item can expand only once
						expanded = 1;
						//"\n\n Expanded one layer \n\n";
					} else {
						//"\n\n tried to expand but the kiln couldn't fit another layer\n\n";
					}	
				}

			}

		}

		if (!allocated) {
			cout << " - Couldn't allocate piece #" << order[i]<< " ";
			if (expanded)
				cout << "but tried to expand. \n";
			else
				cout << "and didn't try to expand. \n";	
		}

	}

	int total = 0;
	for (int i=0 ; i<k->layers.size(); i++)
		total += allocatedPieces[i];

	if (total < numberOfPieces) {
		cout << "\n --> WARNING: some pieces could not be allocated. <--\n\n";
	}

	return sp;

}

int checkBoundingBox(item *items, solution sol, kiln *k) {

	if ((items[sol.id].boundingBox.lu.x + sol.p.x < 0) || (items[sol.id].boundingBox.lu.y + sol.p.y > k->layers[sol.layer].width))
		return 0;
	if ((items[sol.id].boundingBox.rb.x + sol.p.x > k->layers[sol.layer].length) || (items[sol.id].boundingBox.rb.y + sol.p.y < 0))
		return 0;

	return 1;

}

int checkBoundingBox(item *items, int id, double x, double y, int layer, kiln *k) {

	if ((items[id].boundingBox.lu.x + x < 0) || (items[id].boundingBox.lu.y + y > k->layers[layer].width))
		return 0;
	if ((items[id].boundingBox.rb.x + x > k->layers[layer].length) || (items[id].boundingBox.rb.y + y < 0))
		return 0;

	return 1;

}

int checkPastSolutions(double x, double y, int layer, solutionPool sp) {

	for (int i=0 ; i<sp.solutions.size() ; i++) {
		if (sp.solutions[i].p.x == x && sp.solutions[i].p.y == y && sp.solutions[i].layer == layer)
			return 0;
	}

	return 1;
}

int checkSolution(item *items, int id, double x, double y, int layer, solutionPool sp) {

	//cout << "\nTrying to allocate #" << id+1 << " at (" << x << ", " << y << ") \n"; 
	coord a, b, p;
	p.x = x;
	p.y = y;

	int d_func;
	int violatedAtLeastOnce;

	for (int i=0 ; i<sp.solutions.size() ; i++) {

		if (sp.solutions[i].layer != layer)
			continue;

		violatedAtLeastOnce = 0;
		
		//cout << "\nNFP #" << sp.solutions[i].id + 1 << "\n";

		for (int j=0 ; j<items[sp.solutions[i].id].NFP[id].vertices.size() ; j++) {

			a.x = items[sp.solutions[i].id].NFP[id].vertices[j].x + sp.solutions[i].p.x;
			a.y = items[sp.solutions[i].id].NFP[id].vertices[j].y + sp.solutions[i].p.y;
				
			if (j == items[sp.solutions[i].id].NFP[id].vertices.size() -1) {
				//last vertex, so we have a: j, b: first
				b.x = items[sp.solutions[i].id].NFP[id].vertices[0].x + sp.solutions[i].p.x;
				b.y = items[sp.solutions[i].id].NFP[id].vertices[0].y + sp.solutions[i].p.y;
			} else {
				//not the last one, so we have a: j, b: j+1
				b.x = items[sp.solutions[i].id].NFP[id].vertices[j+1].x + sp.solutions[i].p.x;
				b.y = items[sp.solutions[i].id].NFP[id].vertices[j+1].y + sp.solutions[i].p.y;				
			}

			//cout << "d_func( (" << a.x << ", " << a.y << "), (" << b.x << ", " << b.y << ") , (" << p.x << ", " << p.y << ") ) =  " << d_function(a,b,p) << "\n";

			if (d_func = d_function(a,b,p) <= 0) {
				violatedAtLeastOnce = 1;
				break;
			}

		}

		if (!violatedAtLeastOnce) {
			//cout << "\nFailed!\n";
			return 0;
		}

	}

	return 1;

}

void drawSolution(item *items, solutionPool sp, kiln *k, char *fileName, int numberOfItems) {

	if (!items) {
		cout << "Error: Cannot draw solutions. Invalid pointer. \n";
		exit(EXIT_FAILURE);
	}

	ofstream file (fileName, ofstream::out);
	
	file << "\\documentclass[a4paper,landscape]{article}\n\\usepackage{tikz}\n\\usepackage{gensymb}\n\\usepackage[utf8]{inputenc}";
	file << "\n\\usepackage[left=1cm,top=1cm,right=1cm,bottom=1cm,verbose,nohead,nofoot]{geometry}\n\\begin{document}\n\\pagenumbering{gobble}";

	int layer = 0;
	double usedArea = 0, totalArea = 0, usedVolume = 0, totalVolume = 0;
	do {

		if (layer == k->layers.size() - 1) {
			totalArea += sp.layerLengths[layer] * k->width;
			totalVolume += sp.layerLengths[layer] * k->width * k->layers[layer].height;			
		} else {
			totalArea += k->length * k->width;
			totalVolume += k->length * k->width * k->layers[layer].height;
		}

		file << "\n\\thispagestyle{empty}";

		file << "\n\\begin{center} \n\\large\\textbf{"<< layer + 1 << "\\degree Andar}\\\\\n\\vspace*{5px} \n\\large\n\\textmd{Comprimento: " << k->layers[layer].length  << "  -  Profundidade: " <<  k->layers[layer].width  << "  -  Altura: " << k->layers[layer].height << " (cm)}\\\\ \\end{center}";

		file << "\n\\centering\n\\resizebox";

		if (k->layers[layer].width/ k->layers[layer].length < 0.7)
			file << "{0.9\\textwidth}{!}{%";
		else
			file << "{!}{0.9\\textheight}{%";
		
		file << "\n\\begin{tikzpicture}";
		file << "\\draw[black] (0,0) rectangle (" << k->layers[layer].length << ", "<< k->layers[layer].width << ");";


		for (int i=0 ; i<sp.solutions.size() ; i++) {

			if (sp.solutions[i].layer != layer)
				continue;

			int id = sp.solutions[i].id;
			double px = sp.solutions[i].p.x;
			double py = sp.solutions[i].p.y;

			usedArea += items[id].area;
			if (id == 0)
				usedVolume += items[id].area * k->layers[layer].height;
			else
				usedVolume += items[id].area * items[id].height;

			string color = "lightgray";
			if (id == 0)
				color = "darkgray";

			if (items[id].type == RECTANGLE) {
				px = px + DISTANCE/2.0;
				py = py + DISTANCE/2.0;
				file << "\n\\draw[black, fill=" << color << "] (" << px << "," << py << ") rectangle (" << px + items[id].length - DISTANCE << ", " << py + items[id].width - DISTANCE << ");";
				if (id != 0)
					file << "\n\\draw (" << px + (items[id].length - DISTANCE)/2.0  << ", " << py + (items[id].width - DISTANCE)/2.0 << ") node {\\huge " <<  id << "};";
			}
		
			else if (items[sp.solutions[i].id].type == TRIANGLE) {
				px = px + DISTANCE/2.0;
				py = py + DISTANCE/2.0;
				file << "\n\\draw[black, fill=lightgray] (" << px << "," << py << ") -- (" << (items[id].side - DISTANCE) + px << ", " << py << ") -- (" << (items[id].side - DISTANCE) / (2.0) + px << ", "<< sqrt(3.0)/(2.0) * (items[id].side - DISTANCE) + py << ") -- cycle;";
				file << "\n\\draw (" << px + (items[id].side - DISTANCE)/2.0  << ", " << py + ((sqrt(3.0)/(2.0) * (items[id].side - DISTANCE))/3.0) << ") node {\\huge " <<  id << "};";
			}
		
			else if (items[sp.solutions[i].id].type == CIRCLE) {
				file << "\n\\draw[black, fill=lightgray] (" << px << "," << py << ") circle (" << items[id].radius - DISTANCE/2.0 << ");";
				file << "\n\\draw (" << px  << ", " << py << ") node {\\huge " <<  id << "};";
			}

		}

		file << "\\end{tikzpicture}\n}%\n";


		if (layer < k->layers.size()) {
			file << "\\newpage\\clearpage";
		}

		layer++;

	} while (layer < k->layers.size());

	file << "\\thispagestyle{empty}\n\\begin{center} \n\\large\n\\textbf{Descrição das Peças}\\\\";
	file << "\\vspace*{10px} \n \\end{center}";

	file << "\\begin{enumerate}";

	for (int id = 1; id <= numberOfItems ; id++) {
		file << "\\item " << items[id].description << "\n";
	}

	file << "\\end{enumerate}";

	file << "\n\\end{document}";

	file.close();

	cout << "\n- Used Area: " << usedArea ;
	cout << "\n- Total Area: " << totalArea ;
	cout << "\n- Usage: " << usedArea/totalArea*100 << "%\n\n" ;

	cout << "\n- Used Volume: " << usedVolume ;
	cout << "\n- Total Volume: " << totalVolume ;
	cout << "\n- Usage: " << usedVolume/totalVolume*100 << "%\n\n" ;


}

// void drawStep(item *items, solutionPool sp, vector<coord> pointsNFP, int newItem) {

// 	if (!items) {
// 		cout << "Error: Cannot draw intermediate step. Invalid pointer. \n";
// 		exit(EXIT_FAILURE);
// 	}

// 	char *fileName = new char[20];
// 	sprintf (fileName,"adding_%d.tex", newItem);
// 	ofstream file (fileName, ofstream::out);
	
// 	file << "\\documentclass{article}\n\\usepackage{tikz}\n\\begin{document}\n";
// 	file << "\\begin{tikzpicture}[scale=0.5]\n\\begin{scope}[shift={(0.000, 0)}]\n";

// 	for (int i=0 ; i<sp.solutions.size() ; i++) {

// 		int id = sp.solutions[i].id;
// 		double px = sp.solutions[i].p.x;
// 		double py = sp.solutions[i].p.y;

// 		if (items[id].type == RECTANGLE)
// 			file << "\n\\draw[black, fill=blue] (" << px << "," << py << ") rectangle (" << px + items[id].length << ", " << py + items[id].width << ");";
	
// 		else if (items[sp.solutions[i].id].type == TRIANGLE)
// 			file << "\n\\draw[black, fill=blue] (" << px << "," << py << ") -- (" << items[id].side + px << ", " << py << ") -- (" << items[id].side / (2.0) + px << ", "<< sqrt(3.0)/(2.0) * items[id].side + py << ") -- cycle;";
	
// 		else if (items[sp.solutions[i].id].type == CIRCLE)
// 			file << "\n\\draw[black, fill=blue] (" << px << "," << py << ") circle (" << items[id].radius << ");";

// 	}

// 	for (int i=0 ; i<pointsNFP.size() ; i++) {
// 		file << "\n\\draw[fill=red] (" << pointsNFP[i].x << ", " << pointsNFP[i].y << ") circle (10.000pt);";
// 	}

// 	file << "\\end{scope}\n\\end{tikzpicture}\n\\end{document}";

// 	file.close();

// }

