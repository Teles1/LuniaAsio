/*
   Preprocessor 0.5
   Copyright (c) 2005 Anthony Casteel

   This software is provided 'as-is', without any express or implied 
   warranty. In no event will the authors be held liable for any 
   damages arising from the use of this software.

   Permission is granted to anyone to use this software for any 
   purpose, including commercial applications, and to alter it and 
   redistribute it freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you 
      must not claim that you wrote the original software. If you use
	  this software in a product, an acknowledgment in the product 
	  documentation would be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and 
      must not be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source 
      distribution.

   The original version of this library can be located at:
   http://www.angelcode.com/angelscript/
   under addons & utilities or at
   http://www.omnisu.com

   Anthony Casteel
   jm@omnisu.com
*/

#ifndef JM_PREPROCESSOR_STREAM_H
#define JM_PREPROCESSOR_STREAM_H

#include <vector>
#include <string>
#include <sstream>

namespace Preprocessor
{
	class OutStream
	{
	protected:
		virtual void write(const char*,size_t size) = 0;
	public:

		virtual ~OutStream() {}

		template<typename T>
		OutStream& operator<<(const T& in)
		{
			std::stringstream strstr;
			strstr << in;
			std::string str;
			strstr >> str;
			write(str.c_str(),str.length());
			return *this;
		}

		OutStream& operator<<(const std::string& in)
		{
			write(in.c_str(),in.length());
			return *this;
		}		

		OutStream& operator<<(const char* in)
		{
			return operator<<(std::string(in));
		}

		virtual bool valid() { return true; }
	}; //end class Stream

	class VectorOutStream: public OutStream
	{
	private:
		std::vector<char> m_data;
	protected:
		inline void write(const char* d, size_t size) override
		{
			m_data.insert(m_data.end(),d,d+size);
		}

	public:
		const char* data() { return &m_data[0]; }
		size_t size() { return m_data.size(); }
	}; //end class VectorOutStream

	class NullOutStream: public OutStream
	{
	protected:
		inline void write(const char*, size_t) override {}
	}; //end class NullOutStream
}; //end namespace Preprocessor

#endif