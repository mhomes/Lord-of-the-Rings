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
//void absorb(node * n, node * s);

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
		numchildren = 0;
		nav[0] = 0;
		nav[1] = 0; 
		nav[2] = 0; 
	}

	void removeChild(node * n) {
		if (n->value == this->leftchild->value) {
			this->leftchild = this->middlechild;
			this->middlechild = NULL;
		}
		else if (n->value == this->middlechild->value)
			this->middlechild = NULL;
		else if (n->value == this->rightchild->value) {
			this->rightchild = this->middlechild;
			this->middlechild = NULL;
		}
	}

	void printNode() {
		if (numchildren < 2)
			cout << "This is a leaf. Value = " << value << endl;
		else
			cout << "Numchildren = " << numchildren << " ";
	}

	void orderChildren() {

		node * temp;
		if(numchildren == 2)
			if (leftchild->value > rightchild->value) {
				temp = leftchild;
				leftchild = rightchild;
				rightchild = temp;
			}
			else if (numchildren == 3) {
				if (leftchild->value > middlechild->value) {
					temp = leftchild;
					leftchild = middlechild;
					middlechild = temp;
				}
				if (middlechild->value > rightchild->value) {
					temp = middlechild;
					middlechild = rightchild;
					rightchild = temp;
				}
			}
		delete temp;
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

	//target is bigger than largest element in tree
	if (n.nav[2] < target)
		return NULL;
	
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
		else if (target < n.nav[2])
			return search(n.rightchild, target);
	}
}

void absorb(node * n, node * s) {
	if (s->parent == NULL) {
		node oldRoot = s;

		s->leftchild = n;
		n->parent = s;

		s->rightchild = &oldRoot;
		oldRoot.parent = n;

		s->value == 0;
		s->setNav();
		return;
	}
	node * p = s->parent;
	if (p->numchildren < 3) {
		if (p->numchildren == 2) {
			p->middlechild = n;
			n->parent = p;
			p->numchildren++;
			p->orderChildren();
		}
	}
	else { //There are three children already
		node * pOverFlow = new node(p->parent);
		pOverFlow->numchildren = 2;
		p->numchildren = 2;
		int pos;
		for (int i = 0; i < 3; i++) //Determine where the new node falls in the existing tree.
			if (p->nav[i] == s->value)
				pos = i;

		if (pos == 0) {
			pOverFlow->leftchild = n;
			n->parent = pOverFlow;
			pOverFlow->rightchild = p->leftchild;
			p->leftchild->parent = pOverFlow;

			p->leftchild = p->middlechild;
			p->middlechild = NULL;
		}
		else if (pos == 1) {
			pOverFlow->leftchild = p->leftchild;
			p->leftchild->parent = pOverFlow;
			pOverFlow->rightchild = n;
			n->parent = pOverFlow;

			p->leftchild = p->middlechild;
			p->middlechild = NULL;
		}
		else if (pos == 2) {
			pOverFlow->leftchild = p->leftchild;
			p->leftchild->parent = pOverFlow;
			pOverFlow->rightchild = p->middlechild;
			p->middlechild->parent = pOverFlow;

			p->leftchild = n;
			p->middlechild = NULL;
		}
		p->setNav();
		absorb(pOverFlow, p);
	}
}

void insert(node t, node n) {
	//where s is the successor to n.
	node s = search(t, n.value);

	if (t.value == n.value)
		return;
	absorb(&n, &s);
	t.setNav();
}

void discard(node * r, node * n) {
	node * p = n->parent;
	int pos;
	p->removeChild(n);
	p->numchildren--;
	p->setNav();

	if (p->numchildren == 2) {
		p->setNav();
		return;
	}

	//we have 1 child
	int allChild = 0;
	allChild += p->parent->leftchild->numchildren;
	allChild += p->parent->middlechild->numchildren;
	allChild += p->parent->rightchild->numchildren;

	if (allChild >= 4) {
		if (allChild == 5 && p->numchildren == 1)
			if (p->leftchild != NULL)
				if (p->leftchild->value < p->parent->nav[2])
					if (p->parent->numchildren > 3)
						p->parent->leftchild->middlechild = p->leftchild;
					else
						p->parent->rightchild->middlechild = p->leftchild;
				else
					p->parent->rightchild->middlechild = p->leftchild;
		if (allChild != 5)
			p->numchildren--;

		p->orderChildren();
		p->setNav();
		r->setNav();

		return;
	}

	//children = 3
	node * last;
	if (p->leftchild != NULL)
		last = p->leftchild;
	else if (p->middlechild != NULL)
		last = p->middlechild;
	else if (p->rightchild != NULL)
		last = p->rightchild;

	if (p->parent == NULL) {
		r = last;
		return;
	}
	//p is not the root
	p->orderChildren();
	r->setNav();
	p->parent->middlechild = last;
	return discard(r, p);

}

void Delete(node t, node n) {
	node b = search(t, n.value);

	if (b.value == n.value)
		discard(&t, &n);
	t.setNav();
	return;
}

int main() {

	node root = new node(NULL);

	cout << "Gupta" << endl;
	return 0;
}