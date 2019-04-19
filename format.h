/*
* Copyright 2019 Pavel Babyak quarkdoe@gmail.com
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef _STRFORMAT_
#define _STRFORMAT_

#include <string>
#include <ostream>
#include <cstdarg>

namespace fmt{

std::string strformat( const std::string& format, ... );
std::string strformat( const char* format, ... );

std::ostream& streamformat( std::ostream& out, const std::string& format, ... );
std::ostream& streamformat( std::ostream& out, const char* format, ... );

std::string& strformat2( std::string& dst, const std::string& format, ... );
std::string& strformat2( std::string& dst, const char* format, ... );

int strscanf( const std::string& src, const std::string& format, ... );
int strscanf( const std::string& src, const char* format, ... );

std::ostream& _ostrformat( std::ostream& out, const std::string& format, std::va_list ap );
int _istrscanf( std::istream& ins, const std::string& format, std::va_list ap );

}

#endif
