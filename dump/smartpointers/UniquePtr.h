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

#ifndef UNIQUEPTR_H
#define UNIQUEPTR_H

// T is the type object that it pointed to
template <class T>
class UniquePtr {
public:
	  // default constructor points to some garbage value
	  // like raw pointers, this is quite dangerous!
	UniquePtr();
	  // can construct with some allocated memory (a pointer to it)
	  // and a mechanism for deleting that memory
	UniquePtr(T*, void (*) (T*));
	  // move constructor for temporary values that are passed in to
	  // a constructor, transfers ownership over to me (this)
	UniquePtr(UniquePtr<T>&&);


	  // clean up the memory by calling the delete function you
	  // were given
	~UniquePtr();

	  // assignment operator (have it point to something new) clears
	  // what it pointed to before and sets it to point to some other
	  // object of the same type. The other UniquePtr then must NOT point
	  // to that same object.
	UniquePtr<T>& operator=(UniquePtr<T>&);

	  // swap the memory that's pointed to as well as the deleter to
	  // it can be freed
	void swap(UniquePtr<T>&);

	  // reset takes in a pointer to T, deletes the memory that this
	  // currently manages. Then has this manage the pointer. Note that
	  // the delete functions doesn't need to nor does it change the
	  // deleter function. You'll delete objects of type T the same
	  // way no matter which specific T is being pointer to
	void reset(T*);

	  // I've left out the release function on purpose. I don't think
	  // that you should ever let allocated memory run loose in a program
	  // at least you shouldn't be able to let that happen using
	  // this tool.

	  // access and modify const is optional I'd say
	T& operator*() const;
	T* operator->() const;
	T& operator[](int) const;
	T* operator+(int) const;
	operator bool() const;

private:
	  // the main storage is a raw pointer to the
	  // allocated memory
	T* m_pointee;
	  // pointer to a function that returns void
	  // and takes in a pointer to a T (deletes it)
	void (*m_deleteFunc) (T*);

	  // since only one can point to the object, need
	  // a function to remove one
	void clear();
};

#endif

using namespace std;

  // garbage constructor
template<class T>
UniquePtr<T>::UniquePtr()
{
	m_pointee = nullptr;
	m_deleteFunc = nullptr;
}

  // normal constructor
template<class T>
UniquePtr<T>::UniquePtr(T* pointee, void (*dFunc) (T*))
{
	m_pointee = pointee;
	m_deleteFunc = dFunc;
}

  // move constructor
template<class T>
UniquePtr<T>::UniquePtr(UniquePtr<T>&& moved)
{

	  // learn what they pointed to
	m_pointee = moved.m_pointee;
	m_deleteFunc = moved.m_deleteFunc;

	  // they forget
	moved.m_pointee = nullptr;
	moved.m_deleteFunc = nullptr;
}

template<class T>
UniquePtr<T>::~UniquePtr()
{
	  // if the pointer hasn't been cleared, delete it 
	  // with the function provided
	if (m_deleteFunc != nullptr && m_pointee != nullptr) {
		m_deleteFunc(m_pointee);
	}
}

template<class T>
UniquePtr<T>& UniquePtr<T>::operator=(UniquePtr<T>& rhs) {

	  // ensure that the memory this pointed to
	  // is freed
	clear();

	  // impossible for them both to be pointing
	  // to the same thing?
	if (this != &rhs) {
		m_pointee = rhs.m_pointee;        // point to the other one's memory
		m_deleteFunc = rhs.m_deleteFunc;  // learn how to delete it
	}

	  // rhs needs to forget
	rhs.m_pointee = nullptr;
	rhs.m_deleteFunc = nullptr;

	return *this;
}


template<class T>
void UniquePtr<T>::swap(UniquePtr<T>& other) {
	if (this != &other) {
		T* thisP = m_pointee;
		void (*thisD) () = m_deleteFunc;

		m_pointee = other.m_pointee;
		m_deleteFunc = other.m_deleteFunc;

		other.m_pointee = thisP;
		other.m_deleteFunc = thisD;
	}
}

  // don't ever need to reset the deleter function
  // because you always delete an object of type T
  // the same way. This limits functionality a little
  // but serves the lab pretty well.
template<class T>
void UniquePtr<T>::reset(T* objPtr) {

	  // delete what's currently being pointed to
	m_deleteFunc(m_pointee);

	  // then manage the new object
	m_pointee = objPtr;
}


  // get the value in the memory pointed to
  // return a reference so that you can change it
template<class T>
T& UniquePtr<T>::operator*() const {
	return *m_pointee;
}


  // get back a pointer to the memory so that you
  // can call member functions if it's a class
template<class T>
T* UniquePtr<T>::operator->() const {
	return m_pointee;
}


  // access the element stored some offset away
  // from the pointer. If the memory allocated
  // isn't an array, it does the same thing but
  // anything could be stored there - same as raw
  // pointers.
template<class T>
T& UniquePtr<T>::operator[](int i) const {
	return m_pointee[i];
}


  // get the memory location some offset away from
  // what the UniquePtr points to
template<class T>
T* UniquePtr<T>::operator+(int offset) const {
	return m_pointee + offset;
}


template<class T>
UniquePtr<T>::operator bool() const {
	return m_pointee != nullptr;
}


  // Function that enforces the requirement that only one
  // UniquePtr can point to an allocated object at a time.
  // Whenever a UniquePtr is re-assigned, need to clear the
  // memory that it has.
template<class T>
void UniquePtr<T>::clear() {

	  // free the memory if it's allocated
	if (m_pointee != nullptr && m_deleteFunc != nullptr) {
		m_deleteFunc(m_pointee);  // free the memory
		m_pointee = nullptr;      // remember that it's freed
		m_deleteFunc = nullptr;   // forget the delete function
	}
}
