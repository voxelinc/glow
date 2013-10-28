#pragma once

namespace glow {

template <class T>
SharedData<T>::SharedData()
{
}

template <class T>
SharedData<T>::SharedData(T* object)
: m_object(object)
{
}

template <class T>
SharedData<T>::SharedData(const SharedData<T>& sharedData)
: m_object(sharedData.m_object)
{
}

template <class T>
SharedData<T>::~SharedData()
{
}

template <class T>
typename SharedData<T>::T* SharedData<T>::operator->()
{
    return m_object->get();
}

template <class T>
SharedData<T>& SharedData<T>::operator=(const SharedData<T>& sharedData)
{
    m_object = sharedData.m_object;
}

template <class T>
bool SharedData<T>::isValid() const
{
    return m_object;
}

template <class T>
SharedData<T>::operator bool() const
{
    return m_object;
}

}
