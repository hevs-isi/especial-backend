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
 *
 * @see http://come-david.developpez.com/tutoriels/dps/?page=Singleton
 * @author	Christopher Métrailler (mei@hevs.ch)
 */
template<class T> class Singleton {
public:

	/**
	 * @brief Return the single instance of the class.
	 * The instance is created statically (no `new`) even if nobody use it.
	 */
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
