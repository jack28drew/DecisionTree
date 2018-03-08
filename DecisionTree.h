/*
 * DecisionTree.h
 *
 *  Created on: Mar 6, 2018
 *      Author: Jacob & Vanessa
 */

#ifndef DECISIONTREE_H_
#define DECISIONTREE_H_

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <cmath>
#include <sstream>
#include <algorithm>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

struct node {
	vector<vector<string> > data;
	int splitAttribute;
	string label;
	bool isLeaf;
	node* left;
	node* right;
};

void parseData(ifstream&, vector<vector<string> >&, vector<int>);			//read file into 2d vector
void clearColumn(vector<vector<string> >&, vector<int>);
void printTable(vector<vector<string> >);
void buildClassList(vector<vector<string> >&, vector<string>&, vector<int>&);
void classCount(vector<vector<string> >, vector<string>, vector<int>&);
pair<int,double> calculateInfoGain(vector<vector<string> >&, int column);	//calculates and returns optimal info gain and split point
pair<int, int> findSplitAttribute(vector<vector<string> >);			//finds the split attribute index first and returns the split point along with
node* buildTree(vector<vector<string> >, vector<int>);
void printTree(node*, int depth);

int partition(vector<vector<string> >&, int, int, int);
void quickSort(vector<vector<string> >&, int, int, int, int);
double toDouble(string);


#endif /* DECISIONTREE_H_ */
