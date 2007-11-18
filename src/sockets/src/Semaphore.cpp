/**
 **	\file Semaphore.cpp
 **	\date  2007-04-13
 **	\author grymse@alhem.net
**/
/*
Copyright (C) 2007  Anders Hedstrom

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include "Semaphore.h"


#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


Semaphore::Semaphore()
{
	sem_init(&m_sem, 0, 0);
}


Semaphore::Semaphore(unsigned int start_val)
{
	sem_init(&m_sem, 0, start_val);
}


Semaphore::~Semaphore()
{
	sem_destroy(&m_sem);
}


int Semaphore::Post()
{
	return sem_post(&m_sem);
}


int Semaphore::Wait()
{
	return sem_wait(&m_sem);
}


int Semaphore::TryWait()
{
	return sem_trywait(&m_sem);
}


int Semaphore::GetValue(int& i)
{
	return sem_getvalue(&m_sem, &i);
}


#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif
