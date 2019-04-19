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

#include <gloox/messagesession.h>

#include "mucsender.h"
#include "messenger.h"

using namespace std;
using namespace msg;

MUCSender::MUCSender( msg::Messenger* parent, const string& muc_jid ): parent( parent ) {

	if( parent->isDebug() ) cerr << __PRETTY_FUNCTION__ << endl;

	Client* client = parent->getClient();

	nick = new JID( muc_jid );

	if( nick->resource().empty() ) {
		nick->setResource( client->jid().username() );
	}
}

MUCSender::~MUCSender() {
	if( parent->isDebug() ) cerr << __PRETTY_FUNCTION__ << endl;

	delete room;
	delete nick;
}

void msg::MUCSender::start() {
	if( parent->isDebug() ) cerr << __PRETTY_FUNCTION__ << endl;

	room = new MUCRoom( parent->getClient(), *nick, this );
	room->setRequestHistory( 0, MUCRoom::HistoryMaxStanzas );
	room->join();
}

void MUCSender::handleMUCError( gloox::MUCRoom* room, gloox::StanzaError error ) {
	if( parent->isDebug() ) cerr << __PRETTY_FUNCTION__ << endl;

	cerr << "Can'nt join to " << nick->full() << endl;
	parent->remove_muc_sender( nick->full() ); //TODO: кажется как-то ну не очень правильно тут делаем.
}

void MUCSender::handleMUCInfo( gloox::MUCRoom* room, int features, const std::string& name, const gloox::DataForm* infoForm ) {
// 	if( parent->isDebug() ) cerr << __PRETTY_FUNCTION__ << endl;
}

void MUCSender::handleMUCInviteDecline( gloox::MUCRoom* room, const gloox::JID& invitee, const std::string& reason ) {
// 	if( parent->isDebug() ) cerr << __PRETTY_FUNCTION__ << endl;
}

void MUCSender::handleMUCItems( gloox::MUCRoom* room, const Disco::ItemList& items ) {
// 	if( parent->isDebug() ) cerr << __PRETTY_FUNCTION__ << endl;
}

void MUCSender::handleMUCMessage( gloox::MUCRoom* room, const gloox::Message& msg, bool priv ) {
// 	if( parent->isDebug() ) cerr << __PRETTY_FUNCTION__ << endl;
}

void MUCSender::handleMUCParticipantPresence( gloox::MUCRoom* room, const gloox::MUCRoomParticipant participant, const gloox::Presence& presence ) {
// 	if( parent->isDebug() ) cerr << __PRETTY_FUNCTION__ << endl;

	if( presence.presence() == Presence::Available && participant.nick->full() == nick->full() ) {
		if( parent->isDebug() ) cerr << __PRETTY_FUNCTION__  << ": joined to room:" << room->name() << " with affilation " << room->affiliation() << endl;

		room->send( parent->getMessage() );
		Client* client = parent->getClient();
		MessageSession* session = new MessageSession( client, *nick );
		session->send( client->jid().resource() );
		client->disposeMessageSession( session );
	}
}

bool MUCSender::handleMUCRoomCreation( gloox::MUCRoom* room ) {
// 	if( parent->isDebug() ) cerr << __PRETTY_FUNCTION__ << endl;
	return true;
}

void MUCSender::handleMUCSubject( gloox::MUCRoom* room, const std::string& nick, const std::string& subject ) {
// 	if( parent->isDebug() ) cerr << __PRETTY_FUNCTION__ << endl;
}
