/*
	TO CONSIDER:
	If I write an Implementation base class then I can take a SmartPtr that
	was implemented as a UniquePtr and changed it to a SmartPtr implementation
	That could be cool.
*/

#ifndef SMART_PTR_H
#define SMART_PTR_H

#include "BasePtr.h"
#include <utility>

template<class Object, template<class> class Impl>
class SmartPtr {
public:
	SmartPtr();
	  // main constructor takes in a pointer to a function
	  // for allocated memory and a function for deleting
	  // that memory
	SmartPtr(Object* (*) (), void (*) (Object*));

	  // constructor for giving it just a deletion function
	  // but no memory to keep track of
	SmartPtr(void (*) (Object*));

	  // copy contructor
	SmartPtr(SmartPtr<Object, Impl>&);

	  // move constructor
	SmartPtr(SmartPtr<Object, Impl>&&);

	  // destructor calls the Impl's destructor
	~SmartPtr();

	  // assignment operator is pawned off to Impl
	SmartPtr<Object, Impl>& operator=(SmartPtr<Object, Impl>&);
	
	  // have it point to something else - must include delete function
	  // even if it hasn't changed
	void reset(Object*, void (*) (Object*));

	/*
		All the public functions below here are ubiquitous
		across implentations of a pointer, so we can keep
		them in this class, not the implementation.
	*/

	  // access and modify const is optional I'd say

	  // get the value in the memory pointed to
	  // return a reference so that you can change it
	Object& operator*() const {
		return m_implPtr->operator*();
	}

	  // get back a pointer to the memory so that you
	  // can call member functions if it's a class
	Object* operator->() const {
		return m_implPtr->operator->();
	}

	  // access the element stored some offset away
	  // from the pointer. If the memory allocated
	  // isn't an array, it does the same thing but
	  // anything could be stored there - same as raw
	  // pointers.
	Object& operator[](int offset) const {
		return m_implPtr->operator[](offset);
	}

	  // get the memory location some offset away from
	  // what the UniquePtr points to
	Object* operator+(int offset) const {
		return m_implPtr->operator+(offset);
	}

	Object* operator-(int offset) const {
		return m_implPtr->operator-(offset);
	}

	  // boolean functions to see if it's nullptr - BAD
	  // Modern C+ Design page 174-175
	// operator bool() const;
	// operator!() const;

	  // instead maybe we'll just overload the operators that compare it
	  // to nullptr (which we do often) and to another SmartPtr
	bool operator==(Object* rhs) {
		return m_implPtr->operator==(rhs);
	}

	bool operator!=(Object* rhs) {
		return m_implPtr->operator!=(rhs);
	}

	bool operator==(SmartPtr<Object, Impl>& rhs) {
		return m_implPtr->operator==(*rhs.m_implPtr);
	}

	bool operator!=(SmartPtr<Object, Impl>& rhs) {
		return m_implPtr->operator!=(*rhs.m_implPtr);
	}

	void swap(SmartPtr<Object, Impl>&);

	int getNumReferences() const {
		return m_implPtr->getNumReferences();
	}

private:
	  // store the pointer to the implementation - if you want to be
	  // able to swap implementations then you have to store a pointer
	  // to the base class, utilizing polymorphism here
	Impl<Object>* m_implPtr;
};


  // default constructor sets the pointers to nullptr and
  // constructs a new Impl with its default constructor
template<class Object, template<class> class Impl>
SmartPtr<Object, Impl>::SmartPtr()
{
	  // default constructor
	m_implPtr = new Impl<Object>();
}


template<class Object, template<class> class Impl>
SmartPtr<Object, Impl>::SmartPtr(Object* (*newFunc) (),
							     void (*deleteFunc) (Object*))
{
	  // the implementation takes care of what happens when we re-assign
	  // delete, reset, etc
	m_implPtr = new Impl<Object>(newFunc(), deleteFunc);
}


template<class Object, template<class> class Impl>
SmartPtr<Object, Impl>::SmartPtr(void (*deleteFunc) (Object*))
{
	m_implPtr = new Impl<Object>(deleteFunc);
}


template<class Object, template<class> class Impl>
SmartPtr<Object, Impl>::SmartPtr(SmartPtr<Object, Impl>& other)
{
	m_implPtr = new Impl<Object>(*other.m_implPtr);
}


  // move constructor
template<class Object, template<class> class Impl>
SmartPtr<Object, Impl>::SmartPtr(SmartPtr<Object, Impl>&& mover)
{
	  // call impl's move constructor - is there a better way than using
	  // std::move??? I don't think there is
	m_implPtr = new Impl<Object>(std::move(*mover.m_implPtr));
}


  // copy over rhs's variables into this and ask Impl what to do
  // with pointee - assignment operator here says this wants to point
  // to what rhs is pointing to
template<class Object, template<class> class Impl>
SmartPtr<Object, Impl>& SmartPtr<Object, Impl>::operator=(SmartPtr<Object, Impl>& rhs)
{
	if (this != &rhs) {

		  // take rhs's implementation (operator=)
		*m_implPtr = *rhs.m_implPtr;

		/*
			This is where the BasePtr* vs Impl* trade-off comes in. If I do it as I am
			I can have the BasePtr class take care of the things that all pointers do.
			But the problem is that in SmartPtr I don't know which kind of Ptr i have.
			So when I want to pass an object from one pointer to another, I can't know
			which function to call without doing some trial and error casting. I guess
			this is summarized as I can either provide a type guarantee or I can know
			the type that's stored as the cost of having to duplicate some code or I
			can play some casting games but save duplicating some code.
		*/
	}

	return *this;
}


template<class Object, template<class> class Impl>
void SmartPtr<Object, Impl>::reset(Object* obj, void (*dFunc) (Object*))
{
	m_implPtr->reset(obj, dFunc);
}


  // destructor has the Impl clean up memory
  // and in doing so cleans up its own memory
template<class Object, template<class> class Impl>
SmartPtr<Object, Impl>::~SmartPtr()
{
	  // Impl<T>::~Impl() - Impl has to take care
	  // of allocating the memory, it's part of the 
	  // implementation
	delete m_implPtr;
}


  // implementations don't do anything differen in terms of
  // swapping - you're just swapping what some smart ptr with
  // the same implementation points to
template<class Object, template<class> class Impl>
void SmartPtr<Object, Impl>::swap(SmartPtr<Object, Impl>& other) {

	Impl<Object>* thisimpl = m_implPtr;

	m_implPtr = other.m_implPtr;

	other.m_implPtr = thisimpl;
}

#endif