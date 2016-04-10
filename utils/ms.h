#ifndef MS_H
#define MS_H
#include <time.h>

namespace ms {
	void init();
	unsigned int get();
	bool isPast(unsigned int ms);
	struct timespec getAbsTime(unsigned int abstime_ms);
};

#endif /* MS_H */
