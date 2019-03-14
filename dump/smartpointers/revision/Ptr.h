
#ifndef PTR_H
#define PTR_H

template<class> class RefCountedPtrImpl;

template<class T>
class LeakLessPtr {
public:
	// return type, *funcName, argument type(s)
	using AllocFunc = T* (*) ();
	using FreeFunc =  void (*) (T*);

	LeakLessPtr(AllocFunc aFunc, FreeFunc fFunc);
	~LeakLessPtr();

	T& operator*() const { return pImpl->operator*(); }
	bool operator==(const T* other) const { return pImpl->operator==(other); }
	bool operator!=(const T* other) const { return pImpl->operator!=(other); }
	bool operator==(const LeakLessPtr<T>& other) const { return pImpl->operator==(other.pImpl); }
	bool operator!=(const LeakLessPtr<T>& other) const { return pImpl->operator!=(other.pImpl); }

private:
	RefCountedPtrImpl<T>* pImpl;
};

template<class T>
class RefCountedPtrImpl {
public:
	using AllocFunc = typename LeakLessPtr<T>::AllocFunc;
	using FreeFunc =  typename LeakLessPtr<T>::FreeFunc;

	RefCountedPtrImpl(AllocFunc a, FreeFunc f);
	~RefCountedPtrImpl();

	T& operator*() const;
	bool operator==(const T* other) const;
	bool operator!=(const T* other) const;
	bool operator==(const RefCountedPtrImpl<T>* other) const;
	bool operator!=(const RefCountedPtrImpl<T>* other) const;

private:
	FreeFunc deleteFunc;
	AllocFunc newFunc;
	T* const ptr;
	int* numReferences;
};

#endif



template<class T>
RefCountedPtrImpl<T>::RefCountedPtrImpl(AllocFunc nFunc, FreeFunc dFunc)
 : deleteFunc(dFunc), newFunc(nFunc), ptr(newFunc()), numReferences(new int(1))
{

}

template<class T>
RefCountedPtrImpl<T>::~RefCountedPtrImpl()
{
	// if I'm the last reference, delete
	if (*numReferences == 1) {
		delete numReferences;
		deleteFunc(ptr);
	}
}

template<class T>
T& RefCountedPtrImpl<T>::operator*() const {
	return *ptr;
}

template<class T>
bool RefCountedPtrImpl<T>::operator==(const T* other) const {
	return ptr == other;
}

template<class T>
bool RefCountedPtrImpl<T>::operator!=(const T* other) const {
	return ptr != other;
}

template<class T>
bool RefCountedPtrImpl<T>::operator==(const RefCountedPtrImpl<T>* other) const {
	return ptr == other->ptr;
}

template<class T>
bool RefCountedPtrImpl<T>::operator!=(const RefCountedPtrImpl<T>* other) const {
	return ptr != other->ptr;
}





////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Past Implementation ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
LeakLessPtr<T>::LeakLessPtr(AllocFunc aFunc, FreeFunc fFunc) {
	pImpl = new RefCountedPtrImpl<T>(aFunc, fFunc);
}

template<class T>
LeakLessPtr<T>::~LeakLessPtr() {
	delete pImpl;
}




// class BasePtr {
// public:
// 	BasePtr() {}
// 	~BasePtr() {}

// 	void* getObject() const { return ptr; }
// 	virtual void pointTo(void* obj) { ptr = obj; }

// private:
// 	void* ptr;
// };


// template<class T>
// class LeakLessPtr : public BasePtr {
// public:
// 	LeakLessPtr() { }
// 	~LeakLessPtr() { }

// 	T& getObject() const { return *static_cast<T*>(BasePtr::getObject()); }
// 	void pointTo(T& obj) { BasePtr::pointTo(static_cast<void*>(obj)); }

// private:
// };
