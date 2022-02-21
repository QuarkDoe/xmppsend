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

#include <iostream>
#include <sstream>

#include <gloox/messagesession.h>

#include "mucsender.h"
#include "messenger.h"

using namespace std;
using namespace msg;

MUCSender::MUCSender( msg::Messenger* parent, const string& muc_jid ): parent( parent ) {

	if( parent->isDebug() ) clog << __PRETTY_FUNCTION__ << endl;

	auto client = parent->getClient();

	nick = make_unique<JID>( muc_jid );

	if( nick->resource().empty() ) {
		nick->setResource( client->jid().username() );
	}
}

MUCSender::~MUCSender() {
	if( parent->isDebug() ) clog << __PRETTY_FUNCTION__ << endl;
}

void msg::MUCSender::start() {
	if( parent->isDebug() ) clog << __PRETTY_FUNCTION__ << endl;

	room = make_unique<MUCRoom>( parent->getClient(), *nick, this );
	room->setRequestHistory( 0, MUCRoom::HistoryMaxStanzas );
	room->join();
}

void MUCSender::handleMUCError( gloox::MUCRoom* room, gloox::StanzaError error ) {
	if( parent->isDebug() ) clog << __PRETTY_FUNCTION__ << endl;

	cerr << "Can'nt join to " << nick->full() << endl;
	parent->remove_muc_sender( nick->full() ); //TODO: кажется как-то ну не очень правильно тут делаем.
}

void MUCSender::handleMUCInfo( gloox::MUCRoom* room, int features, const std::string& name, const gloox::DataForm* infoForm ) {
	if( parent->isDebug() ) clog << __PRETTY_FUNCTION__ << endl;
}

void MUCSender::handleMUCInviteDecline( gloox::MUCRoom* room, const gloox::JID& invitee, const std::string& reason ) {
	if( parent->isDebug() ) clog << __PRETTY_FUNCTION__ << endl;
}

void MUCSender::handleMUCItems( gloox::MUCRoom* room, const Disco::ItemList& items ) {
	if( parent->isDebug() ) clog << __PRETTY_FUNCTION__ << endl;
}

void MUCSender::handleMUCMessage( gloox::MUCRoom* room, const gloox::Message& msg, bool priv ) {
	if( parent->isDebug() ){
		clog << __PRETTY_FUNCTION__ << endl;
		clog << "\tfrom:\t" << msg.from().full() << endl;
		clog << "\tid:\t" << msg.id() << endl;
		clog << "\tsubj:\t" << msg.subject() << endl;
		clog << "\tbody:\t" << msg.body() << endl;
	}

	if( msg.body() == parent->getClient()->jid().resource() ){
		if( parent->isDebug() ) clog << __PRETTY_FUNCTION__ << ": got stop message" << endl;
		parent->remove_muc_sender( nick->full() );
	}

}

void MUCSender::handleMUCParticipantPresence( gloox::MUCRoom* room, const gloox::MUCRoomParticipant participant, const gloox::Presence& presence ) {
	if( parent->isDebug() ) {
		clog << __PRETTY_FUNCTION__ << endl;
		clog << "\t" << participant.nick->full() << endl;
	}

	if( presence.presence() == Presence::Available && participant.nick->full() == nick->full() ) {
		if( parent->isDebug() ) clog << __PRETTY_FUNCTION__  << ": joined to room:" << room->name() << " with affilation " << room->affiliation() << endl;

		room->send( parent->getMessage() );

		// send 'stop' message
		auto client = parent->getClient();
		auto session = new MessageSession( client, *nick );
		session->send( client->jid().resource() );
		client->disposeMessageSession( session );
	}
}

bool MUCSender::handleMUCRoomCreation( gloox::MUCRoom* room ) {
	if( parent->isDebug() ) clog << __PRETTY_FUNCTION__ << endl;
	return true;
}

void MUCSender::handleMUCSubject( gloox::MUCRoom* room, const std::string& nick, const std::string& subject ) {
	if( parent->isDebug() ) clog << __PRETTY_FUNCTION__ << endl;
}
