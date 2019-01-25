

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
		return *m_pointee;
	}

	T* operator->() const {
		return m_pointee;
	}

	T& operator[](int offset) const {
		return m_pointee[offset];
	}
	T* operator+(int offset) const {
		return m_pointee + offset;
	}
	T* operator-(int offset) const {
		return m_pointee - offset;
	}


	  // are they pointing to the same thing?
	bool operator==(const T* rhs) const {
		return m_pointee == rhs;
	}

	bool operator!=(const T* rhs) const {
		return m_pointee != rhs;
	}

	  // are they the same smart ptr? - I think actually
	  // this should also answer whether or not they are pointing
	  // to the same thing.
	bool operator==(const BasePtr<T>& rhs) const {
		return m_pointee == rhs.m_pointee;
	}

	bool operator!=(const BasePtr<T>& rhs) const {
		return m_pointee != rhs.m_pointee;
	}

	bool operator<(const BasePtr<T>& rhs) const {
		return m_pointee < rhs.m_pointee;
	}

	void swap(BasePtr<T>&);

	  // important virtual functions
	virtual ~BasePtr() = 0;
	virtual void reset(T*, void (*) (T*)) = 0;
	virtual int getNumReferences() const = 0;

protected:
	  // pointer to the allocated object
	T* m_pointee;
};


template<class T>
BasePtr<T>::BasePtr()
 : m_pointee(nullptr) 
{ }

  // track the allocated object and know how to delete it
template<class T>
BasePtr<T>::BasePtr(T* objptr)
 : m_pointee(objptr)
{ }

  // dummy destructor since we need it
  template<class T>
BasePtr<T>::~BasePtr()
{ }


#endif
