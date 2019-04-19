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

#ifndef _OPTIONAL_
#define _OPTIONAL_

#include <memory>

#include "except.h"

namespace opt{
	using namespace exc;
/**
 * @brief контейнер одного значения.
 **/
template <typename _Tp> class optional {
public:
	optional(): _defined( false ) {
	}

	optional( _Tp v ): _value( v ), _defined( true ) {
	}

	optional( const optional<_Tp>& cv ): _value( cv.get() ), _defined( true ) {
	}

	bool defined() {
		return _defined;
	}

	void set( _Tp& v ) noexcept( false ) {
		if( _defined ) throw exception( __PRETTY_FUNCTION__, "value already defined" );
		_value = v;
		_defined = true;
	}

	_Tp& get() noexcept( false ) {
		if( !_defined ) throw exception( __PRETTY_FUNCTION__, "value undefined" );
		else return _value;
	}

	const _Tp& get() const noexcept( false ) {
		if( !_defined ) throw exception( __PRETTY_FUNCTION__, "value undefined" );
		else return _value;
	}

	_Tp& get_or_default( _Tp& defval ) throw() {
		return _defined ? _value : defval;
	}

	const _Tp& get_or_default( const _Tp& defval ) const throw() {
		return _defined ? _value : defval;
	}

	void reset() {
		_defined = false;
	}

	void reset( _Tp& v ) {
		_value = v;
		_defined = true;
	}

	_Tp& operator* () noexcept( false ) {
		if( !_defined ) throw exception( EXCPT_ACCESS, __PRETTY_FUNCTION__, "value undefined" );
		else return _value;
	}

	_Tp* operator& () noexcept( false ) {
		if( !_defined ) throw exception( EXCPT_ACCESS, __PRETTY_FUNCTION__, "value undefined" );
		else return *_value;
	}

	optional<_Tp >& operator= ( const _Tp& v ) noexcept( false ) {
		if( _defined ) throw exception( EXCPT_ACCESS, __PRETTY_FUNCTION__, "value already defined" );
		_value = v;
		_defined = true;
		return *this;
	}

	optional<_Tp >& operator= ( const optional<_Tp >& v ) noexcept( false ) {
		if( _defined ) throw exception( EXCPT_ACCESS, __PRETTY_FUNCTION__, "value already defined" );
		if( this == &v ) return *this;
		_value = v.get();
		_defined = true;
		return *this;
	}

	_Tp* operator->() noexcept( false ) {
		if( !_defined ) throw exception( EXCPT_ACCESS, __PRETTY_FUNCTION__, "value undefined" );
		else return &_value;
	}

	operator _Tp() noexcept( false ) {
		if( !_defined ) throw exception( EXCPT_ACCESS, __PRETTY_FUNCTION__, "value undefined" );
		else return _value;
	}

	operator _Tp&() noexcept( false ) {
		if( !_defined ) throw exception( EXCPT_ACCESS, __PRETTY_FUNCTION__, "value undefined" );
		else return _value;
	}

protected:
	bool _defined;
	_Tp _value;
};

template<typename _Tp> inline std::ostream& operator<<(std::ostream& out, const optional<_Tp>& val) noexcept( false ){
    out << val.get();
    return out;
}

typedef optional<int>    int_c;
typedef optional<size_t> size_c;
typedef optional<double> double_c;
typedef optional<float>  float_c;
typedef optional<bool>   bool_c;
typedef optional<std::string> string_c;

/**
 * @brief контейнер одного значения.
 **/
template <typename _Tp> class weak_optional : public optional<_Tp> {
public:
	weak_optional(): optional<_Tp>() {
	}

	weak_optional( _Tp v ): optional<_Tp>( v ) {
	}

	weak_optional( const optional<_Tp>& cv ): optional<_Tp>( cv ) {
	}

	void set( _Tp& v ) noexcept( false ) {
		optional<_Tp>::_value = v;
		optional<_Tp>::_defined = true;
	}

	weak_optional<_Tp >& operator= ( const _Tp& v ) noexcept( false ) {
		optional<_Tp>::_value = v;
		optional<_Tp>::_defined = true;
		return *this;
	}

	weak_optional<_Tp >& operator= ( const optional<_Tp >& v ) noexcept( false ) {
		if( this == &v ) return *this;
		optional<_Tp>::_value = v.get();
		optional<_Tp>::_defined = true;
		return *this;
	}

};

typedef weak_optional<int>    int_wc;
typedef weak_optional<size_t> size_wc;
typedef weak_optional<double> double_wc;
typedef weak_optional<float>  float_wc;
typedef weak_optional<bool>   bool_wc;
typedef weak_optional<std::string> string_wc;

}

template<typename _T> class t_unique_ptr : public std::unique_ptr<_T>{

public:

	t_unique_ptr<_T>() : std::unique_ptr<_T>(){}

	t_unique_ptr<_T>( _T* ptr ) : std::unique_ptr<_T>( ptr ){}

	inline t_unique_ptr<_T>& operator = ( _T* a ) {
		this->reset( a );
		return *this;
	}

	inline _T& ref(){
		return *this->get();
	}

	inline operator _T* () {
		return this->get();
	}

	inline bool empty() const{
		return this->get() == nullptr;
	}
	
	inline _T& operator* () noexcept( false ) {
		return *this->get();
	}

	inline _T* operator& () noexcept( false ) {
		return this->get();
	}

};

#endif
