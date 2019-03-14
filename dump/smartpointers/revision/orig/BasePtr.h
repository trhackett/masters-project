

#ifndef BASE_PTR_IMPL
#define BASE_PTR_IMPL

template<class T>
class BasePtr
{
public:
	BasePtr();
	BasePtr(T*);

	  // normal accessing and pointer arithmetic is
	  // the same no matter what type of SmartPtr it
	  // is, implementation independent
	T& operator*() const {
		return *basePointee;
	}

	T* operator->() const {
		return basePointee;
	}

	T& operator[](int offset) const {
		return basePointee[offset];
	}
	T* operator+(int offset) const {
		return basePointee + offset;
	}
	T* operator-(int offset) const {
		return basePointee - offset;
	}


	  // are they pointing to the same thing?
	bool operator==(const T* rhs) const {
		return basePointee == rhs;
	}

	bool operator!=(const T* rhs) const {
		return basePointee != rhs;
	}

	  // are they the same smart ptr? - I think actually
	  // this should also answer whether or not they are pointing
	  // to the same thing.
	bool operator==(const BasePtr<T>& rhs) const {
		return basePointee == rhs.basePointee;
	}

	bool operator!=(const BasePtr<T>& rhs) const {
		return basePointee != rhs.basePointee;
	}

	bool operator<(const BasePtr<T>& rhs) const {
		return basePointee < rhs.basePointee;
	}

	void swap(BasePtr<T>&);

	  // important virtual functions
	virtual ~BasePtr() = 0;
	virtual void reset(T*, void (*) (T*)) = 0;
	virtual int getNumReferences() const = 0;

protected:
	  // pointer to the allocated object
	T* basePointee;
};


template<class T>
BasePtr<T>::BasePtr()
 : basePointee(nullptr) 
{ }

  // track the allocated object and know how to delete it
template<class T>
BasePtr<T>::BasePtr(T* objptr)
 : basePointee(objptr)
{ }

  // dummy destructor since we need it
  template<class T>
BasePtr<T>::~BasePtr()
{ }


#endif
