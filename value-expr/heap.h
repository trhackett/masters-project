#include <iostream>
#include <cassert>
using namespace std;


// the heap property for a min heap is that each parent must be smaller
// than both of any direct children it has

// BASE CASE: the empty tree is a min heap
// RECURSIVE CASE: a tree is a min heap if the root is smaller than any
// of the children and the children are min heaps

// this implies that the root is always strictly less than any of its
// descendents

class MinHeap {
public:
	MinHeap() {
		m_max = 5;
		m_heap = new int[5];
		m_size = 0;
	}

	~MinHeap() {
		delete [] m_heap;
	}

	int size() const { return m_size; }
	bool empty() const { return m_size == 0; }
	int top() const { return m_heap[0]; } // going to be bad if it's empty

	// to push an element onto the heap, you put it at the end
	// of the array meaning it will be at ...
	//   the root if the array was empty
	//   the left child of the bottom-most, right-most element if it has no children
	//   the right child of the bottom-most, right-most element if it has one child
	// and then just make sure that it isn't violating the heap property
	void push(int element) {

		// if there's no more space, add some more 
		if (m_size+1 == m_max) {
			allocateMore();
		}

		m_heap[m_size] = element; // add in the element
		heapify_up(m_size); // move it to where it needs to go
		m_size++; // now you have one more element in there
	}

	void pop() {
		m_size--;
		if (m_size > 0) {
			m_heap[0] = m_heap[m_size];
			heapify_down(0);
		}
	}

	void printHeap() {
		for (int i = 0; i < m_size; ++i) {
			cout << m_heap[i] << " " ;
		}
		cout << endl;
	}

private:
	int* m_heap;
	int m_max;
	int m_size;

	void allocateMore() {
		m_max = m_max * 2;

		int* newheap = new int[m_max];
		for (int i = 0; i < m_size; i++) {
			newheap[i] = m_heap[i];
		}

		// swap them
		int* del = m_heap;
		m_heap = newheap;
		delete [] del;
	}

	void heapify_down(int index) {
		int smallest = index;
		int left = 2 * index + 1;
		int right = 2 * index + 2;

		if (left < size() && m_heap[left] < m_heap[smallest]) {
			smallest = left;
		}

		if (right < size() && m_heap[right] < m_heap[smallest]) {
			smallest = right;
		}

		// if the smallest is not already the parent
		//   then swap and heapify the rest
		// else it's already heapified
		if (smallest != index) {
			
			// do the swap
			int tmp = m_heap[smallest];
			m_heap[smallest] = m_heap[index];
			m_heap[index] = tmp;

			// move on down
			heapify_down(smallest);
		}
	}

	void heapify_up(int index) {
		// if you aren't at the root and your parent is bigger
		// than you, then swap and move up
		int parent = (index - 1) / 2;

		if (index > 0 && m_heap[parent] > m_heap[index]) {
			
			// do the swap
			int tmp = m_heap[index];
			m_heap[index] = m_heap[parent];
			m_heap[parent] = tmp;

			// move on up
			heapify_up(parent);
		}
	}
};


int* kElements(int arr[], int n, int k) {
	MinHeap mh;

	for (int i = 0; i < k; ++i) {
		mh.push(arr[k]);
	}

	for (int i = k; i < n; ++i) {
		if (mh.top() < arr[i]) {
			mh.pop();
			mh.push(arr[i]);
		}
	}

	int ret[k];
	for (int i = 0; i < k; ++i) {
		ret[i] = mh.top();
		mh.pop();
	}

	return ret;
}






/*
int main()
{
	MinHeap mh;

	assert(mh.empty());
	
	mh.push(8);
	mh.push(2);
	mh.push(15);
	
	assert(mh.size() == 3);	
	
	assert(mh.top() == 2);
	mh.pop();

	assert(mh.top() == 8);
	mh.pop();

	mh.push(5);
	mh.push(4);
	mh.push(45);

	assert(mh.size() == 4);
	
	assert(mh.top() == 4);
	mh.pop();

	assert(mh.top() == 5);
	mh.pop();

	assert(mh.top() == 15);
	mh.pop();

	assert(mh.top() == 45);
	mh.pop();

	assert(mh.empty());

	// make sure copying over works
	MinHeap m;
	for (int i = 0; i < 10; ++i) {
		m.push(i);
	}

	assert(m.size() == 10);
	for (int i = 0; i < 10; ++i) {
		assert(m.top() == i);
		m.pop();
	}

	cout << "Passed all tests!" << endl;

	return 0;
}
*/