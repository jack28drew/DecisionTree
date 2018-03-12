/*
 * DecisionTree.cpp
 *
 *  Created on: Mar 6, 2018
 *      Author: Jacob & Vanessa
 */

#include "DecisionTree.h"

void parseData(ifstream &fin, vector<vector<string> > &data) {
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

		if (!tempVector.back().empty() && tempVector.back()[tempVector.size() - 1] == '\r')
			tempVector.back().erase(remove(tempVector.back().begin(), tempVector.back().end(), '\r'));

		data.push_back(tempVector);
	}
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
	for(unsigned int i=0; i < /*data.size()*/500; i++) {

		cout << i << " - ";

		for(unsigned int j = 0; j < data[i].size(); j++)
			cout << setw(8) << left << data[i][j];
		cout << endl;
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

pair<double, double> calculateInfoGain(vector<vector<string> > data, int column) {
	vector<string> classNames;
	vector<int> numOfEach;
	pair<double, double> infoSplit (0, 0);		// stores first the split point threshold and then the info gain of that attribute

	buildClassList(data, classNames);
	classCount(data, classNames, numOfEach);

	double dataEntropy = 0;

	for(int i=0; i < classNames.size(); i++)
		dataEntropy -= (((double)numOfEach[i] / (double)data.size()) * log2((double)numOfEach[i] / (double)data.size()));

	classNames.clear();
	numOfEach.clear();

	double threshold = (toDouble(data[0][column]) + toDouble(data[data.size()-1][column])) / 2;

	vector<vector<string> > left;
	vector<vector<string> > right;

	splitData(data, left, right, threshold, column);

	double leftEntropy = 0;
	double rightEntropy = 0;

	buildClassList(left, classNames);
	classCount(left, classNames, numOfEach);

	for(int i=0; i < classNames.size(); i++) {
		leftEntropy -= (((double)numOfEach[i] / (double)left.size()) * log2((double)numOfEach[i] / (double)left.size()));
		leftEntropy = ((double)left.size() / (double)data.size()) * leftEntropy;
	}

	classNames.clear();
	numOfEach.clear();

	buildClassList(right, classNames);
	classCount(right, classNames, numOfEach);

	for(int i=0; i < classNames.size(); i++) {
		rightEntropy -= (((double)numOfEach[i] / (double)right.size()) * log2((double)numOfEach[i] / (double)right.size()));
		rightEntropy = ((double)right.size() / (double)data.size()) * rightEntropy;
	}

	infoSplit.first = threshold;		//threshold used to find split point
	infoSplit.second = dataEntropy - (leftEntropy + rightEntropy);	//entropy of attribute at given threshold

	return infoSplit;
}

pair<int, int> findSplitAttribute(vector<vector<string> > data, vector<int>attributeList) {
	vector<pair<double, double> > attributeInfo(data[0].size()-1);			//stores threshold and info gain for each attribute
	double highestGain = 0;
	int splitAttribute;
	int splitIndex = 0;

	for(int i=0; i < attributeList.size(); i++) {
		quickSort(data, 0, data.size()-1, data.size(), attributeList[i]);
		attributeInfo[attributeList[i]] = (calculateInfoGain(data, attributeList[i]));
	}

	for(int i=0; i < attributeInfo.size(); i++) {
		if(attributeInfo[i].second > highestGain) {
			highestGain = attributeInfo[i].second;
			splitAttribute = i;
		}
	}

	quickSort(data, 0, data.size()-1, data.size(), splitAttribute);

	while(toDouble(data[splitIndex][splitAttribute]) < attributeInfo[splitAttribute].first)
		splitIndex++;

	pair<int, int> selectedAttribute;		//stores first the index of attribute with highest gain and then the index to split attribute on
	selectedAttribute.first = splitAttribute;
	selectedAttribute.second = splitIndex;

	//cout << "att - " << splitAttribute << "  index - " << splitIndex;

	return selectedAttribute;
}

void splitData(vector<vector<string> > data, vector<vector<string> >&left, vector<vector<string> >&right, double threshold, int column) {
	for(int i=0; i < data.size(); i++) {
		if(toDouble(data[i][column]) < threshold)
			left.push_back(data[i]);
		else if(toDouble(data[i][column]) >= threshold)
			right.push_back(data[i]);
	}
}

node* buildTree(vector<vector<string> > data, vector<int> attributeList) {
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

	if(attributeList.size() == 0) {

		newNode->data = data;
		newNode->label = largestClass;
		newNode->isLeaf = true;
		newNode->left = NULL;
		newNode->right = NULL;
		return newNode;
	}

	pair<int, int> splitAttribute = findSplitAttribute(data, attributeList);
	int attribute = splitAttribute.first;
	int splitIndex = splitAttribute.second;

	int tempIndex = find(attributeList.begin(), attributeList.end(), attribute) - attributeList.begin();
	attributeList.erase(attributeList.begin() + tempIndex);

	quickSort(data, 0, data.size()-1, data.size(), attribute);

	stringstream ss;
	ss << attribute;

	newNode->data = data;
	newNode->label = ss.str();
	newNode->isLeaf = false;
	newNode->left = NULL;
	newNode->right = NULL;

	vector<vector<string> > left(data.begin(), data.begin() + splitIndex);
	vector<vector<string> > right(data.begin() + splitIndex, data.end());

	if(left.size() == 0) {
		node* leftNode = new node;
		leftNode->label = attribute;
		leftNode->isLeaf = true;
		leftNode->left = NULL;
		leftNode->right = NULL;
	}
	else
		newNode->left = buildTree(left, attributeList);

	if(right.size() == 0) {
		node* rightNode = new node;
		rightNode->label = attribute;
		rightNode->isLeaf = true;
		rightNode->left = NULL;
		rightNode->right = NULL;
	}
	else
		newNode->right = buildTree(right, attributeList);

	return newNode;
}

void printTree(ofstream &fout, node* currentNode, string prefix, int depth) {
	if(currentNode->isLeaf == true)
		prefix = "leaf: ";
	else
		prefix = "node: ";

	if(currentNode == NULL)
		return;

	fout << setw(depth * 5) <<  prefix << (currentNode->label) << endl;

	if(currentNode->left != NULL)
		printTree(fout, currentNode->left, prefix, depth + 1);

	if(currentNode->right != NULL)
		printTree(fout, currentNode->right, prefix, depth + 1);
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


