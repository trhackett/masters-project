
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
	LeaklessPtr(AllocFunc aFunc, FreeFunc fFunc);

	LeaklessPtr(LeaklessPtr<T,Impl>& other);
	LeaklessPtr<T,Impl>& operator=(LeaklessPtr<T,Impl>& rhs);
	LeaklessPtr(LeaklessPtr<T,Impl>&& mover);

	// // variadic template
	// template<class... Args>
	// LeaklessPtr(T* (*a) (Args...), FreeFunc f, Args... args)
	//  : pImpl(new Impl<T>(a, f, args...))
	// { }

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

	T* operator+(int offset) const;
	T* operator-(int offset) const;

private:
	BasePtrImpl<T>* pImpl;
};


template<class T>
class BasePtrImpl {

public:
	using AllocFunc = typename LeaklessPtr<T,BasePtrImpl>::AllocFunc;
	using FreeFunc  = typename LeaklessPtr<T,BasePtrImpl>::FreeFunc;

	BasePtrImpl(AllocFunc a, FreeFunc f);
	BasePtrImpl(BasePtrImpl<T>&& mover);
	BasePtrImpl(T* movedPtr, AllocFunc a, FreeFunc f);

	BasePtrImpl(BasePtrImpl<T>& other);

	// template<class... Args>
	// //          pointer to a function
	// //          that takes in a
	// //          variable number of
	// //          args
	// BasePtrImpl(T* (*a) (Args...), FreeFunc f, Args... args)
	//  : allocFunc(nullptr), freeFunc(f), ptr(a(args...))
	// { }

	T& operator*() const;
	T* operator->() const;
	T& operator[](int offset) const;
	bool operator==(const T* other) const;
	bool operator!=(const T* other) const;
	bool operator==(const BasePtrImpl<T>& other) const;
	bool operator!=(const BasePtrImpl<T>& other) const;
	T* operator+(int offset) const;
	T* operator-(int offset) const;

	virtual ~BasePtrImpl() = 0;
	virtual BasePtrImpl<T>& operator=(BasePtrImpl<T>& rhs) = 0;

protected:
	AllocFunc allocFunc;
	FreeFunc freeFunc;
	T* ptr;

	void freeDataIfThere();
	virtual void removeReference() = 0;
};


template<class T>
class SelfishPtrImpl : public BasePtrImpl<T> {

public:
	using AllocFunc = typename BasePtrImpl<T>::AllocFunc;
	using FreeFunc = typename BasePtrImpl<T>::FreeFunc;

	SelfishPtrImpl(AllocFunc a, FreeFunc f);
	SelfishPtrImpl(SelfishPtrImpl<T>&& mover);
	~SelfishPtrImpl();

	// template<class... Args>
	// SelfishPtrImpl(T* (*a) (Args...), FreeFunc f, Args... args)
	//  : BasePtrImpl<T>(a, f, args...)
	// { }

	// steal 
	SelfishPtrImpl(SelfishPtrImpl<T>& other);
	BasePtrImpl<T>& operator=(BasePtrImpl<T>& rhs) override;

private:
	// doesn't have reference count
	void removeReference() {}
	void incRefsf() {}
	void decRefs()  {}
};

template<class T>
class AltruisticPtrImpl : public BasePtrImpl<T> {

public:
	using AllocFunc = typename BasePtrImpl<T>::AllocFunc;
	using FreeFunc = typename BasePtrImpl<T>::FreeFunc;

	AltruisticPtrImpl(AllocFunc a, FreeFunc f);
	AltruisticPtrImpl(AltruisticPtrImpl<T>&& mover);
	~AltruisticPtrImpl();

	AltruisticPtrImpl(AltruisticPtrImpl<T>& other);
	BasePtrImpl<T>& operator=(BasePtrImpl<T>& rhs) override;

private:
	int* numReferences;
	void removeReference();
	void decRefs() { (*numReferences)--; }
	void incRefs() { (*numReferences)++; }
};

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
	mover.allocFunc = nullptr;
	mover.freeFunc = nullptr;
}

// only derived classes handle this - basically virtual
template<class T>
BasePtrImpl<T>::BasePtrImpl(BasePtrImpl<T>& other) { }

template<class T>
BasePtrImpl<T>::BasePtrImpl(T* movedPtr, AllocFunc a, FreeFunc f)
 : allocFunc(a), freeFunc(f), ptr(movedPtr)
{

}

// template<class T, class... Args>
// BasePtrImpl<T>::BasePtrImpl<Args>(AllocFunc a, FreeFunc f, Args... args)
//  : aFunc(a), fFunc(f), ptr(aFunc(args...))
// {

// }

template<class T>
void BasePtrImpl<T>::freeDataIfThere()
{
	if (freeFunc != nullptr && ptr != nullptr) {
		freeFunc(ptr);
	}
}

template<class T> T& BasePtrImpl<T>::operator*() const
	{ return *ptr; }
template<class T> T* BasePtrImpl<T>::operator->() const
	{ return ptr; }
template<class T> T& BasePtrImpl<T>::operator[](int offset) const
	{ return *(ptr + offset); }
template<class T> bool BasePtrImpl<T>::operator==(const T* other) const
	{ return this->ptr == other; }
template<class T> bool BasePtrImpl<T>::operator!=(const T* other) const
	{ return this->ptr != other; }
template<class T> bool BasePtrImpl<T>::operator==(const BasePtrImpl<T>& other) const
	{ return this->ptr == other.ptr; }
template<class T> bool BasePtrImpl<T>::operator!=(const BasePtrImpl<T>& other) const
	{ return this->ptr != other.ptr; }
template<class T> T* BasePtrImpl<T>::operator+(int offset) const
	{ return ptr+offset; }
template<class T> T* BasePtrImpl<T>::operator-(int offset) const
	{ return ptr-offset; }



//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////// SelfishPtrImpl ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


template<class T>
SelfishPtrImpl<T>::SelfishPtrImpl(AllocFunc a, FreeFunc f)
 : BasePtrImpl<T>(a, f)
{

}


template<class T>
SelfishPtrImpl<T>::SelfishPtrImpl(SelfishPtrImpl<T>&& mover)
 : BasePtrImpl<T>(std::move(mover))
{

}

// template<class T, class... Args>
// SelfishPtrImpl<T>::SelfishPtrImpl<Args>(AllocFunc a, FreeFunc f, Args... args)
//  : BasePtrImpl(a, f, args...)
// {

// }

template<class T>
SelfishPtrImpl<T>::SelfishPtrImpl(SelfishPtrImpl<T>& other)
 : BasePtrImpl<T>(other.ptr, other.allocFunc, other.freeFunc)
{
	other.ptr = nullptr;
	other.allocFunc = nullptr;
	other.freeFunc = nullptr;
}

template<class T>
BasePtrImpl<T>& SelfishPtrImpl<T>::operator=(BasePtrImpl<T>& rhs)
{
	// must be selfish 
	SelfishPtrImpl<T>& srhs = static_cast<SelfishPtrImpl<T>&>(rhs);

	if (this != &srhs) {
		this->freeDataIfThere();
		this->ptr = srhs.ptr;
		this->allocFunc = srhs.allocFunc;
		this->freeFunc = srhs.freeFunc;

		srhs.ptr = nullptr;
		srhs.allocFunc = nullptr;
		srhs.freeFunc = nullptr;
	}

	return *this;
}


template<class T>
SelfishPtrImpl<T>::~SelfishPtrImpl() {
	// always free memory since I'm the only reference
	this->freeDataIfThere();
}



//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// AltruisticPtrImpl ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
AltruisticPtrImpl<T>::AltruisticPtrImpl(AllocFunc a, FreeFunc f)
 : BasePtrImpl<T>(a, f)
{
	numReferences = new int(1);
}

template<class T>
AltruisticPtrImpl<T>::AltruisticPtrImpl(AltruisticPtrImpl<T>&& mover)
 : BasePtrImpl<T>(std::move(mover)),
   numReferences(mover.numReferences)
{
	mover.numReferences = nullptr;
}

template<class T>
AltruisticPtrImpl<T>::~AltruisticPtrImpl() {
	removeReference();
}

template<class T>
AltruisticPtrImpl<T>::AltruisticPtrImpl(AltruisticPtrImpl<T>& other)
 : BasePtrImpl<T>(other.ptr, other.allocFunc, other.freeFunc),
   numReferences(other.numReferences)
{
	// you now share a reference, so increment it
	incRefs();
}

template<class T>
BasePtrImpl<T>& AltruisticPtrImpl<T>::operator=(BasePtrImpl<T>& rhs)
{
	// must be altruistic 
	AltruisticPtrImpl<T>& arhs = static_cast<AltruisticPtrImpl<T>&>(rhs);

	if (this != &arhs) {
		removeReference();
		this->numReferences = arhs.numReferences;
		this->ptr = arhs.ptr;
		this->allocFunc = arhs.allocFunc;
		this->freeFunc = arhs.freeFunc;
		incRefs();
	}
	
	return *this;
}

template<class T>
void AltruisticPtrImpl<T>::removeReference() {
	// free memory if i'm the last reference
	if (numReferences != nullptr) {
		decRefs();
		if (*numReferences == 0) {
			delete numReferences;
			this->freeDataIfThere();
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// LeaklessPtr /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

template<class T, template<class> class Impl>
LeaklessPtr<T,Impl>::LeaklessPtr(AllocFunc aFunc, FreeFunc fFunc)
 : pImpl(new Impl<T>(aFunc, fFunc))
{ }

template<class T, template<class> class Impl>
LeaklessPtr<T,Impl>::LeaklessPtr(LeaklessPtr<T,Impl>& other) 
 : pImpl(new Impl<T>(*static_cast<Impl<T>*>(other.pImpl)))
{ }

// template<class T, template<class>class Impl, class... Args>
// LeaklessPtr<T,Impl>::LeaklessPtr(AllocFunc a, FreeFunc f, Args... args)
//  : pImpl(new Impl<T>(a, f, args...))
// {

// }

template<class T, template<class> class Impl>
LeaklessPtr<T,Impl>& LeaklessPtr<T,Impl>::operator=(LeaklessPtr<T,Impl>& rhs)
{
	if (this != &rhs) {
		// cast to the derived class to call the derived assignment operator
		*pImpl = *static_cast<Impl<T>*>(rhs.pImpl);
	}
	return *this;
}

template<class T, template<class> class Impl>
LeaklessPtr<T,Impl>::LeaklessPtr(LeaklessPtr<T,Impl>&& mover)
 : pImpl(new Impl<T>(std::move(*static_cast<Impl<T>*>(mover.pImpl))))
{

}

template<class T, template<class> class Impl>
LeaklessPtr<T,Impl>::~LeaklessPtr()
{
	delete pImpl;
}

template<class T, template<class> class Impl>
T& LeaklessPtr<T,Impl>::operator*() const { return pImpl->operator*(); }

template<class T, template<class> class Impl>
T* LeaklessPtr<T,Impl>::operator->() const { return pImpl->operator->(); }

template<class T, template<class> class Impl>
T& LeaklessPtr<T,Impl>::operator[](int offset) const { return pImpl->operator[](offset); }

template<class T, template<class> class Impl>
bool LeaklessPtr<T,Impl>::operator==(const T* other) const {
	return pImpl->operator==(other);
}

template<class T, template<class> class Impl>
bool LeaklessPtr<T,Impl>::operator!=(const T* other) const {
	return pImpl->operator!=(other);
}

template<class T, template<class> class Impl>
bool LeaklessPtr<T,Impl>::operator==(const LeaklessPtr<T,Impl>& other) const {
	return pImpl->operator==(*other.pImpl);
}

template<class T, template<class> class Impl>
bool LeaklessPtr<T,Impl>::operator!=(const LeaklessPtr<T,Impl>& other) const {
	return pImpl->operator!=(*other.pImpl);
}

template<class T, template<class> class Impl>
T* LeaklessPtr<T,Impl>::operator+(int offset) const { return pImpl->operator+(offset); }

template<class T, template<class> class Impl>
T* LeaklessPtr<T,Impl>::operator-(int offset) const { return pImpl->operator-(offset); }

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