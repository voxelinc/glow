#pragma once

#include <glow/ref_ptr.h>

namespace glow
{

template <class T>
class SharedData
{
public:
	SharedData();
    SharedData(T* object);
    SharedData(const SharedData<T>& sharedData);

    virtual ~SharedData();

    T* operator->();
    SharedData<T>& operator=(const SharedData<T>& sharedData);
    operator bool() const;

    bool isValid() const;
protected:
    ref_ptr<T> m_object;
};

}

#include <glowsd/SharedData.hpp>
