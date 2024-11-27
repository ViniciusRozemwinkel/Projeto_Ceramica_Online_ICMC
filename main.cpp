#include "main.h"

using namespace std;

int main(int argc, char *argv[]) {

    char *kilnFile = (char *)malloc(1024 * sizeof(char));
    char *itemsFile = (char *)malloc(1024 * sizeof(char));
    char *solutionFile = (char *)malloc(1024 * sizeof(char));
    
    if (!kilnFile || !itemsFile || !solutionFile) {
        cout << "Error: Internal error while allocating memory for the configuration files. \n";
		exit(EXIT_FAILURE);
    }
    
    if (argc >= 4) {
        strcpy(kilnFile, argv[1]);
        strcpy(itemsFile, argv[2]);
        strcpy(solutionFile, argv[3]);
    } else {
        strcpy(kilnFile, "kiln.txt");
        strcpy(itemsFile, "items.txt");
        strcpy(solutionFile, "solution.tex");
    }

    cout << "\n";
	kiln *mainKiln = initKiln(kilnFile);	
	//printKilnInformation(mainKiln);
    
    calculateAllPossibleHeights(mainKiln);
    //printAllPossibleHeights(mainKiln);

	//numberOfItems: number of different types of items
	int numberOfItems = -1;
	item *items = initItems(itemsFile,&numberOfItems);
	//printItemsInformation(items, numberOfItems);

	boundingBox(items, numberOfItems);
    //printBoundingBox(items, numberOfItems);

    calculateArea(items, numberOfItems);
    //printArea(items, numberOfItems);

	calculateNFP(items, numberOfItems);

	vector<int> order = generateOrder(items, numberOfItems);
	//printOrder(items, order);

	solutionPool result = solve(items, numberOfItems, order, mainKiln);

	cout << "Solution: ";
	for (int l=0 ; l < mainKiln->layers.size() ; l++) {
		cout << " - Layer #" << l+1 << " - Length: " << result.layerLengths[l] << "\n";
		for (int i=0; i<result.solutions.size() ; i++) {
			if (result.solutions[i].layer == l && result.solutions[i].id != 0)
				cout << "   - Item #" << result.solutions[i].id << ": (" << result.solutions[i].p.x << ", " << result.solutions[i].p.y << ") \n";
		}	
	}
	
	cout << "\n";
	drawSolution(items, result, mainKiln, solutionFile, numberOfItems);

	free(kilnFile);
	free(itemsFile);
	free(solutionFile);
    free(mainKiln);
	free(items);

	return 0;
}