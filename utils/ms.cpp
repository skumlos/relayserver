#include "ms.h"

unsigned int ms::get() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC,&ts);
	return ((ts.tv_sec * 1000) + (ts.tv_nsec/1000000));
}

bool ms::isPast(unsigned int t) {
	return (get() > t);
}

struct timespec
ms::getAbsTime(unsigned int future_ms) {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC,&ts);
	ts.tv_sec += future_ms / 1000;
	ts.tv_nsec += (future_ms % 1000) * 1000000;
	return ts;
}

