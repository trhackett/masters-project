
#ifndef LEAKLESSPTR_H
#define LEAKLESSPTR_H

#include <utility>
#include <iostream>
using namespace std;

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
	bool operator==(const T* other) const;
	bool operator!=(const T* other) const;
	bool operator==(const LeaklessPtr<T,Impl>& other) const;
	bool operator!=(const LeaklessPtr<T,Impl>& other) const;

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
	BasePtrImpl(T* movedPtr, AllocFunc a, FreeFunc f);

	T& operator*() const;
	T* operator->() const;
	T& operator[](int offset) const;
	bool operator==(const T* other) const;
	bool operator!=(const T* other) const;
	bool operator==(const BasePtrImpl<T>& other) const;
	bool operator!=(const BasePtrImpl<T>& other) const;

	virtual ~BasePtrImpl() = 0;
	BasePtrImpl(BasePtrImpl<T>& other) = delete;
	BasePtrImpl<T>& operator=(BasePtrImpl<T>& rhs) = delete;
};


template<class T>
struct SelfishPtrImpl : public BasePtrImpl<T> {

	using AllocFunc = typename BasePtrImpl<T>::AllocFunc;
	using FreeFunc = typename BasePtrImpl<T>::FreeFunc;

	SelfishPtrImpl(AllocFunc a, FreeFunc f);
	SelfishPtrImpl(SelfishPtrImpl<T>&& mover);
	~SelfishPtrImpl();

	// can't copy Singleton pointers
	SelfishPtrImpl(SelfishPtrImpl<T>& other) = delete;
	SelfishPtrImpl<T>& operator=(SelfishPtrImpl<T>& rhs) = delete;
};

/*template<class T>
struct AltruisticPtrImpl : public BasePtrImpl<T> {

	using AllocFunc = typename BasePtrImpl<T>::AllocFunc;
	using FreeFunc = typename BasePtrImpl<T>::FreeFunc;

	AltruisticPtrImpl(AllocFunc a, FreeFunc f);
	AltruisticPtrImpl(AltruisticPtrImpl<T>&& mover);
	~AltruisticPtrImpl();

	AltruisticPtrImpl(AltruisticPtrImpl<T>& other);
	AltruisticPtrImpl<T>& operator=(AltruisticPtrImpl<T>& rhs);
};*/

#endif



//////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// BasePtrImpl ////////////////////////////////////////////
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
BasePtrImpl<T>::BasePtrImpl(T* movedPtr, AllocFunc a, FreeFunc f)
 : allocFunc(a), freeFunc(f), ptr(movedPtr)
{

}

template<class T>
T& BasePtrImpl<T>::operator*() const { return *ptr; }

template<class T>
T* BasePtrImpl<T>::operator->() const { return ptr; }

template<class T>
T& BasePtrImpl<T>::operator[](int offset) const { return *(ptr + offset); }

template<class T>
bool BasePtrImpl<T>::operator==(const T* other) const { return this->ptr == other; }

template<class T>
bool BasePtrImpl<T>::operator!=(const T* other) const { return this->ptr != other; }

template<class T>
bool BasePtrImpl<T>::operator==(const BasePtrImpl<T>& other) const {
	return this->ptr == other.ptr;
}

template<class T>
bool BasePtrImpl<T>::operator!=(const BasePtrImpl<T>& other) const {
	return this->ptr != other.ptr;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// SingletonPtr ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


template<class T>
SelfishPtrImpl<T>::SelfishPtrImpl(AllocFunc a, FreeFunc f)
 : BasePtrImpl<T>(a, f)
{

}


template<class T>
SelfishPtrImpl<T>::SelfishPtrImpl(SelfishPtrImpl<T>&& mover)
 : BasePtrImpl<T>(mover.ptr, mover.allocFunc, mover.freeFunc)
{
	// too selfish to share the ptr
	mover.ptr = nullptr;
	mover.freeFunc = nullptr;
	mover.allocFunc = nullptr;
}


template<class T>
SelfishPtrImpl<T>::~SelfishPtrImpl() {
	// I'm the only one that knows about this object, so delete it
	BasePtrImpl<T>::freeFunc(BasePtrImpl<T>::ptr);
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
 : pImpl(new Impl<T>(std::move(static_cast<Impl<T>&&>(*mover.pImpl))))
{
	mover.pImpl = nullptr; // mover stops existing, don't delete
}

template<class T, template<class> class Impl>
LeaklessPtr<T,Impl>::~LeaklessPtr() {
	if (pImpl != nullptr) {
		delete pImpl;
	}
}

template<class T, template<class> class Impl>
T& LeaklessPtr<T,Impl>::operator*() const { return pImpl->operator*(); }

template<class T, template<class> class Impl>
T* LeaklessPtr<T,Impl>::operator->() const { return pImpl->operator->(); }

template<class T, template<class> class Impl>
T& LeaklessPtr<T,Impl>::operator[](int offset) const { return pImpl->operator[](offset); }

template<class T, template<class> class Impl>
bool LeaklessPtr<T,Impl>::operator==(const T* other) const {
	return pImpl->operator==(other->pImpl);
}

template<class T, template<class> class Impl>
bool LeaklessPtr<T,Impl>::operator!=(const T* other) const {
	return pImpl->operator!=(other->pImpl);
}

template<class T, template<class> class Impl>
bool LeaklessPtr<T,Impl>::operator==(const LeaklessPtr<T,Impl>& other) const {
	return pImpl->operator==(*other.pImpl);
}

template<class T, template<class> class Impl>
bool LeaklessPtr<T,Impl>::operator!=(const LeaklessPtr<T,Impl>& other) const {
	return pImpl->operator!=(*other.pImpl);
}

// dummy destructor so that we can't initialize classes
// of type BasePtrImpl
template<class T>
BasePtrImpl<T>::~BasePtrImpl() { }













//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// Garbage Bin /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

// template<class T>
// SelfishPtrImpl<T>::SelfishPtrImpl(SelfishPtrImpl<T>& other)
//  : BasePtrImpl<T>::allocFunc(other.allocFunc), BasePtrImpl<T>::freeFunc(other.freeFunc),
//    BasePtrImpl<T>::ptr(other.ptr)
// {
// 	other.ptr = nullptr;
// }


// template<class T>
// SelfishPtrImpl<T>& SelfishPtrImpl<T>::operator=(SelfishPtrImpl<T>& rhs)
// {
// 	if (this != &rhs) {
// 		BasePtrImpl<T>::allocFunc = rhs.allocFunc;
// 		BasePtrImpl<T>::freeFunc = rhs.freeFunc;
// 		BasePtrImpl<T>::ptr = rhs.ptr;
// 	}

// 	return *this;
// }