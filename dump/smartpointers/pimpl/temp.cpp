
  // templated class that stores something of type Object and
  // a templated class of type Impl<>
  // https://en.cppreference.com/w/cpp/language/template_parameters !!!
template<class Object, template<class> class Impl>
class SmartPtr {
public:
	SmartPtr() {}
	  // main constructor takes in a pointer to a function
	  // for allocated memory and a function for deleting
	  // that memory
	SmartPtr(Object* (*) (), void (*) (Object*)) {}

	  // destructor calls the Impl's destructor
	~SmartPtr() {}

private:
	  // store the pointer to the allocated memory
	Object* m_pointee;
	  // for now, store the pointer to the allocation function
	  // but I don't think you're going to use it
	Object* (*m_newFunc) ();
	  // store the pointer to the free function - returns void
	  // and takes as input a pointer to the stores object
	void (*m_deleteFunc) (Object*);
	  // store the pointer to the Impl
	Impl<Object>* m_implPtr;
};

template<class T>
class templateClass {

};


int main() {
	SmartPtr<int, templateClass> sptr;
}