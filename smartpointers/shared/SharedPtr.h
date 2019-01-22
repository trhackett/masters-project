
#ifndef SHARED_PTR_H
#define SHARED_PTR_H

#include "UniquePtr.h"

template<class T>
class SharedPtr : public UniquePtr<T>
{
public:
	  // keep in mind the default constructor for UniquePtr
	  // gets called before any of these constructors run
	  // (order of construction)

	  // default constructor should call UniquePtr's default
	  // constructor and initialize the ReferenceCount
	SharedPtr();
	  // normally used constructor
	SharedPtr(T*, void (*) (T*));
	  // Copy Constructor is ok - it have this point to what
	  // other points to and adds one to the reference count
	  // that is shared between then
	SharedPtr(SharedPtr&);
	  // assignment operator takes one off of this count (deleting
	  // it is it's 0) and have this point to something else
	  // aka to what another shared pointer is pointing to
	SharedPtr& operator=(SharedPtr&);

	  // destructor takes one off of the count and maybe destroys
	  // the pointed to object
	virtual ~SharedPtr();

	bool operator==(SharedPtr<T>&);

	  // the rest of the functions do the same as UniquePtr

private:
	  // the RefCount only needs to track the number of
	  // references so it should be able to increment and
	  // decrement
	struct RefCount {
		int numRefs;

		  // starts with one since it's always constructed
		  // in the SharedPtr constructor (that one being the
		  // only reference)
		RefCount() : numRefs(1) { }
		void increment()        { numRefs++; }
		void decrement()        { numRefs--; }
		int getCount()          { return numRefs; }
	};

	  // I can't think of a way around storing a dynamically
	  // allocated RefCount. Multiple objects need to know about
	  // the same one so we'd want ideally to use a SharedPtr
	  // but we can't do that
	RefCount* m_refCount;

	void shareReference(SharedPtr&, SharedPtr&);
};



template<class T>
SharedPtr<T>::SharedPtr() {
	m_refCount = new RefCount;
}


template<class T>
SharedPtr<T>::SharedPtr(T* pointee, void (*dFunc) (T*)) {
	UniquePtr<T>::m_pointee = pointee;
	m_deleteFunc = dFunc;

	m_refCount = new RefCount;
}


template<class T>
SharedPtr<T>::SharedPtr(SharedPtr<T>& other) {
	  // if other is pointing to nullptr and the RefCount
	  // is 0, then you better not add anything to RefCount
	  // since then you'll try to delete null memory

	  // actually nevermind, if it's nullptr, you should
	  // still point to it nad have multiple reference because
	  // if you give one of them memory later, you should
	  // make sure you're counting all the references
	shareReference(*this, other);
}


template<class T>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<&>& rhs) {
	if (this != &other) {
		shareReference(*this, other);
	}
	return *this;
}



template<class T>
SharedPtr<T>::~SharedPtr() {

	  // decrement and if you're the last reference
	  // delete the object and the RefCount
	m_refCount->decrement();
	if (m_refCount->getCount == 0) {
		m_deleteFunc(m_pointee);
		delete m_refCount;
	} 
}


template<class T>
bool SharedPtr<T>::operator==(SharedPtr<T>& other) {
	return m_pointee == other.m_pointee;
}



template<class T>
void SharedPtr<T>::shareReference(SharedPtr<&>& recipient, SharedPtr<T>& provider) {

	  // each knows about the reference, so it has one more
	  // ref
	recipient.m_refCount = provider.m_refCount;
	recipient.m_refCount->increment;

	  // copy the pointers
	recipient.m_pointee = provider.m_pointee;
	recipient.m_deleteFunc = provider.m_deleteFunc;
}


#endif