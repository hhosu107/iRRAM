/*

STREAMS.cc -- implementation of the C++-style IO for the iRRAM library
 
Copyright (C) 2004 Norbert Mueller
 
This file is part of the iRRAM Library.
 
The iRRAM Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.
 
The iRRAM Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.
 
You should have received a copy of the GNU Library General Public License
along with the iRRAM Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. 
*/


#include <fstream>
#include <iostream>

#include <iRRAM/STREAMS.h>
#include <iRRAM/cache.h>
#include <iRRAM/SWITCHES.h>
#include <iRRAM/REAL.h>
#include <iRRAM/DYADIC.h>
#include <iRRAM/INTEGER.h>
#include <iRRAM/RATIONAL.h>

namespace iRRAM {

orstream cerr(&std::cerr, false);
orstream clog(&std::clog, false);
orstream cout;
irstream cin;

orstream::orstream(std::ostream * s, bool respect_iteration)
{
	target = s;
	iRRAM_DEBUG1(2, "I/O-handler: Creating output stream \n");
	_respect_iteration = respect_iteration;
	real_w = 20;
	real_f = float_form::absolute;
}
orstream::orstream()
{
	target = &std::cout;
	iRRAM_DEBUG1(2, "I/O-handler: Creating output stream cout\n");
	_respect_iteration = true;
	real_w = 20;
	real_f = float_form::absolute;
}
orstream::orstream(std::string s, std::ios::openmode mod)
{
	if (actual_stack().inlimit > 0) {
		iRRAM_DEBUG1(2, "I/O-handler: Operation illegal in continuous "
		                "section!\n");
		return;
	}
	_respect_iteration = true;
	real_w = 20;
	real_f = float_form::absolute;

	if (actual_stack().inlimit == 0) {
		if (cache<std::ostream *>().get(target)) {
			/*    state.thread_data_address->cache_ui.get(real_w);
			    state.thread_data_address->cache_ui.get(real_f);
			    state.thread_data_address->cache_b.get(_respect_iteration);*/
			iRRAM_DEBUG1(2,
			             "I/O-handler: Recreating output stream '"
			                     << s << "'(" << real_w << ")\n");
			return;
		}
		target = new std::ofstream(s.c_str(), mod);
		iRRAM_DEBUG1(2, "I/O-handler: Creating new output stream '"
		                        << s << "'\n");
		cache<std::ostream *>().put(target);
		/*    state.thread_data_address->cache_ui.put(real_w);
		    state.thread_data_address->cache_ui.put(real_f);
		    state.thread_data_address->cache_b.put(_respect_iteration);*/
	} else {
		target = new std::ofstream(s.c_str(), mod);
		iRRAM_DEBUG1(2, "I/O-handler: Creating new output stream '"
		                        << s << "'\n");
	}
	return;
}

irstream::irstream()
{
	iRRAM_DEBUG1(2, "I/O-handler: Creating input stream cin\n");
	target = &std::cin;
}

irstream::irstream(std::string s, std::ios::openmode mod)
{
	if (actual_stack().inlimit > 0) {
		iRRAM_DEBUG1(2, "I/O-handler: Operation illegal in continuous "
		                "section!\n");
		return;
	}
	if (actual_stack().inlimit == 0) {
		if (cache<std::istream *>().get(target)) {
			return;
		}
		iRRAM_DEBUG1(2, "I/O-handler: Creating new input stream '"
		                        << s << "'\n");
		target = new std::ifstream(s.c_str(), mod);
		cache<std::istream *>().put(target);
	} else {
		iRRAM_DEBUG1(2, "I/O-handler: Creating new input stream '"
		                        << s << "'\n");
		target = new std::ifstream(s.c_str(), mod);
	}
	return;
}




void orstream::rewind() { state->requests = 0; }

void orstream::reset()
{
	state->requests = 0;
	state->outputs = 0;
}

#define iRRAM_outexec(x)                                                       \
	if (state->inReiterate) {                                              \
		iRRAM_DEBUG1(2, "I/O-Handler: In iteration, so stream will "   \
		                "be closed later.\n");                         \
		return;                                                        \
	}                                                                      \
	if (actual_stack().inlimit > 0) {                                      \
		iRRAM_DEBUG1(2, "illegal output in continuous section!\n");    \
		return;                                                        \
	}                                                                      \
	if (actual_stack().inlimit == 0) {                                     \
		single_valued code;                                            \
		x;                                                             \
	} else {                                                               \
		x;                                                             \
	}

template <class PARAM>
static orstream & iRRAM_out(orstream * s, const PARAM & x)
{
	if (actual_stack().inlimit > 0 && s->_respect_iteration) {
		iRRAM_DEBUG1(2, "illegal output in continuous section!\n");
		return *s;
	}
	if (actual_stack().inlimit == 0 && s->_respect_iteration) {
		single_valued code;
		if (++state->requests > state->outputs) {
			*s->target << x;
			state->outputs++;
		}
	} else {
		*s->target << x;
	}
	return *s;
}

#define iRRAM_out2(x)                                                          \
	if (actual_stack().inlimit > 0 && _respect_iteration) {                \
		iRRAM_DEBUG1(2, "I/O-handler: Illegal output in continuous "   \
		                "section!\n");                                 \
		return *this;                                                  \
	}                                                                      \
	x;                                                                     \
	return *this;

orstream& orstream::operator<<(const bool c)           {return iRRAM_out<bool>(this, c);}
orstream& orstream::operator<<(const char* c)          {return iRRAM_out<const char *>(this, c);}
orstream& orstream::operator<<(const short i)          {return iRRAM_out<short>(this, i);}
orstream& orstream::operator<<(const unsigned short i) {return iRRAM_out<unsigned short>(this, i);}
orstream& orstream::operator<<(const int i)            {return iRRAM_out<int>(this, i);}
orstream& orstream::operator<<(const unsigned int ui)  {return iRRAM_out<unsigned int>(this, ui);}
orstream& orstream::operator<<(const long i)           {return iRRAM_out<long>(this, i);}
orstream& orstream::operator<<(const unsigned long ui) {return iRRAM_out<unsigned long>(this, ui);}
orstream& orstream::operator<<(const double d)         {return iRRAM_out<double>(this, d);}
orstream& orstream::operator<<(const float d)          {return iRRAM_out<float>(this, d);}
orstream& orstream::operator<<(const std::string& s)   {return iRRAM_out<std::string>(this, s);}
orstream& orstream::operator<<(const long long ll)     {return iRRAM_out<long long>(this, ll);}
orstream& orstream::operator<<(const unsigned long long ull) {return iRRAM_out<unsigned long long>(this, ull);}

orstream& orstream::operator<<(const REAL& x)     {return iRRAM_out<>(this, swrite(x,real_w,real_f));}
orstream& orstream::operator<<(const DYADIC& x)   {return iRRAM_out<>(this, swrite(x,real_w));}
orstream& orstream::operator<<(const INTEGER& x)  {return iRRAM_out<>(this, swrite(x,real_w));}
orstream& orstream::operator<<(const RATIONAL& x) {return iRRAM_out<>(this, swrite(x,real_w));}

orstream& orstream::operator<<(decltype(std::setw(0)) _f) {return iRRAM_out<>(this, _f);}
orstream& orstream::operator<<(decltype(std::setprecision(0)) _f) {return iRRAM_out<>(this, _f);}
orstream& orstream::operator<<(decltype(std::setfill('*')) _f) {return iRRAM_out<>(this, _f);}
orstream& orstream::operator<<(decltype(std::setiosflags(std::ios_base::dec)) _f) {return iRRAM_out<>(this, _f);}
orstream& orstream::operator<<(decltype(std::resetiosflags(std::ios_base::dec)) _f) { return iRRAM_out<>(this, _f);}
orstream& orstream::operator<<(decltype(std::setbase(0)) _f)    {return iRRAM_out<>(this, _f);}
orstream& orstream::operator<<(std::ostream& _f(std::ostream&)) {return iRRAM_out<>(this, _f);}

orstream & orstream::operator<<(_SetRwidth _f) { iRRAM_out2(real_w = _f._M_n); }
orstream & orstream::operator<<(_SetRflags _f) { iRRAM_out2(real_f = _f._M_n); }

orstream::~orstream()
{
	if (++state->requests > state->outputs) {
		if (target != &std::cout && _respect_iteration) {
			iRRAM_DEBUG1(2, "I/O-handler: Closing handler for "
			                "output stream\n");
			iRRAM_outexec(delete target; target = 0;);
		}
		state->outputs++;
	}
}

irstream::~irstream()
{
	if (++state->requests > state->outputs) {
		if (target != &std::cin) {
			iRRAM_DEBUG1(2, "I/O-Handler: Closing handler for "
			                "input stream\n");
			iRRAM_outexec(delete target; target = 0;);
		}
		state->outputs++;
	}
}


#define iRRAM_in(VAR, CACHE)                                                   \
	if (actual_stack().inlimit > 0) {                                      \
		iRRAM_DEBUG1(2, "illegal input in continuous section!\n");     \
		return *this;                                                  \
	}                                                                      \
	if (actual_stack().inlimit == 0) {                                     \
		if (CACHE.get(VAR)) {                                          \
			return *this;                                          \
		}                                                              \
		{ single_valued code; *target >> VAR; }                        \
		CACHE.put(VAR);                                                \
	} else {                                                               \
		*target >> VAR;                                                \
	}                                                                      \
	return *this;


irstream& irstream::operator>>(bool& b)            {iRRAM_in(b,cache<bool>());}
irstream& irstream::operator>>(short& i)           {iRRAM_in(i,cache<short>());}
irstream& irstream::operator>>(unsigned short& ui) {iRRAM_in(ui,cache<unsigned short>());}
irstream& irstream::operator>>(int& i)             {iRRAM_in(i,cache<int>());}
irstream& irstream::operator>>(unsigned int& ui)   {iRRAM_in(ui,cache<unsigned>());}
irstream& irstream::operator>>(long& l)            {iRRAM_in(l,cache<long>());}
irstream& irstream::operator>>(unsigned long& ul)  {iRRAM_in(ul,cache<unsigned long>());}
irstream& irstream::operator>>(float& d)           {iRRAM_in(d,cache<float>());}
irstream& irstream::operator>>(double& d)          {iRRAM_in(d,cache<double>());}
irstream& irstream::operator>>(long long& ll)      {iRRAM_in(ll,cache<long long>());}
irstream& irstream::operator>>(unsigned long long& ull) {iRRAM_in(ull,cache<unsigned long long>());}
irstream& irstream::operator>>(std::string& s)     {iRRAM_in(s,cache<std::string>());}

#define iRRAM_in2(VAR, DATA)                                                   \
	std::string s;                                                         \
	if (actual_stack().inlimit > 0) {                                      \
		iRRAM_DEBUG1(2, "I/O-handler: Illegal input in continuous "    \
		                "section!\n");                                 \
		return *this;                                                  \
	}                                                                      \
	if (actual_stack().inlimit == 0) {                                     \
		if (get_cached(s)) {                                           \
			VAR = DATA(s);                                         \
			return *this;                                          \
		}                                                              \
		{ single_valued code; *target >> s; VAR = DATA(s); }           \
		put_cached(s);                                                 \
	} else {                                                               \
		*target >> s;                                                  \
		VAR = DATA(s);                                                 \
	};                                                                     \
	return *this;

irstream & irstream::operator>>(REAL & d)    { iRRAM_in2(d, REAL); }
irstream & irstream::operator>>(INTEGER & d) { iRRAM_in2(d, INTEGER); }
// irstream&  irstream::operator>>(DYADIC& d){iRRAM_in2(d,DYADIC);}

#define iRRAM_inexec(VAR, CACHE, STMNT)                                        \
	if (actual_stack().inlimit > 0) {                                      \
		iRRAM_DEBUG1(2, "I/O-handler: Illegal input in continuous "    \
		                "section!\n");                                 \
		return VAR;                                                    \
	}                                                                      \
	if (actual_stack().inlimit == 0) {                                     \
		if (CACHE.get(VAR)) {                                          \
			return VAR;                                            \
		}                                                              \
		{ single_valued code; STMNT; }                                 \
		CACHE.put(VAR);                                                \
	} else { /* TODO: wtf? that can't happen... */                         \
		STMNT;                                                         \
	}                                                                      \
	return VAR;

bool orstream::eof()
{
	bool test = false;
	iRRAM_inexec(test, cache<bool>(), test = target->eof())
}

bool orstream::fail()
{
	bool test = false;
	iRRAM_inexec(test, cache<bool>(), test = target->fail())
}

bool orstream::good()
{
	bool test = false;
	iRRAM_inexec(test, cache<bool>(), test = target->good())
}

bool orstream::bad()
{
	bool test = false;
	iRRAM_inexec(test, cache<bool>(), test = target->bad())
}

bool operator!(orstream & x)
{
	bool test = false;
	iRRAM_inexec(test, cache<bool>(), test = (x.target->fail()))
}

orstream::operator bool()
{
	bool test = false;
	iRRAM_inexec(test, cache<bool>(), test = !(target->fail()))
}

bool irstream::eof()
{
	bool test = false;
	iRRAM_inexec(test, cache<bool>(), test = target->eof())
}

bool irstream::fail()
{
	bool test = false;
	iRRAM_inexec(test, cache<bool>(), test = target->fail())
}

bool irstream::good()
{
	bool test = false;
	iRRAM_inexec(test, cache<bool>(), test = target->good())
}

bool irstream::bad()
{
	bool test = false;
	iRRAM_inexec(test, cache<bool>(), test = target->bad())
}

bool operator!(irstream & x)
{
	bool test = false;
	iRRAM_inexec(test, cache<bool>(), test = x.target->fail())
}

irstream::operator bool()
{
	bool test = false;
	iRRAM_inexec(test, cache<bool>(), test = !target->fail())
}


} // namespace iRRAM
