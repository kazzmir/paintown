/** \file MemFile.cpp
 **	\date  2005-04-25
 **	\author grymse@alhem.net
**/
/*
Copyright (C) 2004-2007  Anders Hedstrom

This library is made available under the terms of the GNU GPL.

If you would like to use this library in a closed-source application,
a separate license agreement is available. For information about 
the closed-source license agreement for the C++ sockets library,
please visit http://www.alhem.net/Sockets/license.html and/or
email license@alhem.net.

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
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif
#include <stdarg.h>

#include "MemFile.h"


#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


std::map<std::string,MemFile::block_t *> MemFile::m_files;


MemFile::MemFile()
:m_temporary(true)
,m_base(new block_t)
,m_current_read(m_base)
,m_current_write(m_base)
,m_read_ptr(0)
,m_write_ptr(0)
,m_b_read_caused_eof(false)
{
}


MemFile::MemFile(const std::string& path)
:m_path(path)
,m_temporary(false)
,m_base(m_files[path])
,m_current_read(NULL)
,m_current_write(NULL)
,m_read_ptr(0)
,m_write_ptr(0)
,m_b_read_caused_eof(false)
{
	if (!m_base)
	{
		m_base = new block_t;
		m_files[path] = m_base;
	}
	m_current_read = m_base;
	m_current_write = m_base;
}


MemFile::~MemFile()
{
	while (m_base && m_temporary)
	{
		block_t *p = m_base;
		m_base = p -> next;
		delete p;
	}
}


bool MemFile::fopen(const std::string& path, const std::string& mode)
{
	return true;
}


void MemFile::fclose()
{
}


/// \todo: fix for reads much larger than BLOCKSIZE
size_t MemFile::fread(char *ptr, size_t size, size_t nmemb) const
{
	size_t p = m_read_ptr % BLOCKSIZE;
	size_t sz = size * nmemb;
	size_t available = m_write_ptr - m_read_ptr;
	if (sz > available) // read beyond eof
	{
		sz = available;
		m_b_read_caused_eof = true;
	}
	if (!sz)
	{
		return 0;
	}
	if (p + sz < BLOCKSIZE)
	{
		memcpy(ptr, m_current_read -> data + p, sz);
		m_read_ptr += sz;
	}
	else
	{
		size_t sz1 = BLOCKSIZE - p;
		size_t sz2 = size - sz1;
		memcpy(ptr, m_current_read -> data + p, sz1);
		m_read_ptr += sz1;
		if (m_current_read -> next)
		{
			m_current_read = m_current_read -> next;
			memcpy(ptr + sz1, m_current_read -> data, sz2);
			m_read_ptr += sz2;
		}
		else
		{
			return sz1;
		}
	}
	return sz;
}


/// \todo: fix for writes that are much larger than BLOCKSIZE
size_t MemFile::fwrite(const char *ptr, size_t size, size_t nmemb)
{
	size_t p = m_write_ptr % BLOCKSIZE;
	size_t sz = size * nmemb;
	if (p + sz <= BLOCKSIZE)
	{
		memcpy(m_current_write -> data + p, ptr, sz);
		m_write_ptr += sz;
	}
	else
	{
		size_t sz1 = BLOCKSIZE - p; // size left
		size_t sz2 = sz - sz1;
		memcpy(m_current_write -> data + p, ptr, sz1);
		block_t *next = new block_t;
		m_current_write -> next = next;
		m_current_write = next;
		memcpy(m_current_write -> data, ptr + sz1, sz2);
		m_write_ptr += sz;
	}
	return sz;
}



char *MemFile::fgets(char *s, int size) const
{
	int n = 0;
	while (n < size - 1 && !eof())
	{
		char c;
		size_t sz = fread(&c, 1, 1);
		if (sz)
		{
			if (c == 10)
			{
				s[n] = 0;
				return s;
			}
			s[n++] = c;
		}
	}
	s[n] = 0;
	return s;
}


void MemFile::fprintf(const char *format, ...)
{
	va_list ap;
	char tmp[BLOCKSIZE];
	va_start(ap, format);
#ifdef _WIN32
	vsprintf(tmp, format, ap);
#else
	vsnprintf(tmp, BLOCKSIZE - 1, format, ap);
#endif
	va_end(ap);
	fwrite(tmp, 1, strlen(tmp));
}


off_t MemFile::size() const
{
	return (off_t)m_write_ptr;
}


bool MemFile::eof() const
{
	return m_b_read_caused_eof; //(m_read_ptr < m_write_ptr) ? false : true;
}


#ifdef SOCKETS_NAMESPACE
}
#endif

