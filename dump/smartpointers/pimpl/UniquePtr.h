/*
	Let's first make a class that just stores a pointer,
	taking care of deletion for us. We'll assume that it
	isn't an array (aka delete X is ok)

	Things it needs
	- exclusive ownership: you own what you point to and therefore
       you must take care of it
    - move only: it cannot be copied

    I think it's not super unreasonable to create a class that associates
    a pointer of some templated type with a destructor of some templated
    type. This is basically saying hey we want you to point to this object
    and when you're done, call this delete function on it. It saves the
    programmer from having to explicitly call delete.
*/

#ifndef UNIQUE_PTR_H
#define UNIQUE_PTR_H

#include "BasePtr.h"

// T is the type object that it pointed to
template <class T>
class UniquePtr : public BasePtr<T>
{
public:
	  // default constructor points to some garbage value
	  // like raw pointers, this is quite dangerous!
	UniquePtr();
	  // can construct with some allocated memory (a pointer to it)
	  // and a mechanism for deleting that memory
	UniquePtr(T*, void (*) (T*));
	  // construct with just a deletion function
	UniquePtr(void (*) (T*));
	  // copy contructor: this steals from other
	UniquePtr(UniquePtr<T>&);
	  // move constructor for temporary values that are passed in to
	  // a constructor, transfers ownership over to me (this)
	  // move constr is called when object is initialized using std::move
	  // or when it's initialized using the return value of a function
	  // https://en.cppreference.com/w/cpp/language/move_constructor
	UniquePtr(UniquePtr<T>&&);


	  // clean up the memory by calling the delete function you
	  // were given
	virtual ~UniquePtr();

	  // assignment operator (have it point to something new) clears
	  // what it pointed to before and sets it to point to some other
	  // object of the same type. The other UniquePtr then must NOT point
	  // to that same object.
	UniquePtr<T>& operator=(UniquePtr<T>&);

	  // should also be able to reassign it to new memory
	void reset(T*, void (*) (T*)) override;

	  // I've left out the release function on purpose. I don't think
	  // that you should ever let allocated memory run loose in a program
	  // at least you shouldn't be able to let that happen using
	  // this tool.

	int getNumReferences() const override {
		return 1;
	}

private:
	  // the main storage is a raw pointer to the
	  // allocated memory - notice that this follows
	  // in a cheap way the PImpl idiom since the real
	  // implementation of a pointer is the raw pointer
	  // and we're extending some of it's functionality 
	  // without inheriting form it (which you can't do)

	  // store a reference to a pointer so that this one is
	  // an alias for the base class's pointer.
	T*& m_pointee;
	  // pointer to a function that returns void
	  // and takes in a pointer to a T (deletes it)
	void (*m_deleteFunc) (T*);

	  // since only one can point to the object, need
	  // a function to remove one
	void clearMemory();

	  // reset takes in a pointer to T, deletes the memory that this
	  // currently manages. Then has this manage the pointer. Note that
	  // the delete functions doesn't need to nor does it change the
	  // deleter function. You'll delete objects of type T the same
	  // way no matter which specific T is being pointer to
	// void reset(T*);
};

using namespace std;

  // garbage constructor
template<class T>
UniquePtr<T>::UniquePtr()
  // call the BasePtrImpl constructor first
  : m_pointee(BasePtr<T>::m_pointee), m_deleteFunc(nullptr)
{
}

  // normal constructor
template<class T>
UniquePtr<T>::UniquePtr(T* pointee, void (*dFunc) (T*))
  // call the BasePtrImpl with these args first and init reference
  : BasePtr<T>(pointee), m_pointee(BasePtr<T>::m_pointee), m_deleteFunc(dFunc)
{
}


  // constructor that doesn't refer to allocated memory - should just
  // store the deletion function - BasePtr default constructor sets
  // m_pointee to nullptr
template<class T>
UniquePtr<T>::UniquePtr(void (*dFunc) (T*))
 : m_pointee(BasePtr<T>::m_pointee), m_deleteFunc(dFunc)
{
}


template<class T>
UniquePtr<T>::UniquePtr(UniquePtr<T>& other)
 : BasePtr<T>(other.m_pointee), m_pointee(BasePtr<T>::m_pointee),
   m_deleteFunc(other.m_deleteFunc)
{
	  // other can't point to it anymore
	other.m_pointee = nullptr;
}

  // move constructor
template<class T>
UniquePtr<T>::UniquePtr(UniquePtr<T>&& moved)
  // fine to just give BasePtr m_pointee since that's all it cares about
 : BasePtr<T>(moved.m_pointee), m_pointee(BasePtr<T>::m_pointee),
   m_deleteFunc(moved.m_deleteFunc)
{
	  // they forget so no two things have it at once
	moved.m_pointee = nullptr;
}

template<class T>
UniquePtr<T>::~UniquePtr()
{
	  // if the pointer hasn't been cleared, delete it 
	  // with the function provided
	clearMemory();
}


  // with unique pointer, only one can point to it, so delete
  // one but keep the other
template<class T>
UniquePtr<T>& UniquePtr<T>::operator=(UniquePtr<T>& rhs) {
	if (this != &rhs) {

		  // ensure that the memory this pointed to
		  // is freed since no more than one can point
		  // there
		clearMemory();

		  // point to the other one's memory
		BasePtr<T>::m_pointee = rhs.BasePtr<T>::m_pointee;
		m_pointee = BasePtr<T>::m_pointee;
		m_deleteFunc = rhs.m_deleteFunc;   // learn how to delete it

		  // rhs needs to forget - so only i have ownership
		rhs.m_pointee = nullptr;
		rhs.BasePtr<T>::m_pointee = nullptr;
	}

	return *this;

	  // after this function, rhs's destructor will be called,
	  // so it'll delete nothing except for rhs itself
}


  // we're making a trade off here as well. The user of this
  // class has to reuse some code - they have to pass in the same
  // delete function every time they reset the object pointed to.
  // The thing is that if we offered a reset function that only
  // takes in a new object to point to, then the user could leak
  // memory. They could construct an empty UniquePtr then reset
  // it with new memory but not provide a delete function. Then
  // the SmartPtr doesn't know how to delete it

  // don't ever need to reset the deleter function
  // because you always delete an object of type T
  // the same way. This limits functionality a little
  // but serves the lab pretty well.
template<class T>
void UniquePtr<T>::reset(T* objPtr, void (*dFunc) (T*)) {

	  // if you're reseting to the same object, do nothing
	if (m_pointee != objPtr) {
		clearMemory();
		BasePtr<T>::m_pointee = objPtr;
		m_pointee = BasePtr<T>::m_pointee;
		m_deleteFunc = dFunc;
	}
}


  // Function that enforces the requirement that only one
  // UniquePtr can point to an allocated object at a time.
  // Whenever a UniquePtr is re-assigned, need to clear the
  // memory that it has.
template<class T>
void UniquePtr<T>::clearMemory() {

	  // free the memory if it's allocated
	if (m_pointee != nullptr && m_deleteFunc != nullptr) {
		m_deleteFunc(m_pointee);  // free the memory
		  // remember that it's freed - avoid double deletion!!!
		m_pointee = nullptr;
	}
}


#endif














/*
  Actually not good to have these operators since if we do, we
  allow the UniquePtr to behave like a bool in many more situations
  than just if (uptr)...
template<class T>
UniquePtr<T>::operator bool() const {
	return m_pointee != nullptr;
}


template<class T>
UniquePtr<T>::operator!() const {
	return m_pointee == nullptr;
}
*/
