// twothree.cpp
// Allen Burris and Mathew Homes
// Parrallel Algorithms Poject 4:
// 2-3 Trees
//

#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;
struct node;
int max(node* n);

struct node {
	node* parent = NULL;
	node* leftchild = NULL;
	node* rightchild = NULL;
	node* middlechild = NULL;

	node* orphen = NULL;

	int value;
	int * nav;
	int numchildren;

	node(node* p) {
		parent = p;
		nav = new int[3];
	}

	void printNode() {
		if (numchildren < 2)
			cout << "This is a leaf. Value = " << value << endl;
		else
			cout << "Parent = " << parent << " ";
	}

	void setNav() {
		nav[0] = max(this->leftchild);
		nav[1] = max(this->middlechild);
		nav[2] = max(this->rightchild);
	}
};

int max(node* n) {

	if (n->numchildren > 2)
		return n->value;
	else
		return max(n->rightchild);
	return -4;
}

node search(node n, int target) {
	
	//tree is just root
	if (n.numchildren > 2)
		if (target <= n.value)
			return n;
		else return NULL;
	else if (target > n.nav[2])
		return NULL;
	else {
		if (target < n.nav[0])
			return search(n.leftchild, target);
		else if (target < n.nav[1])
			return search(n.middlechild, target);
		else
			return search(n.rightchild, target);
	}
}

int main() {

	node root = new node(NULL);

	cout << "Gupta" << endl;
	return 0;
}