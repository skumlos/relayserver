/**
 * Copyright (c) 2013, Martin Hejnfelt (martin@hejnfelt.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "Subject.h"
#include "Observer.h"

Subject::Subject() {
	pthread_mutex_init(&m_listMutex,NULL);
}

Subject::~Subject() {
	pthread_mutex_destroy(&m_listMutex);
}

void Subject::attach(Observer* obs)
{
	pthread_mutex_lock(&m_listMutex);
	m_observers.push_back(obs);
	pthread_mutex_unlock(&m_listMutex);
}

void Subject::detach(Observer* obs)
{
	pthread_mutex_lock(&m_listMutex);
	m_observers.remove(obs);
	pthread_mutex_unlock(&m_listMutex);
}

void Subject::notify(void* obj)
{
	pthread_mutex_lock(&m_listMutex);
	std::list<Observer*>::iterator it;
	for(it = m_observers.begin(); it != m_observers.end(); ++it) {
		(*it)->update(this,obj);
	}
	pthread_mutex_unlock(&m_listMutex);
}
