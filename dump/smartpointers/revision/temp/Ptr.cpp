
#include "Ptr.h"

template<class T>
class RefCountedPtrImpl {
public:
	using AllocFunc = typename CleanlyPtr<T>::AllocFunc;
	using FreeFunc =  typename CleanlyPtr<T>::FreeFunc;

	RefCountedPtrImpl(AllocFunc a, FreeFunc f);
	~RefCountedPtrImpl();

private:
	T* ptr;
	AllocFunc aFunc;
	FreeFunc fFunc;
};

template<class T>
RefCountedPtrImpl<T>::RefCountedPtrImpl(AllocFunc aF, FreeFunc fF)
 : aFunc(aF), fFunc(fF), ptr(aFunc())
{

}

template<class T>
RefCountedPtrImpl<T>::~RefCountedPtrImpl()
{
	fFunc(ptr);
}




template<class T>
CleanlyPtr<T>::CleanlyPtr(AllocFunc aFunc, FreeFunc fFunc) {
	pImpl = new RefCountedPtrImpl<T>(aFunc, fFunc);
}

template<class T>
CleanlyPtr<T>::~CleanlyPtr() {
	delete pImpl;
}

