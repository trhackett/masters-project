
#ifndef SHARED_PTR_H
#define SHARED_PTR_H

#include <iostream>
using namespace std;
#include "BasePtr.h"

template<class T>
class SharedPtr : public BasePtr<T>
{
public:

	  // keep in mind the default constructor for BasePtr
	  // gets called before any of these constructors run
	  // (order of construction)

	  // default constructor should call BasePtr's default
	  // constructor and initialize the ReferenceCount
	SharedPtr();
	  // normally used constructor
	SharedPtr(T*, void (*) (T*));
	  // Copy Constructor is ok - it have this point to what
	  // other points to and adds one to the reference count
	  // that is shared between then
	SharedPtr(SharedPtr&);
	  // move contructor
	SharedPtr(SharedPtr&&);

	  // destructor takes one off of the count and maybe destroys
	  // the pointed to object
	virtual ~SharedPtr();

	  // assignment operator takes one off of this count (deleting
	  // it is it's 0) and have this point to something else
	  // aka to what another shared pointer is pointing to
	SharedPtr& operator=(SharedPtr&);
	
	void reset(T*, void (*) (T*)) override;

	  // if it's an invalid reference (has been moved to another),
	  // then return -1
	int getNumReferences() const {
		if (m_refCount != nullptr) {
			return m_refCount->getCount();
		} 

		return -1;
	}

	  // the rest of the functions do the same as BasePtr

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
		int getCount() const    { return numRefs; }
	};

	  // I can't think of a way around storing a dynamically
	  // allocated RefCount. Multiple objects need to know about
	  // the same one so we'd want ideally to use a SharedPtr
	  // but we can't do that
	RefCount* m_refCount;

	  // pointer to a function that returns void
	  // and takes in a pointer to a T (deletes it)
	void (*m_deleteFunc) (T*);

	  // the main storage is a raw pointer to the
	  // allocated memory - notice that this follows
	  // in a cheap way the PImpl idiom since the real
	  // implementation of a pointer is the raw pointer
	  // and we're extending some of it's functionality 
	  // without inheriting form it (which you can't do)

	  // refers to the same thing BasePtr points to but takes
	  // care of the freeing of memory!!!
	T*& m_pointee;

	  // function that decrements the reference since you no longer
	  // point to the object, checks to see if you're the last pointer
	  // to it and if so and it exists, deletes it
	void removeReference();
};



template<class T>
SharedPtr<T>::SharedPtr()
 : BasePtr<T>(), m_pointee(BasePtr<T>::m_pointee)
{
	m_deleteFunc = nullptr;
	m_refCount = new RefCount;
}


  // become the first reference pointing to this object
template<class T>
SharedPtr<T>::SharedPtr(T* pointee, void (*dFunc) (T*))
 : BasePtr<T>(pointee), m_pointee(BasePtr<T>::m_pointee)

{
	m_deleteFunc = dFunc;
	m_refCount = new RefCount;
}


  // share this object with another
template<class T>
SharedPtr<T>::SharedPtr(SharedPtr<T>& other)
  // make a copy of the pointer
 : BasePtr<T>(other.m_pointee), m_pointee(BasePtr<T>::m_pointee)
{
	m_deleteFunc = other.m_deleteFunc;
	  // if other is pointing to nullptr and the RefCount
	  // is 0, then you better not add anything to RefCount
	  // since then you'll try to delete null memory

	  // actually nevermind, if it's nullptr, you should
	  // still point to it and have multiple reference because
	  // if you give one of them memory later, you should
	  // make sure you're counting all the references
	m_refCount = other.m_refCount;
	if (m_refCount != nullptr) {
		m_refCount->increment();
	}
}


template<class T>
SharedPtr<T>::SharedPtr(SharedPtr<T>&& mover)
 : BasePtr<T>(mover.m_pointee), m_pointee(BasePtr<T>::m_pointee)
{
	m_refCount = mover.m_refCount;
	  // ref count hasn't changed since mover no longer points to
	  // the object and this now does - it's a wash
	m_deleteFunc = mover.m_deleteFunc;

	mover.m_pointee = nullptr;
	mover.m_refCount = nullptr;
}



template<class T>
SharedPtr<T>::~SharedPtr() {

	  // decrement and if you're the last reference
	  // delete the object and the RefCount if there is one
	removeReference();
}


template<class T>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<T>& rhs) {

	  // if they are different and they don't already refer to
	  // the same data, then share the reference
	if (this != &rhs && m_pointee != rhs.m_pointee) {

		  // if you're the last reference, delete
		removeReference();

		BasePtr<T>::m_pointee = rhs.m_pointee;  // copy the pointer over
		m_pointee = BasePtr<T>::m_pointee;      // refer to base's pointer
		m_refCount = rhs.m_refCount;            // same reference count

		if (m_refCount != nullptr) {
			m_refCount->increment();            // now one more reference
		}
	}

	  // if they point to the same data, then you shouldn't change
	  // anything since these SharedPtr's (this and rhs) both already
	  // will have incremented the reference count
	return *this;
}



  // operator= for having it point to other data, need to remove reference
  // for what it's currently pointing to and have it point to something else
  // it must be the case that this is the only reference to this, or the
  // programmer has made a huge mistake
template<class T>
void SharedPtr<T>::reset(T* objptr, void (*dFunc) (T*))
{
	  // no change if they are already pointing to the same thing
	if (m_pointee != objptr) {
		  // no longer pointing to your thing
		removeReference();

		  // pointing to new thing with new ref count
		BasePtr<T>::m_pointee = objptr;
		m_pointee = BasePtr<T>::m_pointee;
		m_deleteFunc = dFunc;
		m_refCount = new RefCount;
	}
}



template<class T>
void SharedPtr<T>::removeReference() {

	  // m_ref could be nullptr if the reference was moved
	  // to another SmartPtr
	if (m_refCount != nullptr) {
		m_refCount->decrement();

		  // if you're the last reference, delete
		if (m_refCount->getCount() == 0) {
			  // if it's pointing to something, delete it
			if (m_pointee != nullptr && m_deleteFunc != nullptr) {
				m_deleteFunc(m_pointee);
			}

			  // always delete reference
			delete m_refCount;
		}
	}
}


#endif