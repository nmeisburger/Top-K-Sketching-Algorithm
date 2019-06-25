#include <iostream>

using namespace std;

#include "CMS.h"
#define TABLENULL -1

int main() {

	int data[] = {0,1,2,3,4,3,3,2,1,4,5,6,TABLENULL,TABLENULL,TABLENULL,TABLENULL};

	CMS* cms = new CMS(2, 10, 1);

	cms->add(0, data, 16);

	cms->showCMS(0);

	int* topK = new int[3];

	cms->topK(3, 0, topK, 0);

	printf("TOP K:\n");
	for (int i = 0; i < 3; i++) {
		printf("\t%d", topK[i]);
	}
	printf("\n");

	cms->showCMS(0);

	int newLHH[40] = {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6};

	cms->combineSketches(newLHH);

	cms->showCMS(0);

	delete cms;

	return 0;
}
