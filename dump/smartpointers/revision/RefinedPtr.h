
#ifndef REFINEDPTR_H
#define REFINEDPTR_H

struct Ptr
{
	virtual void accessData() = 0;
};

template<class T>
class RefinedPtr : public Ptr
{
public:
	RefinedPtr() {}
	void accessData() { }

private:
	class PtrImpl;
	PtrImpl* m_Impl;

	T* m_ptr;
};

template<class T>
class RefinedPtr<T>::PtrImpl
{

};


#endif