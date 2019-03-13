
#ifndef PTR_H
#define PTR_H

template<class> class RefCountedPtrImpl;

template<class T>
class CleanlyPtr {
public:
	// return type, *funcName, argument type(s)
	using AllocFunc = T* (*) ();
	using FreeFunc =  void (*) (T*);

	CleanlyPtr(AllocFunc aFunc, FreeFunc fFunc);
	~CleanlyPtr();

	int getNumRefs() const { return pImpl->getNumRefs(); }
private:
	RefCountedPtrImpl<T>* pImpl;
};

#endif


template<class T>
class RefCountedPtrImpl {
public:
	using AllocFunc = typename CleanlyPtr<T>::AllocFunc;
	using FreeFunc =  typename CleanlyPtr<T>::FreeFunc;

	RefCountedPtrImpl(AllocFunc a, FreeFunc f);
	~RefCountedPtrImpl();

	int getNumRefs() const { return *numReferences; }

private:
	FreeFunc deleteFunc;
	AllocFunc newFunc;
	T* ptr;
	int* numReferences;
};

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





////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Past Implementation ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
template<class T>
CleanlyPtr<T>::CleanlyPtr(AllocFunc aFunc, FreeFunc fFunc) {
	pImpl = new RefCountedPtrImpl<T>(aFunc, fFunc);
}

template<class T>
CleanlyPtr<T>::~CleanlyPtr() {
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
// class CleanlyPtr : public BasePtr {
// public:
// 	CleanlyPtr() { }
// 	~CleanlyPtr() { }

// 	T& getObject() const { return *static_cast<T*>(BasePtr::getObject()); }
// 	void pointTo(T& obj) { BasePtr::pointTo(static_cast<void*>(obj)); }

// private:
// };
