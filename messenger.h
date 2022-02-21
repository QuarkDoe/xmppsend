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

#ifndef MSG_MESSENGER_H
#define MSG_MESSENGER_H

#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include <gloox/message.h>
#include <gloox/client.h>
#include <gloox/messagehandler.h>
#include <gloox/connectionlistener.h>
#include <gloox/presencehandler.h>
#include <gloox/loghandler.h>

#include "mucsender.h"

namespace msg {

using namespace std;
using namespace gloox;

struct Destination {

	/**
	* Describes the different destination types.
	*/
	enum Type {
		Normal         = Message::Normal,        /**< A normal message. */
		Groupchat      = Message::Groupchat,     /**< A groupchat message. */
	};

	Type type;
	string jid;
};

/**
* @todo write docs
*/
class Messenger: public ConnectionListener, MessageHandler, LogHandler {

public:
	/**
	* @todo write docs
	*/
	Messenger( const std::string&, const std::string& );

	/**
	* @todo write docs
	*/
	~Messenger();

	/**
	* @todo write docs
	*/
	void setServer( const string& server ) {
		client->setServer( server );
	}

	/**
	* @todo write docs
	*/
	void setPort( int port ) {
		client->setPort( port );
	}

	/**
	* @todo write docs
	*/
	void setTls( TLSPolicy tls ) {
		client->setTls( tls );
	}

	/**
	* @todo write docs
	*/
	void setResource( const string& r ) {
		client->setResource( r );
	}

	/**
	* @todo write docs
	*/
	void setMessage( const string& m ) {
		message = m;
	}

	/**
	* @todo write docs
	*/
	void setLevel( LogLevel l ) {
		logLevel = l;
	}

	/**
	* @todo write docs
	*/
	void addDestination( const Destination& d ) {
		destinations.push_back( d );
	}

	/**
	* @todo write docs
	*/
	void setDestinations( const vector<Destination>& src ) {
		destinations = src;
	};

	/**
	* @todo write docs
	*/
	int start();

	/**
	* @todo write docs
	*/
	void setDebug( bool v ) {
		debug = v;
	};

	/**
	* @todo write docs
	*/
	virtual void onConnect() override;

	/**
	* @todo write docs
	*/
	virtual void onDisconnect( ConnectionError ) override;

	/**
	* @todo write docs
	*/
	virtual bool onTLSConnect( const CertInfo& ) override;

	/**
	* @todo write docs
	*/
	virtual void handleMessage( const gloox::Message& msg, gloox::MessageSession* session ) override;

	/**
	* @todo write docs
	*/
	virtual void handleLog( LogLevel, LogArea, const std::string& ) override;

	Client* getClient() {
		return client.get();
	};

	const string& getMessage() {
		return message;
	}

	bool isDebug() {
		return debug;
	}

	void remove_muc_sender( const string& );

private:
	unique_ptr<Client> client;
	unique_ptr<JID> jid;
	string pwd;
	vector<Destination> destinations;
	string message;
	bool debug;
	int result;

	std::map< string, shared_ptr<MUCSender> > muc_senders;
	std::mutex map_guard;

	LogLevel logLevel;

	void sendToRoom( const string& );
	void sendTo( const string& );
};

}

#endif // MSG_MESSENGER_H
