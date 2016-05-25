#include "vector"
#ifdef __CINT__ 
#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;
#pragma link C++ class vector<T>+;
#pragma link C++ class vector<T>::*;
#ifdef G__VECTOR_HAS_CLASS_ITERATOR
#pragma link C++ operators vector<T>::iterator;
#pragma link C++ operators vector<T>::const_iterator;
#pragma link C++ operators vector<T>::reverse_iterator;
#endif
#endif
