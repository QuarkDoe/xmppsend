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

#ifndef _SML_UTILS_EXCEPT_
#define _SML_UTILS_EXCEPT_

#include <string>
#include <exception>

#define EXCPT_GENERAL  1
#define EXCPT_FILEIO   10
#define EXCPT_FOPEN    (EXCPT_FILEIO + 1)
#define EXCPT_FREAD    (EXCPT_FILEIO + 2)
#define EXCPT_FWRITE   (EXCPT_FILEIO + 3)
#define EXCPT_ACCESS   20
#define EXCPT_REQUEST  30
#define EXCPT_PARAMS   40
#define EXCPT_PARSE    50

namespace exc{

class exception : public std::exception {
public:
	exception( const std::string& msg ): Id( EXCPT_GENERAL ), Text( msg ), Source( __PRETTY_FUNCTION__ ) {}
	exception( const std::string& src, const std::string& msg ): Id( EXCPT_GENERAL ), Text( msg ), Source( src ) {}
	exception( int n, const std::string& msg ): Id( n ), Text( msg ), Source( __PRETTY_FUNCTION__ ) {}
	exception( int n, const std::string& src, const std::string& msg ): Id( n ), Text( msg ), Source( src ) {}
	exception( const exception& e )	: Id( e.Id ), Text( e.Text ), Source( e.Source ) {}
	~exception() throw() {
		//Text.std::string::~string();
	}

	exception& operator = ( const exception& e ) {
		Text = e.Text;
		Id = e.Id;
		Source = e.Source;
		return *this;
	}

	int id() const {
		return Id;
	}

	const std::string& text() const {
		return Text;
	}

	const std::string& source() const {
		return Source;
	}

	const char* what() const throw() {
		return Text.c_str();
	}

	const char* ctext() const throw() {
		return Text.c_str();
	}

private:
	int Id;
	std::string Text;
	std::string Source;
};

}

#endif
