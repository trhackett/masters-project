
#ifndef PRUDENTPTR_H
#define PRUDENTPTR_H

#include <memory>
using namespace std;

class PrudentPtr_base
{

};

template<class T>
class PrudentPtr : public PrudentPtr_base
{
public:
	PrudentPtr() {}
private:
	class PtrBaseImpl;   // forward declaration
	PtrBaseImpl* m_Impl; // pointer to implementation
};


class PrudentPtr::PtrBaseImpl
{

};

template<class T>
class SingletonPtr : public PrudentPtr::PtrBaseImpl
{

};

#endif