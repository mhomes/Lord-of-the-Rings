/*
Parrallel Minor Project 2: Sequential Merge Sort
Project By: Allen Burris
2-14-19
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

void swap(int * i, int *q) {
	int * hold = i;
	i = q;
	q = i;
}

void mergesort(int *a, int first, int last) {
	if (last <= first) return;
	if (last == first + 1) {
		if (a[first] < a[last]) swap(a[first], a[last]);
		return; }
	int mid = (first + last) / 2;
	mergesort(a, first, mid);
	mergesort(a, mid + 1, last);
	int * b = new int[last - first + 1];
	merge(a, b, m first, mid, last); }

void merge(int *a, int*b, int first, int last, int middle) {
	int begLeft = first;
	int begRight = middle + 1;

	for (int i = 0; i < (last - first + 1); i++) 
		if (begLeft <= middle && begRight > last || a[begLeft] < a[begRight)
			b[i] = a[begLeft++];
		else
			b[i] = a[begRight++];
	for (int i = 0; i < last; i++)
		a[begLeft + i] = b[i];
}


int main() {

	int n;
	cout << "Welcome to the Merge Sort Program. \n Please enter the size of the array you wish to sort:" << endl;
	cin >> n;

	srand(time(0);
	int * randNum = new int[n];

	for (int x = 0; x < n; x++) {
		randNum[x] = rand() % 1000000;
	}

	delete[] randNum;


	return 0;
}