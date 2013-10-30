/*******************************************************************************

ScopedPtr.hpp

*******************************************************************************/

#ifndef __ScopedPtr_HPP
#define	__ScopedPtr_HPP
//------------------------------------------------------------------------------

namespace Common
{

//! Lightweight and simple pointer for sole ownership of single objects.
/**
	The scoped pointer stares a pointer to a dynamically allocated object, which
	is reclaimed either on destuction of the smart pointer or via explicit reset().
	It has no shared ownership or ownership tranfer semantics and is non-copyable.
	The scoped pointer is safer and faster for objects that should not be copied.
	The scoped pointer does not meet the CopyConstructible and Assignable
	requirements for STL containers.
	The scoped ptr is used to avoid leaks with constructor exception
*/
template<class T>
class ScopedPtr
{
public:
	ScopedPtr(T *p = 0)
		: p_(p) {}

	~ScopedPtr()
	{
		delete p_;
	}

	void reset(T *p = 0)
	{
		if(p_ != p) {
			delete p_;
			p_ = p;
		}
	}

	T &operator*() const
	{
		return *p_;
	}

	T *operator->() const
	{
		return p_;
	}

	T *get() const
	{
		return p_;
	}

private:
	// No need for copy and assignment
	ScopedPtr(const ScopedPtr<T> &o);
	ScopedPtr &operator=(const ScopedPtr<T> &o);

	T *p_;
};

} // namespace Common

//------------------------------------------------------------------------------
#endif
