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

#include <gloox/message.h>
#include <gloox/mucroom.h>
#include <gloox/disco.h>
#include <gloox/presence.h>
#include <gloox/messagesession.h>
#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>

#include "messenger.h"

using namespace std;
using namespace msg;
using namespace gloox;

const char* get_log_area( LogArea );
const char* get_log_level( LogLevel );

Messenger::Messenger( const string& user_jid, const string& user_pwd ) {
	debug = false;
	result = 0;
	logLevel = LogLevelWarning;
	jid = new JID( user_jid );
	pwd = user_pwd;
	client = new Client( *jid, pwd );
}

Messenger::~Messenger() {
	if( debug ) cerr << __PRETTY_FUNCTION__ << endl;

	client->disconnect();
	delete client;
	delete jid;
}

int Messenger::start() {
	if( debug ) cerr << __PRETTY_FUNCTION__ << endl;

	client->registerConnectionListener( this );
	client->registerMessageHandler( this );
	client->logInstance().registerLogHandler( logLevel, LogAreaAll, this );

	if( debug ) {
		cerr << __PRETTY_FUNCTION__ << ": connect to " << client->server() << ":" << client->port() << endl;
	}

	client->connect( true );

	return result;
}

void Messenger::onConnect() {
	if( debug ) cerr << __PRETTY_FUNCTION__  << ": connected" << endl;

	client->setPresence( Presence::Available, -1 );

	if( debug ) cerr << __PRETTY_FUNCTION__  << ": total destinations " << destinations.size() << endl;

	for( auto& dst : destinations ) {
		switch( dst.type ) {
			case Destination::Normal :
				sendTo( dst.jid );
				break;

			case Destination::Groupchat :
				sendToRoom( dst.jid );
				break;
		}
	}

	//костыли
	std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

	{
		std::lock_guard<std::mutex> lck( this->map_guard );

		if( muc_senders.empty() )  client->disconnect();
	}

	if( debug ) cerr << __PRETTY_FUNCTION__  << ": finished" << endl;
}

void Messenger::sendTo( const string& dst ) {

	if( debug ) cerr << __PRETTY_FUNCTION__  << ": send to:" << dst << endl;

	JID to( dst );
	auto session = new MessageSession( client, to );
	session->send( message );
	client->disposeMessageSession( session );

}



void Messenger::sendToRoom( const string& room ) {

	if( debug ) cerr << __PRETTY_FUNCTION__  << ": send to room:" << room << endl;

	JID nick( room );

	if( nick.resource().empty() ) {
		nick.setResource( jid->username() );
	}

	const string& bare = nick.bare();

	shared_ptr<MUCSender> sender( new MUCSender( this, room ) );

	bool inserted = false;
	{
		std::lock_guard<std::mutex> lck( this->map_guard );
		inserted = muc_senders.emplace( bare, sender ).second;
	}

	if( inserted ) sender->start();

}

bool Messenger::onTLSConnect( const CertInfo& info ) {
	if( debug ) {
		time_t from( info.date_from );
		time_t to( info.date_to );
		cerr << __PRETTY_FUNCTION__ << endl;
		cerr << __PRETTY_FUNCTION__  << ": TLS connect" << endl;
		cerr << __PRETTY_FUNCTION__  << ": Server: " << info.server << endl;
		cerr << __PRETTY_FUNCTION__  << ": Protocol: " << info.protocol << endl;
		cerr << __PRETTY_FUNCTION__  << ": Issuer: " << info.issuer << endl;
		cerr << __PRETTY_FUNCTION__  << ": Compression: " << info.compression << endl;
		cerr << __PRETTY_FUNCTION__  << ": Status: " << info.status << endl;
		cerr << __PRETTY_FUNCTION__  << ": from: " << ctime( &from );
		cerr << __PRETTY_FUNCTION__  << ": to: " << ctime( &to );
	}

	return true;
}

const char* get_stream_error( StreamError e ) {
	switch( e ) {
		case StreamErrorBadFormat:
			return "StreamError: bad format";

		case StreamErrorBadNamespacePrefix:
			return "StreamError: bad namespace prefix";

		case StreamErrorConflict:
			return "StreamError: conflict";

		case StreamErrorConnectionTimeout:
			return "StreamError: connection timeout";

		case StreamErrorHostGone:
			return "StreamError: host gone";

		case StreamErrorHostUnknown:
			return "StreamError: host unknown";

		case StreamErrorImproperAddressing:
			return "StreamError: improper addressing";

		case StreamErrorInternalServerError:
			return "StreamError: internal server error";

		case StreamErrorInvalidFrom:
			return "StreamError: invalid from";

		case StreamErrorInvalidId:
			return "StreamError: invalid id";

		case StreamErrorInvalidNamespace:
			return "StreamError: invalid namespace";

		case StreamErrorInvalidXml:
			return "StreamError: invalid xml";

		case StreamErrorNotAuthorized:
			return "StreamError: not authorized";

		case StreamErrorPolicyViolation:
			return "StreamError: policy violation";

		case StreamErrorRemoteConnectionFailed:
			return "StreamError: remote connection failed";

		case StreamErrorResourceConstraint:
			return "StreamError: resource constraint";

		case StreamErrorRestrictedXml:
			return "StreamError: restricted xml";

		case StreamErrorSeeOtherHost:
			return "StreamError: see other host";

		case StreamErrorSystemShutdown:
			return "StreamError: system shutdown";

		case StreamErrorUndefinedCondition:
			return "StreamError: undefined condition";

		case StreamErrorUnsupportedEncoding:
			return "StreamError: unsupported encoding";

		case StreamErrorUnsupportedStanzaType:
			return "StreamError: unsupported stanza type";

		case StreamErrorUnsupportedVersion:
			return "StreamError: unsupported version";

		case StreamErrorXmlNotWellFormed:
			return "StreamError: xml not well formed";

		case StreamErrorUndefined:
			return "StreamError: undefined";

		default:
			return "StreamError: unknown";
	}
}

void Messenger::onDisconnect( ConnectionError e ) {
	if( debug ) cerr << __PRETTY_FUNCTION__  << ": disconnected: " << e << endl;

	if( e != ConnUserDisconnected && e != ConnNoError ) {
		result = e;
		switch( e ) {
			case ConnStreamError:
				cerr << "ConnectionError: " << get_stream_error( client->streamError() ) << endl;
				break;

			case ConnStreamVersionError:
				cerr << "ConnectionError: stream version error"  << endl;
				break;

			case ConnStreamClosed:
				cerr << "ConnectionError: stream closed"  << endl;
				break;

			case ConnProxyAuthRequired:
				cerr << "ConnectionError: proxy auth required"  << endl;
				break;

			case ConnProxyAuthFailed:
				cerr << "ConnectionError: proxy auth failed"  << endl;
				break;

			case ConnProxyNoSupportedAuth:
				cerr << "ConnectionError: proxy no supported auth"  << endl;
				break;

			case ConnIoError:
				cerr << "ConnectionError: io error"  << endl;
				break;

			case ConnParseError:
				cerr << "ConnectionError: parse error"  << endl;
				break;

			case ConnConnectionRefused:
				cerr << "ConnectionError: connection refused"  << endl;
				break;

			case ConnDnsError:
				cerr << "ConnectionError: dns error"  << endl;
				break;

			case ConnOutOfMemory:
				cerr << "ConnectionError: out of memory"  << endl;
				break;

			case ConnNoSupportedAuth:
				cerr << "ConnectionError: no supported auth"  << endl;
				break;

			case ConnTlsFailed:
				cerr << "ConnectionError: tls failed"  << endl;
				break;

			case ConnTlsNotAvailable:
				cerr << "ConnectionError: tls not available"  << endl;
				break;

			case ConnCompressionFailed:
				cerr << "ConnectionError: compression failed"  << endl;
				break;

			case ConnAuthenticationFailed:
				cerr << "ConnectionError: authentication failed" << endl;
				break;

			case ConnNotConnected:
				cerr << "ConnectionError: not connected" << endl;
				break;

			default:
				break;
		}
	}
}

void Messenger::remove_muc_sender( const string& room ) {
	JID room_jid( room );
	auto& bare = room_jid.bare();
	{
		std::lock_guard<std::mutex> lck( this->map_guard );
		muc_senders.erase( bare );

		if( muc_senders.empty() )  client->disconnect();
	}
}

void Messenger::handleMessage( const gloox::Message& msg, gloox::MessageSession* session ) {
	if( debug ) cerr << __PRETTY_FUNCTION__ << endl;

	if( msg.body() == client->jid().resource() ) {
		if( debug ) cerr << __PRETTY_FUNCTION__ << " remove muc_sender " << msg.from().bare() << endl;

		remove_muc_sender( msg.from().bare() );

		if( debug ) cerr << __PRETTY_FUNCTION__ << " muc_sender " << msg.from().bare() << " removed" << endl;
	}
}

void Messenger::handleLog( gloox::LogLevel level, gloox::LogArea area, const std::string& message ) {
// 	cerr << "=> " << get_log_level( level ) << ":" << get_log_area( area ) << " '" << message << "'" << endl;
}

const char* get_log_area( LogArea area ) {
	switch( area ) {
		case LogAreaClassParser: /**< Log messages from Parser. */
			return "Parser";

		case LogAreaClassConnectionTCPBase : /**< Log messages from ConnectionTCPBase. */
			return "ConnectionTCPBase";

		case LogAreaClassClient : /**< Log messages from Client. */
			return "Client";

		case LogAreaClassClientbase : /**< Log messages from ClientBase. */
			return "Clientbase";

		case LogAreaClassComponent : /**< Log messages from Component. */
			return "Component";

		case LogAreaClassDns : /**< Log messages from DNS. */
			return "Dns";

		case LogAreaClassConnectionHTTPProxy : /**< Log messages from ConnectionHTTPProxy */
			return "ConnectionHTTPProxy";

		case LogAreaClassConnectionSOCKS5Proxy : /**< Log messages from ConnectionSOCKS5Proxy */
			return "ConnectionSOCKS5Proxy";

		case LogAreaClassConnectionTCPClient : /**< Log messages from ConnectionTCPClient. */
			return "ConnectionTCPClient";

		case LogAreaClassConnectionTCPServer : /**< Log messages from ConnectionTCPServer. */
			return "ConnectionTCPServer";

		case LogAreaClassS5BManager : /**< Log messages from SOCKS5BytestreamManager. */
			return "S5BManager";

		case LogAreaClassSOCKS5Bytestream : /**< Log messages from SOCKS5Bytestream. */
			return "SOCKS5Bytestream";

		case LogAreaClassConnectionBOSH : /**< Log messages from ConnectionBOSH */
			return "ConnectionBOSH";

		case LogAreaClassConnectionTLS : /**< Log messages from ConnectionTLS */
			return "ConnectionTLS";

		case LogAreaLinkLocalManager : /**< Log messages from LinkLocalManager */
			return "LinkLocalManager";

		case LogAreaAllClasses : /**< All log messages from all the classes. */
			return "AllClasses";

		case LogAreaXmlIncoming : /**< Incoming XML. */
			return "XmlIncoming";

		case LogAreaXmlOutgoing : /**< Outgoing XML. */
			return "XmlOutgoing";

		case LogAreaUser : /**< User-defined sources. */
			return "User";

		case LogAreaAll :  /**< All log sources. */
			return "All";

		default:
			return "";
	};
}

const char* get_log_level( LogLevel level ) {
	switch( level ) {
		case LogLevelDebug:   /**< Debug messages. */
			return "Debug";

		case LogLevelWarning: /**< Non-crititcal warning messages. */
			return "Warning";

		case LogLevelError:   /**< Critical, unrecoverable errors. */
			return "Error";

		default:
			return "";
	};
}
