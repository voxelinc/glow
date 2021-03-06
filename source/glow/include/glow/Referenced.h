#pragma once

#include <glow/glow.h>

namespace glow 
{

/** \brief Superclass for all classes that use reference counting in glow.
    
    The ref counter can be increased and decreased using ref() and unref().
    If the ref counter decreases to zero, the referenced objects is deleted.
    
    Referenced objects should not be copy constructed or assigned.

    \see ref_ptr
 */
class GLOW_API Referenced
{
public:
	Referenced();
	virtual ~Referenced();

	void ref();
	void unref();

	int refCounter() const;

protected:
	Referenced(const Referenced &);
	Referenced & operator=(const Referenced &);

private:
    int m_refCounter;
};

} // namespace glow
