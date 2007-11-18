/** \file HTTPSocket.cpp
 **	\date  2004-04-06
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
#include "HTTPSocket.h"
#include <stdarg.h>
#include "Parse.h"
#include "ISocketHandler.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


HTTPSocket::HTTPSocket(ISocketHandler& h)
:TcpSocket(h)
,m_first(true)
,m_header(true)
,m_http_version("HTTP/1.0")
,m_request(false)
,m_response(false)
,m_body_size_left(0)
{
	SetLineProtocol();
	DisableInputBuffer();
}


HTTPSocket::~HTTPSocket()
{
}


void HTTPSocket::OnRawData(const char *buf,size_t len)
{
	if (!m_header)
	{
		size_t sz = m_body_size_left < len ? m_body_size_left : len;
		OnData(buf, sz);
		m_body_size_left -= sz;
		if (!m_body_size_left)
		{
			SetLineProtocol( true );
			m_first = true;
			m_header = true;
			m_body_size_left = 0;
			if (len - sz > 0)
			{
				char tmp[TCP_BUFSIZE_READ];
				memcpy(tmp, buf + sz, len - sz);
				tmp[len - sz] = 0;
				OnRead( tmp, len - sz );
			}
		}
	}
}


void HTTPSocket::OnLine(const std::string& line)
{
	if (m_first)
	{
		Parse pa(line);
		std::string str = pa.getword();
		if (str.substr(0,4) == "HTTP") // response
		{
			m_http_version = str;
			m_status = pa.getword();
			m_status_text = pa.getrest();
			m_response = true;
		}
		else // request
		{
			m_method = str;
			m_url = pa.getword();
			size_t spl = m_url.find("?");
			if (spl != std::string::npos)
			{
				m_uri = m_url.substr(0,spl);
				m_query_string = m_url.substr(spl + 1);
			}
			else
			{
				m_uri = m_url;
			}
			m_http_version = pa.getword();
			m_request = true;
		}
		m_first = false;
		OnFirst();
		return;
	}
	if (!line.size())
	{
		if (m_body_size_left)
		{
			SetLineProtocol(false);
			m_header = false;
		}
		OnHeaderComplete();
		return;
	}
	Parse pa(line,":");
	std::string key = pa.getword();
	std::string value = pa.getrest();
	OnHeader(key,value);
	if (Utility::ToLower(key) == "content-length")
	{
		m_body_size_left = atol(value.c_str());
	}
	/* If remote end tells us to keep connection alive, and we're operating
	in http/1.1 mode (not http/1.0 mode), then we mark the socket to be
	retained. */
#ifdef ENABLE_POOL
	if (!strcasecmp(key.c_str(), "connection") &&
	    !strcasecmp(value.c_str(), "keep-alive") )
	{
		SetRetain();
	}
#endif
}


void HTTPSocket::SendResponse()
{
	std::string msg;
	msg = m_http_version + " " + m_status + " " + m_status_text + "\r\n";
	for (string_m::iterator it = m_response_header.begin(); it != m_response_header.end(); it++)
	{
		std::string key = (*it).first;
		std::string val = (*it).second;
		msg += key + ": " + val + "\r\n";
	}
	msg += "\r\n";
	Send( msg );
}


void HTTPSocket::AddResponseHeader(const std::string& header, const char *format, ...)
{
	char slask[5000]; // temporary for vsprintf / vsnprintf
	va_list ap;

	va_start(ap, format);
#ifdef _WIN32
	vsprintf(slask, format, ap);
#else
	vsnprintf(slask, 5000, format, ap);
#endif
	va_end(ap);

	m_response_header[header] = slask;
}


void HTTPSocket::SendRequest()
{
	std::string msg;
	msg = m_method + " " + m_url + " " + m_http_version + "\r\n";
	for (string_m::iterator it = m_response_header.begin(); it != m_response_header.end(); it++)
	{
		std::string key = (*it).first;
		std::string val = (*it).second;
		msg += key + ": " + val + "\r\n";
	}
	msg += "\r\n";
	Send( msg );
}


std::string HTTPSocket::MyUseragent()
{
	std::string version = "C++Sockets/";
#ifdef _VERSION
	version += _VERSION;
#endif
	return version;
}


void HTTPSocket::Reset()
{
	m_first = true;
	m_header = true;
	m_request = false;
	m_response = false;
	SetLineProtocol(true);
        while (m_response_header.size())
        {
                string_m::iterator it = m_response_header.begin();
                m_response_header.erase(it);
        }

}


const std::string& HTTPSocket::GetMethod()
{
	return m_method;
}


void HTTPSocket::SetMethod(const std::string& x)
{
	m_method = x;
}


const std::string& HTTPSocket::GetUrl()
{
	return m_url;
}


void HTTPSocket::SetUrl(const std::string& x)
{
	m_url = x;
}


const std::string& HTTPSocket::GetUri()
{
	return m_uri;
}


const std::string& HTTPSocket::GetQueryString()
{
	return m_query_string;
}


const std::string& HTTPSocket::GetHttpVersion()
{
	return m_http_version;
}


const std::string& HTTPSocket::GetStatus()
{
	return m_status;
}


const std::string& HTTPSocket::GetStatusText()
{
	return m_status_text;
}


bool HTTPSocket::IsRequest()
{
	return m_request;
}


bool HTTPSocket::IsResponse()
{
	return m_response;
}


void HTTPSocket::SetHttpVersion(const std::string& x)
{
	m_http_version = x;
}


void HTTPSocket::SetStatus(const std::string& x)
{
	m_status = x;
}


void HTTPSocket::SetStatusText(const std::string& x)
{
	m_status_text = x;
}


void HTTPSocket::AddResponseHeader(const std::string& x,const std::string& y)
{
	m_response_header[x] = y;
}


void HTTPSocket::SetUri(const std::string& x)
{
	m_uri = x;
}


void HTTPSocket::url_this(const std::string& url_in,std::string& protocol,std::string& host,port_t& port,std::string& url,std::string& file)
{
	Parse pa(url_in,"/");
	protocol = pa.getword(); // http
	if (!strcasecmp(protocol.c_str(), "https:"))
	{
#ifdef HAVE_OPENSSL
		EnableSSL();
#else
		Handler().LogError(this, "url_this", -1, "SSL not available", LOG_LEVEL_WARNING);
#endif
		port = 443;
	}
	else
	{
		port = 80;
	}
	host = pa.getword();
	if (strstr(host.c_str(),":"))
	{
		Parse pa(host,":");
		pa.getword(host);
		port = static_cast<port_t>(pa.getvalue());
	}
	url = "/" + pa.getrest();
	{
		Parse pa(url,"/");
		std::string tmp = pa.getword();
		while (tmp.size())
		{
			file = tmp;
			tmp = pa.getword();
		}
	}
} // url_this


#ifdef SOCKETS_NAMESPACE
}
#endif

