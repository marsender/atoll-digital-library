/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

StdString.cpp

*******************************************************************************/

#include "StdString.hpp"
#include "AppAssert.hpp"
#include <limits>
#include <time.h>
#include <algorithm>
#ifndef WIN32
	#include <stdio.h>
	#include <string.h>
#endif
//------------------------------------------------------------------------------

using namespace Common;

// Much faster than snprintf() - also less generated code!
template<class T>
inline std::string tostring_unsigned(T value)
{
	STATIC_ASSERT_UNSIGNED_TYPE(T);
	// We need a special case for 0, and we might as well handle all single
	// digit numbers with it too.
	if (value < 10)
		return std::string(1, '0' + char(value));

	char buf[(sizeof(T) * 5 + 1) / 2];
	char * p = buf + sizeof(buf);
	do {
		AppAssert(p > buf);
		char ch(value % 10);
		value /= 10;
		*(--p) = ch + '0';
	}
	while (value);

	return std::string(p, buf + sizeof(buf) - p);
}
//------------------------------------------------------------------------------

template<class T>
inline std::string tostring(T value)
{
	// We need a special case for 0, and we might as well handle all single
	// digit positive numbers with it too.
	if (value < 10 && value >= 0)
		return std::string(1, '0' + char(value));

	bool negative = (value < 0);
	if (negative) value = -value;

	char buf[(sizeof(T) * 5 + 1) / 2 + 1];
	char * p = buf + sizeof(buf);
	do {
		AppAssert(p > buf);
		char ch(value % 10);
		value /= 10;
		*(--p) = ch + '0';
	}
	while (value);

	if (negative) {
		AppAssert(p > buf);
		*--p = '-';
	}

	return std::string(p, buf + sizeof(buf) - p);
}
//------------------------------------------------------------------------------

template<class T>
inline std::string format(const char * fmt, T value)
{
	char buf[128];

	size_t size = sprintf(buf, fmt, value);
	// Buffer overflow.
	if (size >= sizeof(buf))
		abort();

	return std::string(buf, size);
}
//------------------------------------------------------------------------------

std::string StdString::Str(int value)
{
	return tostring(value);
}
//------------------------------------------------------------------------------

std::string StdString::Str(unsigned int value)
{
	return tostring_unsigned(value);
}
//------------------------------------------------------------------------------

std::string StdString::Str(long value)
{
	return tostring(value);
}
//------------------------------------------------------------------------------

std::string StdString::Str(unsigned long value)
{
	return tostring_unsigned(value);
}
//------------------------------------------------------------------------------

std::string StdString::Str(long long value)
{
	return tostring(value);
}
//------------------------------------------------------------------------------

std::string StdString::Str(unsigned long long value)
{
	return tostring_unsigned(value);
}
//------------------------------------------------------------------------------

std::string StdString::Str(double value)
{
	return format("%.20g", value);
}
//------------------------------------------------------------------------------

std::string StdString::Str(const void * value)
{
	return format("%p", value);
}
//------------------------------------------------------------------------------

std::string &StdString::AppendBool(std::string &ioStr, bool inBool)
{
	ioStr += inBool ? "true" : "false";

	return ioStr;
}
//------------------------------------------------------------------------------

std::string &StdString::AppendInt(std::string &ioStr, int inInt)
{
	char *buf = new char[std::numeric_limits<int>::digits10 + 2];
	sprintf(buf, "%d", inInt);
	ioStr += buf;
	delete[] buf;

	return ioStr;
}
//------------------------------------------------------------------------------

std::string &StdString::AppendUInt(std::string &ioStr, unsigned int inInt)
{
	char *buf = new char[std::numeric_limits<unsigned int>::digits10 + 2];
	sprintf(buf, "%u", inInt);
	ioStr += buf;
	delete[] buf;

	return ioStr;
}
//------------------------------------------------------------------------------

std::string &StdString::ToLower(std::string &ioStr)
{
	std::transform(ioStr.begin(), ioStr.end(), ioStr.begin(), ::tolower);

	return ioStr;
}
//------------------------------------------------------------------------------

std::string &StdString::FindAndReplace(std::string &ioStr, const char *inFind, const char *inRepl)
{
	size_t i = 0;
	size_t l1 = strlen(inFind);
	size_t l2 = strlen(inRepl);

	while ((i = ioStr.find(inFind, i, l1)) != std::string::npos) {
		ioStr.replace(i, l1, inRepl, l2);
		i += l2;
	}

	return ioStr;
}
//------------------------------------------------------------------------------

std::string &StdString::FindAndReplace(std::string &ioStr, const std::string &inFind, const std::string &inRepl)
{
	size_t i = 0;
	size_t l1 = inFind.length();
	size_t l2 = inRepl.length();

	while ((i = ioStr.find(inFind, i)) != std::string::npos) {
		ioStr.replace(i, i + l1, inRepl);
		i += l2;
	}

	return ioStr;
}
//------------------------------------------------------------------------------

std::string StdString::TimeToString()
{
	time_t t;
	struct tm *tbk;
	char buf[50];

	t = time(NULL); // Get current time
	tbk = localtime(&t);	// Converts date/time to a structure
	sprintf(buf, "%.4d/%.2d/%.2d %.2d:%.2d:%.2d",
		tbk->tm_year + 1900,
		tbk->tm_mon + 1,
		tbk->tm_mday,
		tbk->tm_hour,
		tbk->tm_min,
		tbk->tm_sec);

	return buf;
}
//------------------------------------------------------------------------------
