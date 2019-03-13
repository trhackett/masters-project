
#ifndef LEAKLESSPTR_H
#define LEAKLESSPTR_H

#include <utility>

template<class> class BasePtrImpl;

template<class T, template<class> class Impl>
class LeaklessPtr {
public:
	using AllocFunc = T* (*) ();
	using FreeFunc = void (*) (T*);

	// main constructor takes in an allocation function
	// and a freeing function
	LeaklessPtr(T* (*aFunc) (), void (*fFunc) (T*));

	LeaklessPtr(LeaklessPtr<T,Impl>& other);
	LeaklessPtr<T,Impl>& operator=(LeaklessPtr<T,Impl>& rhs);
	LeaklessPtr(LeaklessPtr<T,Impl>&& mover);

	// destructor has to free memory
	~LeaklessPtr();

	// regular pointer things
	T& operator*() const;
	T* operator->() const;
	T& operator[](int offset) const;

private:
	BasePtrImpl<T>* pImpl;
};


template<class T>
struct BasePtrImpl {

	using AllocFunc = typename LeaklessPtr<T,BasePtrImpl>::AllocFunc;
	using FreeFunc  = typename LeaklessPtr<T,BasePtrImpl>::FreeFunc;

	AllocFunc allocFunc;
	FreeFunc freeFunc;
	T* ptr;

	BasePtrImpl(AllocFunc a, FreeFunc f);
	BasePtrImpl(BasePtrImpl<T>&& mover);

	T& operator*() const;
	T* operator->() const;
	T& operator[](int offset) const;

	virtual ~BasePtrImpl() = 0;
	BasePtrImpl(BasePtrImpl<T>& other) = 0;
	BasePtrImpl<T>& operator=(BasePtrImpl<T>& rhs) = 0;
};


template<class T>
struct SingeltonPtrImpl : public BasePtrImpl<T> {

	SingeltonPtrImpl(AllocFunc a, FreeFunc f);
	SingeltonPtrImpl(SingeltonPtrImpl<T>& other);
	SingeltonPtrImpl<T>& operator=(SingeltonPtrImpl<T>& rhs);
	SingeltonPtrImpl(SingeltonPtrImpl<T>&& mover);
	~SingeltonPtrImpl();

};

#endif



//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// BasePtr ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


template<class T>
BasePtrImpl<T>::BasePtrImpl(AllocFunc a, FreeFunc f)
 : allocFunc(a), freeFunc(f), ptr(allocFunc())
{

}

template<class T>
BasePtrImpl<T>::BasePtrImpl(BasePtrImpl<T>&& mover)
 : allocFunc(mover.allocFunc), freeFunc(mover.freeFunc), ptr(mover.ptr)
{
	mover.ptr = nullptr;
}

template<class T>
T& BasePtrImpl<T>::operator*() const {
	return *ptr;
}

template<class T>
T* BasePtrImpl<T>::operator->() const {
	return ptr;
}

template<class T>
T& BasePtrImpl<T>::operator[](int offset) const {
	return *(ptr + offset);
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// SingletonPtr ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


template<class T>
SingeltonPtrImpl<T>::SingeltonPtrImpl(AllocFunc a, FreeFunc f)
 : BasePtr<T>::allocFunc(a), BasePtr<T>::freeFunc(f), BasePtr<T>::ptr(allocFunc())
{

}


template<class T>
SingeltonPtrImpl<T>::SingeltonPtrImpl(SingeltonPtrImpl<T>& other)
 : allocFunc(other.allocFunc), freeFunc(other.freeFunc), ptr(other.ptr)
{
	other.ptr = nullptr;
}


template<class T>
SingeltonPtrImpl<T>& SingeltonPtrImpl<T>::operator=(SingeltonPtrImpl<T>& rhs)
{
	if (this != &rhs) {
		allocFunc = rhs.allocFunc;
		freeFunc = rhs.freeFunc;
		ptr = rhs.ptr;
	}

	return *this;
}


template<class T>
SingeltonPtrImpl<T>::SingeltonPtrImpl(SingeltonPtrImpl<T>&& mover)
 : allocFunc(mover.allocFunc), freeFunc(mover.freeFunc), ptr(mover.ptr)
{
	mover.ptr = nullptr;
}


template<class T>
SingeltonPtrImpl<T>::~SingeltonPtrImpl() {
	freeFunc(ptr);
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// LeaklessPtr /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

template<class T, template<class> class Impl>
LeaklessPtr<T,Impl>::LeaklessPtr(T* (*aFunc) (), void (*fFunc) (T*))
 : pImpl(new Impl<T>(aFunc, fFunc))
{ }

template<class T, template<class> class Impl>
LeaklessPtr<T,Impl>::LeaklessPtr(LeaklessPtr<T,Impl>& other) 
 : pImpl(new Impl<T>(*other.pImpl))
{ }

template<class T, template<class> class Impl>
LeaklessPtr<T,Impl>& LeaklessPtr<T,Impl>::operator=(LeaklessPtr<T,Impl>& rhs) {
	if (this != &rhs) {
		*pImpl = *rhs.pImpl;
	}
	return *this;
}

template<class T, template<class> class Impl>
LeaklessPtr<T,Impl>::LeaklessPtr(LeaklessPtr<T,Impl>&& mover)
 : pImpl(new Impl<T>(std::move(mover.pImpl)))
{
	mover.pImpl = nullptr; // mover no longer exists
}

template<class T, template<class> class Impl>
LeaklessPtr<T,Impl>::~LeaklessPtr() {
	if (pImpl != nullptr) {
		delete pImpl;
	}
}

template<class T, template<class> class Impl>
T& LeaklessPtr<T,Impl>::operator*() const {
	return pImpl->operator*();
}

template<class T, template<class> class Impl>
T* LeaklessPtr<T,Impl>::operator->() const {
	return pImpl->operator->();
}

template<class T, template<class> class Impl>
T& LeaklessPtr<T,Impl>::operator[](int offset) const {
	return pImpl->operator[](offset);
}