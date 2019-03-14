/*
	TO CONSIDER:
	If I write an Implementation base class then I can take a SmartPtr that
	was implemented as a UniquePtr and changed it to a SmartPtr implementation
	That could be cool.
*/

#ifndef SMART_PTR_H
#define SMART_PTR_H

template<class Object, template<class> class Impl>
class SmartPtr {
public:
	SmartPtr();
	  // main constructor takes in a pointer to a function
	  // for allocated memory and a function for deleting
	  // that memory
	SmartPtr(Object* (*) (), void (*) (Object*));

	  // destructor calls the Impl's destructor
	~SmartPtr();

	  // assignment operator is pawned off to Impl
	SmartPtr<Object, Impl>& operator=(SmartPtr<Object, Impl>&);

	/*
		All the public functions below here are ubiquitous
		across implentations of a pointer, so we can keep
		them in this class, not the implementation.
	*/

	  // access and modify const is optional I'd say
	Object& operator*() const;
	Object* operator->() const;
	Object& operator[](int) const;
	Object* operator+(int) const;

	  // boolean functions to see if it's nullptr - BAD
	  // Modern C+ Design page 174-175
	// operator bool() const;
	// operator!() const;

	  // instead maybe we'll just overload the operators that compare it
	  // to nullptr (which we do often) and to another pointer
	bool operator==(Object*);
	bool operator!=(Object*);
	bool operator==(SmartPtr<Object, Impl>&);
	bool operator!=(SmartPtr<Object, Impl>&);

	  // functions for Impl
	void swap(SmartPtr<Object, Impl>&);

private:
	  // makes sense for SmartPtr to track the actual pointer 
	  // but this class should NEVER free the memory. that it
	  // what the pointer to the Implementation should do. We
	  // do this because somethings all pointers regardless of
	  // implementation do ([] * -> +) but besides that the
	  // the functionality varies between shared and unique
	  // pointers
	Object* m_pointee;
	  // for now, store the pointer to the allocation function
	  // but I don't think you're going to use it
	Object* (*m_newFunc) ();
	  // store the pointer to the free function - returns void
	  // and takes as input a pointer to the stores object
	void (*m_deleteFunc) (Object*);
	  // store the pointer to the Impl
	Impl<Object>* m_implPtr;
};


  // default constructor sets the pointers to nullptr and
  // constructs a new Impl with its default constructor
template<class Object, template<class> class Impl>
SmartPtr<Object, Impl>::SmartPtr()
{
	m_pointee = nullptr;
	m_newFunc = nullptr;
	m_deleteFunc = nullptr;

	  // default constructor
	m_implPtr = new Impl<Object>();
}


template<class Object, template<class> class Impl>
SmartPtr<Object, Impl>::SmartPtr(Object* (*newFunc) (),
							     void (*deleteFunc) (Object*))
{
	m_newFunc = newFunc;
	m_deleteFunc = deleteFunc;

	  // allocate the pointer here, have the implementation
	  // delete it
	m_pointee = newFunc();

	  // the implementation takes care of what happens when we re-assign
	  // swap, and perhaps reference counts
	m_implPtr = new Impl<Object>(m_pointee, m_deleteFunc);
}


  // copy over other's variables into this and ask Impl what to do
  // with pointee
template<class Object, template<class> class Impl>
SmartPtr<Object, Impl>& SmartPtr<Object, Impl>::operator=(SmartPtr<Object, Impl>& other)
{
	if (this != &other) {

		  // take other's implementation (operator=)
		*m_implPtr = *other.m_implPtr;

		  // takes other's functions and pointer
		m_newFunc = other.m_newFunc;
		m_deleteFunc = other.m_deleteFunc;
		m_pointee = other.m_pointee;
	}

	return *this;
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


  // get the value in the memory pointed to
  // return a reference so that you can change it
template<class Object, template<class> class Impl>
Object& SmartPtr<Object, Impl>::operator*() const {
	return *m_pointee;
}


  // get back a pointer to the memory so that you
  // can call member functions if it's a class
template<class Object, template<class> class Impl>
Object* SmartPtr<Object, Impl>::operator->() const {
	return m_pointee;
}


  // access the element stored some offset away
  // from the pointer. If the memory allocated
  // isn't an array, it does the same thing but
  // anything could be stored there - same as raw
  // pointers.
template<class Object, template<class> class Impl>
Object& SmartPtr<Object, Impl>::operator[](int i) const {
	return m_pointee[i];
}



  // implementations don't do anything differen in terms of
  // swapping.
template<class Object, template<class> class Impl>
void SmartPtr<Object, Impl>::swap(SmartPtr<Object, Impl>& other) {
	  // copies for swapping
	Object* thisptr = m_pointee;
	Object* (*thisnewfunc) () = m_newFunc;
	void (*thisdelfunc) (Object*) = m_deleteFunc;
	Impl<Object>* thisimpl = m_implPtr;

	m_pointee = other.m_pointee;
	m_deleteFunc = other.m_deleteFunc;
	m_newFunc = other.m_newFunc;
	m_implPtr = other.m_implPtr;

	other.m_pointee = thisptr;
	other.m_newFunc = thisnewfunc;
	other.m_deleteFunc = thisdelfunc;
	other.m_implPtr = thisimpl;
}


  // get the memory location some offset away from
  // what the UniquePtr points to
template<class Object, template<class> class Impl>
Object* SmartPtr<Object, Impl>::operator+(int offset) const {
	return m_pointee + offset;
}


template<class Object, template<class> class Impl>
bool SmartPtr<Object, Impl>::operator==(Object* other) {
	return m_pointee == other;
}

template<class Object, template<class> class Impl>
bool SmartPtr<Object, Impl>::operator!=(Object* other) {
	return m_pointee != other;
}

template<class Object, template<class> class Impl>
bool SmartPtr<Object, Impl>::operator==(SmartPtr<Object, Impl>& other) {
	return m_pointee == other.m_pointee;
}

template<class Object, template<class> class Impl>
bool SmartPtr<Object, Impl>::operator!=(SmartPtr<Object, Impl>& other) {
	return m_pointee != other.m_pointee;
}


#endif