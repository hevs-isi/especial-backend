/*
 * Interrupt.h
 */
#pragma once

class Interrupt {
public:
	virtual ~Interrupt() {
	}

	/**
	 * Called when an Interrupt ReQuest is available.
	 */
	virtual void irq() = 0;
};
