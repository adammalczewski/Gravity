#include "Heap.h"

Heap::Heap(){
	size = 0;
}

Heap::~Heap(){

}

Col Heap::Take(){


	if (size > 0){
		Col x = heap[1];
		remove(1);
		return x;
	}

}

double Heap::Top(){
  return heap[1].t;      
}

void Heap::Add(Col x){

	heap[++size] = x;
	up(size);

}

void Heap::swap(int i, int j){

	Col	temp = heap[i];
	heap[i] = heap[j];
	heap[j] = temp;

}

void Heap::up(int i){

	int current = i;
	int parent = i/2;

	while (parent > 0 && heap[current].t < heap[parent].t){

		swap(parent,current);
		current = parent;
		parent = current/2;

	}

}

void Heap::down(int i){

	int parent = i, left = i*2, right = i*2 + 1;
	int best;

	if (left <= size && heap[left].t < heap[parent].t){
		best = left;
	} else {
		best = parent;
	}

	if (right <= size && heap[right].t < heap[best].t){
		best = right;
	}

	if (best != parent){

		swap(best,parent);
		down(best);

	}

}

void Heap::remove(int i){


	heap[i] = heap[size--];
	
	if (i <= size){
		down(i);
	}

}

int Heap::GiveSize(){

	return size;

}
