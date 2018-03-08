/*
 * main.cpp
 *
 *  Created on: Mar 6, 2018
 *      Author: Jacob & Vanessa
 */

#include "DecisionTree.h"

int main(int argc, char** argv) {

	ifstream fin;
	fin.open(argv[1]);

	if(!fin) {
		cerr << "File not found";
		return 1;
	}

	vector<int> attributes;

	for(int i=2; i < argc; i++)
		attributes.push_back(atoi(argv[i]));

	sort(attributes.begin(), attributes.end());

	vector<vector<string> > data;

	parseData(fin, data, attributes);

	pair<int, int> x;

	//x = findSplitAttribute(data); cout << x.first;

	node* root = buildTree(data, attributes);

	//printTree(root, 0);


	return 0;
}



