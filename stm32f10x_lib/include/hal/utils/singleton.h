/*
 * Singleton.h
 */
#pragma once

#include <stddef.h>
#include <assert.h>

/**
 * Singleton template.
 *
 * The instance is created statically without new, even if not used.
 */
template<class T> class Singleton {
public:
	static T* instance() {
		// assert(_Instance != NULL);
		return &_instance;
	}

protected:
	Singleton();
	virtual ~Singleton();

private:
	Singleton(Singleton const&);
	Singleton& operator=(Singleton const&);
	static T _instance;
};

// Created statically here to avoid a new in the instance method
template<class T> T Singleton<T>::_instance = T();
