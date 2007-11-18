/** \file HttpdSocket.cpp
*/
/*
Copyright (C) 2001-2007  Anders Hedstrom (grymse@alhem.net)

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
#include "Utility.h"
#include "HttpdCookies.h"
#include "HttpdForm.h"
#include "MemFile.h"
#include "HttpdSocket.h"


#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

#define TMPSIZE 10000


// statics
int HttpdSocket::m_request_count = 0;
std::string HttpdSocket::m_start = "";


HttpdSocket::HttpdSocket(ISocketHandler& h)
:HTTPSocket(h)
,m_content_length(0)
,m_file(NULL)
,m_received(0)
,m_request_id(++m_request_count)
,m_cookies(NULL)
,m_form(NULL)
{
	m_http_date = datetime2httpdate(GetDate());
	if (!m_start.size())
		m_start = m_http_date;
}


HttpdSocket::~HttpdSocket()
{
	if (m_file)
	{
		delete m_file;
	}
	if (m_cookies)
		delete m_cookies;
	if (m_form)
		delete m_form;
}


void HttpdSocket::OnFirst()
{
}


void HttpdSocket::OnHeader(const std::string& key,const std::string& value)
{
	if (!strcasecmp(key.c_str(),"content-length"))
	{
		m_content_length = atoi(value.c_str());
		m_content_length_str = value;
	}
	else
	if (!strcasecmp(key.c_str(),"cookie"))
	{
		m_http_cookie = value;
	}
	else
	if (!strcasecmp(key.c_str(),"content-type"))
	{
		m_content_type = value;
	}
	else
	if (!strcasecmp(key.c_str(),"if-modified-since"))
	{
		m_if_modified_since = value;
	}
}


void HttpdSocket::OnHeaderComplete()
{
	m_cookies = new HttpdCookies(m_http_cookie);

	if (GetMethod() == "GET")
	{
		Utility::SetEnv("QUERY_STRING", GetQueryString());
	}
	Utility::SetEnv("REQUEST_METHOD", GetMethod());
	Utility::SetEnv("HTTP_COOKIE", m_http_cookie);
	Utility::SetEnv("CONTENT_TYPE", m_content_type);
	Utility::SetEnv("CONTENT_LENGTH", m_content_length_str);
/*
#if (defined(SOLARIS8) || defined(SOLARIS))
	{
		char slask[TMPSIZE];
		if (GetMethod() == "GET")
		{
			sprintf(slask,"QUERY_STRING=%s", GetQueryString().c_str());
			putenv(slask);
		}
		sprintf(slask,"REQUEST_METHOD=%s", GetMethod().c_str());
		putenv(slask);
		sprintf(slask,"HTTP_COOKIE=%s", m_http_cookie.c_str());
		putenv(slask);
		sprintf(slask,"CONTENT_TYPE=%s", m_content_type.c_str());
		putenv(slask);
		sprintf(slask,"CONTENT_LENGTH=%s", m_content_length_str.c_str());
		putenv(slask);
	}
#elif defined _WIN32
	{
		char slask[TMPSIZE];
		if (GetMethod() == "GET")
		{
			sprintf(slask,"QUERY_STRING=%s", GetQueryString().c_str());
			_putenv(slask);
		}
		sprintf(slask,"REQUEST_METHOD=%s", GetMethod().c_str());
		_putenv(slask);
		sprintf(slask,"HTTP_COOKIE=%s", m_http_cookie.c_str());
		_putenv(slask);
		sprintf(slask,"CONTENT_TYPE=%s", m_content_type.c_str());
		_putenv(slask);
		sprintf(slask,"CONTENT_LENGTH=%s", m_content_length_str.c_str());
		_putenv(slask);
	}
#else
	if (GetMethod() == "GET")
	{
		setenv("QUERY_STRING", GetQueryString().c_str(), 1);
	}
	setenv("REQUEST_METHOD", GetMethod().c_str(), 1);
	setenv("HTTP_COOKIE", m_http_cookie.c_str(), 1);
	setenv("CONTENT_TYPE", m_content_type.c_str(), 1);
	setenv("CONTENT_LENGTH", m_content_length_str.c_str(), 1);
#endif
*/
	if (GetMethod() == "POST")
	{
		m_file = new MemFile;
	}
	else
	if (GetMethod() == "GET")
	{
		m_form = new HttpdForm(GetQueryString(), GetQueryString().size() );
		AddResponseHeader("Date", datetime2httpdate(GetDate()) );
		if (GetUri() == "/image")
		{
			std::string str64 = 
"iVBORw0KGgoAAAANSUhEUgAAAGAAAABeCAIAAABTioayAAAACXBIWXMAAABkAAAAZAAPlsXdAAAAUHRF"
"WHRSYXcgcHJvZmlsZSB0eXBlIEFQUDEyAApnZW5lcmljIHByb2ZpbGUKICAgICAgMTUKNDQ3NTYzNmI3"
"OTAwMDEwMDA0MDAwMDAwM2MwMDAwCg2F1B0AABb4SURBVHja7Xx7rGVXed/3WGs/zvPeuTPXMzbjcUpq"
"HAwlpQlYoUlDaJsAiVuFFKSKGlUlKgXqIKJAKxfRR4IUZEpSt1FoQEK4oYQEErlSFAoKIEoo2BQasBmc"
"2NjyYzxz5957Hnufvfd6fF//2Oecez2hvbXvmZkg3U9b556Zc85ea//291rf+n0bVRWO5P8udLUn8Jdd"
"jgA6QI4AOkCOADpAjgA6QI4AOkCOADpAjgA6QI4AOkCOADpAjgA6QI4AOkCOADpAjgA6QMxVHHtfoUWx"
"/dP+C5fvABGv4gwBAK9qPWg5NLbvrjIY302ujgapAmKrQQiqC3xAQQF0H1AIcyXSq4XeZdegfedfWs78"
"jYKAAgDi3sVri4mqzH+D9DSTu+I2dyVNTEFRl34GUTUikiqotrgoAqqIAhBxqzj7ZocAighXWJUuI0Cq"
"injJ+SMAIZKolkXlGrOzM61mjfckCoN+X9UjorU26wgb7HTSPAfDRlVVFQAR21NeOT26XAC1p10CpKoI"
"iKTO+ckkFpO4vVX1Bh3kkKRsDCIBE4sIMRNx8FTPYgwQfJ1k4ZpT3W4nAQBVQaSF/1pcw+VE63JrEKiC"
"KhChC83Fi6EYEyp2uzxYN4kFY6H1O63TIUQAiCIIEgK5Rke7zWhUE2a9AR+/JunmpIr7of/eA2hhWREA"
"QUlBkGg8af7s7E4n75+6Lu/2iBkUAgJLhP2+RkSJsNU5ACUCQCAyxTQ88eS0rt1gSGdOD621Ik8zs8uH"
"0eUCSDSqCDHGUD/2mCvLXpbWp051bEqqGoIAINHeVRHRJTMREQBEACQEUDa0vT07/2QZvbvxphO9vhGh"
"1m0j0uVzSysGaOF6QASJQvD4yKN1UeiZ053eULzX4FsspB221RdVZWYARaTFaeYXjIgiqqpEag16jw9+"
"e3daTH/gBevHjw1EoIXle0ODdJH/KQiCxiiPPOLrWX7taeh1o3ek8wQH26jUao1qa1ZPy6VbgNovtIAi"
"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"
"MhMxAmhTBwhU"
"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";
			Send64(str64, "image/png");
		}
		else
		{
			Exec();
		}
		Reset(); // prepare for next request
	}
	else
	{
		AddResponseHeader("Date", GetHttpDate());
		AddResponseHeader("Connection", "close");
		SetStatus("405");
		SetStatusText("Method not allowed");
		SendResponse();
	}
}


void HttpdSocket::OnData(const char *p,size_t l)
{
	if (m_file)
	{
		m_file -> fwrite(p,1,l);
	}
	m_received += l;
	if (m_received >= m_content_length && m_content_length)
	{
		// all done
		if (m_file && !m_form)
		{
			m_form = new HttpdForm(m_file, m_content_type, m_content_length);
			AddResponseHeader("Date", datetime2httpdate(GetDate()) );
			if (GetUri() == "/image")
			{
				std::string str64 = 
"iVBORw0KGgoAAAANSUhEUgAAAGAAAABeCAIAAABTioayAAAACXBIWXMAAABkAAAAZAAPlsXdAAAAUHRF"
"WHRSYXcgcHJvZmlsZSB0eXBlIEFQUDEyAApnZW5lcmljIHByb2ZpbGUKICAgICAgMTUKNDQ3NTYzNmI3"
"OTAwMDEwMDA0MDAwMDAwM2MwMDAwCg2F1B0AABb4SURBVHja7Xx7rGVXed/3WGs/zvPeuTPXMzbjcUpq"
"HAwlpQlYoUlDaJsAiVuFFKSKGlUlKgXqIKJAKxfRR4IUZEpSt1FoQEK4oYQEErlSFAoKIEoo2BQasBmc"
"2NjyYzxz5957Hnufvfd6fF//2Oecez2hvbXvmZkg3U9b556Zc85ea//291rf+n0bVRWO5P8udLUn8Jdd"
"jgA6QI4AOkCOADpAjgA6QI4AOkCOADpAjgA6QI4AOkCOADpAjgA6QI4AOkCOADpAjgA6QMxVHHtfoUWx"
"/dP+C5fvABGv4gwBAK9qPWg5NLbvrjIY302ujgapAmKrQQiqC3xAQQF0H1AIcyXSq4XeZdegfedfWs78"
"jYKAAgDi3sVri4mqzH+D9DSTu+I2dyVNTEFRl34GUTUikiqotrgoAqqIAhBxqzj7ZocAighXWJUuI0Cq"
"injJ+SMAIZKolkXlGrOzM61mjfckCoN+X9UjorU26wgb7HTSPAfDRlVVFQAR21NeOT26XAC1p10CpKoI"
"iKTO+ckkFpO4vVX1Bh3kkKRsDCIBE4sIMRNx8FTPYgwQfJ1k4ZpT3W4nAQBVQaSF/1pcw+VE63JrEKiC"
"KhChC83Fi6EYEyp2uzxYN4kFY6H1O63TIUQAiCIIEgK5Rke7zWhUE2a9AR+/JunmpIr7of/eA2hhWREA"
"QUlBkGg8af7s7E4n75+6Lu/2iBkUAgJLhP2+RkSJsNU5ACUCQCAyxTQ88eS0rt1gSGdOD621Ik8zs8uH"
"0eUCSDSqCDHGUD/2mCvLXpbWp051bEqqGoIAINHeVRHRJTMREQBEACQEUDa0vT07/2QZvbvxphO9vhGh"
"1m0j0uVzSysGaOF6QASJQvD4yKN1UeiZ053eULzX4FsspB221RdVZWYARaTFaeYXjIgiqqpEag16jw9+"
"e3daTH/gBevHjw1EoIXle0ODdJH/KQiCxiiPPOLrWX7taeh1o3ek8wQH26jUao1qa1ZPy6VbgNovtIAi"
"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"
"MhMxAmhTBwhU"
"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";
				Send64(str64, "image/png");
			}
			else
			{
				Exec();
			}
			Reset(); // prepare for next request
		}
	}
}


void HttpdSocket::Send64(const std::string& str64, const std::string& type)
{
	Base64 bb;

	if (!strcasecmp(m_start.c_str(), m_if_modified_since.c_str()))
	{
		SetStatus("304");
		SetStatusText("Not Modified");
		SendResponse();
	}
	else
	{
		size_t len = bb.decode_length(str64);
		unsigned char *buf = new unsigned char[len];

		SetStatus("200");
		SetStatusText("OK");

		AddResponseHeader("Content-length", Utility::l2string( (long)len) );
		AddResponseHeader("Content-type", type );
		AddResponseHeader("Last-modified", m_start);
		SendResponse();

		bb.decode(str64, buf, len);
		SendBuf( (char *)buf, len);
		delete[] buf;
	}
}


std::string HttpdSocket::datetime2httpdate(const std::string& dt)
{
	struct tm tp;
	time_t t;
	const char *days[] = { "Sun","Mon","Tue","Wed","Thu","Fri","Sat" };
	const char *months[] = { "Jan","Feb","Mar","Apr","May","Jun",
	                   "Jul","Aug","Sep","Oct","Nov","Dec" };
	int i;
	char s[40];

/* 1997-12-16 09:50:40 */

	if (dt.size() == 19) 
	{
		tp.tm_year = atoi(dt.substr(0,4).c_str()) - 1900;
		i = atoi(dt.substr(5,2).c_str()) - 1;
		tp.tm_mon = i >= 0 ? i : 0;
		tp.tm_mday = atoi(dt.substr(8,2).c_str());
		tp.tm_hour = atoi(dt.substr(11,2).c_str());
		tp.tm_min = atoi(dt.substr(14,2).c_str());
		tp.tm_sec = atoi(dt.substr(17,2).c_str());
		tp.tm_wday = 0;
		tp.tm_yday = 0;
		tp.tm_isdst = 0;
		t = mktime(&tp);
		if (t == -1)
		{
			Handler().LogError(this, "datetime2httpdate", 0, "mktime() failed");
		}

		sprintf(s,"%s, %02d %s %d %02d:%02d:%02d GMT",
		 days[tp.tm_wday],
		 tp.tm_mday,
		 months[tp.tm_mon],
		 tp.tm_year + 1900,
		 tp.tm_hour,tp.tm_min,tp.tm_sec);
	} 
	else
	{
		*s = 0;
	}
	return s;
}


std::string HttpdSocket::GetDate()
{
	time_t t = time(NULL);
	struct tm tp;
#ifdef _WIN32
	memcpy(&tp, localtime(&t), sizeof(tp));
#else
	localtime_r(&t, &tp);
#endif
	char slask[40]; // yyyy-mm-dd hh:mm:ss
	sprintf(slask,"%d-%02d-%02d %02d:%02d:%02d",
		tp.tm_year + 1900,
		tp.tm_mon + 1,
		tp.tm_mday,
		tp.tm_hour,tp.tm_min,tp.tm_sec);
	return slask;
}


void HttpdSocket::Reset()
{
	HTTPSocket::Reset();
	m_content_length = 0;
	if (m_file)
	{
		delete m_file;
		m_file = NULL;
	}
	m_received = 0;
	m_request_id = ++m_request_count;
	if (m_cookies)
		delete m_cookies;
	m_cookies = NULL;
	if (m_form)
		delete m_form;
	m_form = NULL;
}


const std::string& HttpdSocket::GetHttpDate() const
{
	return m_http_date; 
}


HttpdCookies *HttpdSocket::GetCookies()
{
	return m_cookies; 
}


const HttpdForm *HttpdSocket::GetForm() const
{
	return m_form; 
}



#ifdef SOCKETS_NAMESPACE
}
#endif

