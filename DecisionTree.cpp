/*
 * DecisionTree.cpp
 *
 *  Created on: Mar 6, 2018
 *      Author: Jacob & Vanessa
 */

#include "DecisionTree.h"

void parseData(ifstream &fin, vector<vector<string> > &data, vector<int> attributes) {
	string tuple;
	string delimiter = ",";
	string token;

	while(getline(fin, tuple)) {
		size_t position = 0;
		vector<string> tempVector;

		while((position = tuple.find(delimiter)) != string::npos) {
			token = tuple.substr(0, position);
			tempVector.push_back(token);
			tuple.erase(0, position + delimiter.length());
		}
		tempVector.push_back(tuple);
		data.push_back(tempVector);
	}
	clearColumn(data, attributes);
}

void clearColumn(vector<vector<string> > &data, vector<int> attributes) {
	vector<string> tempVector;

	for(unsigned int i=0; i < data.size(); i++) {
		for(int j=0; j < attributes.size(); j++) {
			tempVector.push_back(data[i][attributes[j]]);
		}
		tempVector.push_back(data[i].back());

		data[i].clear();
		data[i].resize(tempVector.size());
		data[i] = tempVector;

		tempVector.clear();
	}
}

void printTable(vector< vector<string> >data) {
	for(unsigned int i=0; i < data.size(); i++) {
		for(unsigned int j = 0; j < data[i].size(); j++) {
			cout << data[i][j];

			if(j != data[i].size()-1)
				cout << " ";
		}
	}
	cout << endl << "row count: " << data.size() << endl;
}

void buildClassList(vector<vector<string> >data, vector<string> &classList) {
	vector<string> columnData;
	int lastIndex = data[0].size()-1;

	for(unsigned int i=0; i < data.size(); i++)
		columnData.push_back(data[i][lastIndex]);

	sort(columnData.begin(), columnData.end());
	columnData.erase(unique(columnData.begin(), columnData.end()), columnData.end());

	for(unsigned int i=0; i < columnData.size(); i++)
		classList.push_back(columnData[i]);
}

void classCount(vector<vector<string> > data, vector<string> classList, vector<int> &classCount) {
	int lastIndex = data[0].size()-1;

	classCount.resize(classList.size());

		for(int i=0; i < classList.size(); i++) {
			for(unsigned int j=0; j < data.size(); j++) {
				if(classList[i] == data[j][lastIndex])
					classCount[i]++;
			}
		}
}

pair<int, double> calculateInfoGain(vector<vector<string> > &data, int column) {
	vector<string> classNames;
	vector<int> numOfEach;
	pair<int, double> infoSplit (0, 0);

	buildClassList(data, classNames);
	classCount(data, classNames, numOfEach);

	quickSort(data, 0, data.size()-1, data.size(), column);
	double dataEntropy = 0;

	for(int i=0; i < classNames.size(); i++)
		dataEntropy -= (((double)numOfEach[i] / (double)data.size()) * log2((double)numOfEach[i] / (double)data.size()));

	classNames.clear();
	numOfEach.clear();

	for(unsigned int i=1; i < data.size(); i++) {
		double leftEntropy = 0;
		double rightEntropy = 0;

		size_t split = i;
		vector<vector<string> > left(data.begin(), data.begin() + split);
		buildClassList(left, classNames);
		classCount(left, classNames, numOfEach);

		for(int i=0; i < classNames.size(); i++) {
				leftEntropy -= (((double)numOfEach[i] / (double)left.size()) * log2((double)numOfEach[i] / (double)left.size()));
				leftEntropy = ((double)left.size() / (double)data.size()) * leftEntropy;
		}

		classNames.clear();
		numOfEach.clear();

		vector<vector<string> > right(data.begin() + split, data.end());
		buildClassList(right, classNames);
		classCount(right, classNames, numOfEach);

		for(int i=0; i < classNames.size(); i++) {
				rightEntropy -= (((double)numOfEach[i] / (double)right.size()) * log2((double)numOfEach[i] / (double)right.size()));
				rightEntropy = ((double)right.size() / (double)data.size()) * rightEntropy;
		}

		double currentGain = dataEntropy - (leftEntropy + rightEntropy);

		if(currentGain > infoSplit.second) {
			infoSplit.second = currentGain;
			infoSplit.first = static_cast<int>(split);
		}
	}
	return infoSplit;
}

pair<int, int> findSplitAttribute(vector<vector<string> > data) {
	vector<pair<int, double> > attributeInfo;			//stores optimal split point and info gain for each attribute
	double highestGain = 0;

	for(int i=0; i < data[0].size()-1; i++)
		attributeInfo.push_back(calculateInfoGain(data, i));

	pair<int, int> splitAttribute (0, 0);				//first = attribute index, second = split point

	for(int i=0; i < attributeInfo.size(); i++) {
		if(attributeInfo[i].second > highestGain) {
			highestGain = attributeInfo[i].second;
			splitAttribute.first = i;
			splitAttribute.second = attributeInfo[i].first;
		}
	}
	return splitAttribute;
}

node* buildTree(vector<vector<string> > data, vector<int> attributes) {
	vector<string> classList;
	vector<int> numOfEach;
	node* newNode = new node;

	buildClassList(data, classList);
	classCount(data, classList, numOfEach);

	string largestClass;
	int highestCount = 0;

	for(int i=0; i < classList.size(); i++) {
		if(numOfEach[i] > highestCount) {
			highestCount = numOfEach[i];
			largestClass = classList[i];
		}
	}

	if(classList.size() == 1) {
		newNode->data = data;
		newNode->label = classList[0];
		newNode->isLeaf = true;
		newNode->left = NULL;
		newNode->right = NULL;
		return newNode;
	}

	if(attributes.size() == 0) {

		newNode->data = data;
		newNode->label = largestClass;
		newNode->isLeaf = true;
		newNode->left = NULL;
		newNode->right = NULL;
		return newNode;
	}

	pair<int, int> splitAttribute = findSplitAttribute(data);
	int attribute = splitAttribute.first;
	int splitIndex = splitAttribute.second;

	cout << attribute << endl;

	newNode->data = data;
	newNode->label = attribute;
	newNode->isLeaf = false;
	newNode->left = NULL;
	newNode->right = NULL;

	vector<vector<string> > left(data.begin(), data.begin() + splitIndex);
	vector<vector<string> > right(data.begin() + splitIndex, data.end());

	if(left.size() == 0) {
		node* leftNode = new node;
		leftNode->label = largestClass;
		leftNode->isLeaf = true;
		leftNode->left = NULL;
		leftNode->right = NULL;
	}
	else
		newNode->left = buildTree(left, attributes);

	if(right.size() == 0) {
			node* rightNode = new node;
			rightNode->label = largestClass;
			rightNode->isLeaf = true;
			rightNode->left = NULL;
			rightNode->right = NULL;
		}
		else
			newNode->right = buildTree(right, attributes);

	return newNode;
}

void printTree(node* currentNode, int depth) {
	if(currentNode == NULL)
		return;

	if(depth == 0) {
		cout << "root: " << currentNode->splitAttribute;
	}


}

//*****************************************//
//***quickSort algorithm for 2-d vectors***//
//*****************************************//

int partition(vector<vector<string> > &data, int left, int right, int column) {
	const int mid = left + (right - left) / 2;
	const double pivot = toDouble(data[mid][column]);

	swap(data[mid], data[left]);

	int i = left + 1;
	int j = right;

	while(i <= j) {
		while(i <= j && toDouble(data[i][column]) <= pivot)
			i++;

		while(i <= j && toDouble(data[j][column]) > pivot)
			j--;

		if(i < j)
			swap(data[i], data[j]);
	}
	swap(data[i-1], data[left]);
	return i-1;
}

void quickSort(vector<vector<string> > &data, int left, int right, int size, int column) {
	if(left >= right)
		return;

	int partitionIndex = partition(data, left, right, column);

	quickSort(data, left, partitionIndex-1, size, column);
	quickSort(data, partitionIndex+1, right, size, column);
}

//*******************************//
//***converts string to double***//
//*******************************//

double toDouble(string s) {
	double d;
	stringstream ss(s);
	ss >> d;
	return d;
}


