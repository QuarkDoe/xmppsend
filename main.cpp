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

#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <libconfig.h++>
#include <gloox/jid.h>

#include <libconfig.h++>
#if ( !( ( LIBCONFIGXX_VER_MAJOR > 1 ) || ( ( LIBCONFIGXX_VER_MAJOR <= 1 ) && ( LIBCONFIGXX_VER_MINOR > 4 ) ) \
 || ( ( LIBCONFIGXX_VER_MAJOR <= 1 ) && ( LIBCONFIGXX_VER_MINOR <= 4 ) && ( LIBCONFIGXX_VER_REVISION >= 7 ) ) ) )
#warning "Require libconfig++ >= 1.4.7"
#endif

#include "except.h"
#include "format.h"
#include "optional.h"
#include "messenger.h"

extern "C" {
#include <getopt.h>
}

using namespace std;
using namespace opt;
using namespace libconfig;
using namespace msg;
using namespace gloox;

const string DEFAULT_ROOT = "xmpp";
string DEFAULT_CONFIG;

struct xmpp_config {

public:

	string_wc conf_file;
	string_wc root;
	string_wc jid;
	string_wc password;
	string_wc server;
	int_wc    port;
// 	string_wc component;
	string_wc message;
	bool_wc   debug;

	vector<Destination> recipients;
	Config cfg;
	JID _JID;

	void init_config() {
		try {
			cfg.readFile( get_conf_file().c_str() );
		} catch( ParseException& e ) {
			throw exc::exception( fmt::strformat( "%s:%s:%s:%d:", e.what(), e.getError(), e.getFile(), e.getLine() ) );
		} catch( ConfigException& e ) {
			// do nothin'
		}
	};

	const string& get_conf_file() noexcept {
		return conf_file.get_or_default( DEFAULT_CONFIG );
	}

	const string& get_root() noexcept {
		return root.get_or_default( DEFAULT_ROOT );
	};

	const string& get_jid() {
		if( !jid.defined() ) {
			jid = cfg.lookup( get_root() )["jid"];
		}

		return *jid;
	};

	const JID& get_JID() {
		if( !( _JID || _JID.setJID( get_jid() ) ) ) {
			throw exc::exception( fmt::strformat( "Incorrect JID '%s'", optarg ) );
		};

		return _JID;
	};

	const string& get_password() {
		if( !password.defined() ) {
			password = cfg.lookup( get_root() )["password"];
		}

		return *password;
	};

	const string& get_server() {
		if( !server.defined() ) {
			if( cfg.exists( get_root() + ".server" ) ) {
				server = cfg.lookup( get_root() )["server"];
			} else {
				server = get_JID().server();
			}
		}

		return *server;
	};

	int get_port() noexcept {
		if( !port.defined() ) {
			if( cfg.exists( get_root() + ".port" ) ) {
				server = cfg.lookup( get_root() )["port"];
			} else {
				port = -1;
			}
		}

		return *port;
	};

	bool get_debug() noexcept {
		return debug.get_or_default( false );
	};

};

void print_help( ostream& );
void parse_opt_args( int, char**, xmpp_config& ) noexcept( false );

string read_from_stream( istream& inp ) {
	// don't skip the whitespace while reading
	inp >> noskipws;

	// use stream iterators to copy the stream to a string
	istream_iterator<char> it( inp );
	istream_iterator<char> end;
	return string( it, end );
}

void print_setting( Setting& s ) {
	clog << s.getPath() << " ";

	if( s.isAggregate() ) {
		clog << endl;

		for( auto& it : s ) {
			print_setting( it );
		}
	} else {
		clog << s.c_str() << endl;
	}
}

int main( int argc, char** argv ) {

	{
		char* t;

		if( NULL != ( t = getenv( "HOME" ) ) ) {
			DEFAULT_CONFIG.assign( t ).append( "/.config/xmppsend.conf" );
		} else {
			DEFAULT_CONFIG.assign( "." ).append( "/.config/xmppsend.conf" );
		}
	}

	t_unique_ptr<xmpp_config> config = new xmpp_config;

	try {
		parse_opt_args( argc, argv, *config );
		config->init_config();

		if( config->get_debug() ) {
			clog << "config.config: " << config->get_conf_file() << endl;
			clog << "config.root: " << config->get_root() << endl;
			clog << "config.jid: " << config->get_jid() << endl;
			clog << "config.jid is valid: " << config->get_JID() << endl;
			clog << "config.password: " << config->get_password().length() << endl;
			clog << "config.server: " << config->get_server() << endl;
			clog << "config.port: " << config->get_port() << endl;
// 			clog << "config.component: " << config->component.get_or_default( "<not defined>" ) << endl;
			clog << "config.message: " << config->message.get_or_default( "<not defined>" ) << endl;
		}
	} catch( exc::exception& e ) {
		cerr << e.text() << endl;
		return e.id();
	} catch( std::exception& e ) {
		cerr << e.what() << endl;
		return -1;
	}

	if( config->recipients.empty() ) {
		cerr << "No destination found." << endl;
		return -1;
	}

	string msg;

	if( config->message.defined() ) {
		ifstream in( config->message );

		if( !in ) {
			cerr << "Message file '" << config->message << "' not found." << endl;
			return -1;
		}

		msg = read_from_stream( in );
		in.close();
	} else {
		msg = read_from_stream( cin );
	}

	if( config->get_debug() ) for( auto& recipient : config->recipients ) {
			cerr << "recipient: " << recipient.jid << " " << recipient.type << endl;
		}

	if( config->get_debug() ) cerr << flush;

	Messenger messenger( config->get_jid(), config->get_password() );
	messenger.setLevel( config->get_debug() ? LogLevelDebug : LogLevelWarning );
	messenger.setServer( config->get_server() );
	messenger.setPort( config->get_port() );
	messenger.setDestinations( config->recipients );
	messenger.setMessage( msg );
	messenger.setDebug( config->get_debug() );

	return messenger.start();
}

void print_help( ostream& out ) {
	out << "Utility to send xmpp message." << endl;
	out << "xmppsend [options] " << endl;
	out << "<recipient_N>     " << "Destination JID." << endl;
	out << "Options:" << endl;
	out << "    -h, --help                          " << "Help message" << endl;
	out << "    -v, --verbose                       " << "Be verbose." << endl;
	out << "    -c, --config <file>                 " << "Configuration file. Default are ~/.config/xmppsend.cfg" << endl;
	out << "    -t, --root <config root>            " << "Configuration root. Default are 'xmpp'." << endl;
	out << "    -j, --jid <user JID>                " << "Use JID instead of the one in the configuration file." << endl;
	out << "    -P, --password <user password>      " << "Use password instead of the one in the configuration file." << endl;
	out << "    -s, --server <server addr>          " << "Alternative host if it is different from the server part of your JID." << endl;
	out << "    -p, --port <port>                   " << "Alternative port." << endl;
// 	out << "    -o, --component <component name>    " << "Use componentname in connect call. Seems needed for Google talk." << endl;
	out << "    -m, --message <message file>        " << "Use supplied file as source of message instead of reading from stdin." << endl;
	out << "    -a, --chatroom <jid>                " << "Add chatroom destination." << endl;
	out << "    -d, --destination <jid>             " << "Add normal destination." << endl;
}

const char* short_options = "hvc:t:j:P:s:p:m:a:d:";
static struct option long_options[] = {
	/* These options don't set a flag.
	We distinguish them by their indices. */
	{ "help",            no_argument,       0, 'h' },    //0
	{ "verbose",         no_argument,       0, 'v' },    //1
	{ "config",          required_argument, 0, 'c' },    //2
	{ "root",            required_argument, 0, 't' },    //3
	{ "jid",             required_argument, 0, 'j' },    //4
	{ "password",        required_argument, 0, 'P' },    //5
	{ "server",          required_argument, 0, 's' },    //6
	{ "port",            required_argument, 0, 'p' },    //7
// 	{ "component",       required_argument, 0, 'o' },    //8
	{ "message",         required_argument, 0, 'm' },    //9
	{ "chatroom",        required_argument, 0, 'a' },    //10
	{ "destination",     required_argument, 0, 'd' },    //11
	{ 0, 0,                                 0, 0   }
};

void parse_opt_args( int argc, char** argv, xmpp_config& config ) noexcept( false ) {
	int c;

	while( 1 ) {
		int opt_index = 0;
		c = getopt_long( argc, argv, short_options, long_options, &opt_index );

		if( c == -1 ) break;

		switch( c ) {

			case 'h':
				print_help( cout );
				exit( 0 );

			case 'v':
				config.debug = true;
				break;

			case 'c':
				config.conf_file = optarg;
				break;

			case 't':
				config.root = optarg;
				break;

			case 'j':
				config.jid = optarg;
				break;

			case 'P':
				config.password = optarg;
				break;

			case 's': 
				config.server = optarg;
				break;
			

			case 'p':{
				const char* portstr = optarg;
				char* eptr = nullptr;
				int temp = strtod( portstr, &eptr );

				if( eptr == portstr )
					throw exc::exception( fmt::strformat( "Invalid port: %s ", portstr ) );

				config.port = temp;
				break;
			}

// 			case 'o':
// 				config.component = optarg;
// 				break;
// 
			case 'd':
			case 'a':
				config.recipients.push_back(
					Destination{
						c == 'd' ? Destination::Normal : Destination::Groupchat,
						optarg
					}
				);
				break;

			case 'm':
				config.message = optarg;
				break;

			default:
				throw exc::exception( fmt::strformat( "Unknown parameter '%c'", c ) );
		}
	}

	if( argc - optind > 0 ) {
		stringstream msg;

		for( size_t idx = optind; idx < argc; idx++ ) {
			fmt::streamformat( msg, "Unrecognized option: '%s'", argv[ idx ] ) << endl;
		}

		throw exc::exception( msg.str() );
	}
}
