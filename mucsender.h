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

#ifndef MUCSENDER_H
#define MUCSENDER_H

#include <string>
#include <memory>
#include <gloox/mucroom.h>
#include <gloox/mucroomhandler.h>

namespace msg {

using namespace std;
using namespace gloox;

class Messenger;

/**
 * @todo write docs
 */
class MUCSender: public MUCRoomHandler {
public:
	MUCSender( Messenger* parent, const string& muc_jid );

	~MUCSender();

	void start();

	void handleMUCError( gloox::MUCRoom* room, gloox::StanzaError error ) override ;
	void handleMUCInfo( gloox::MUCRoom* room, int features, const std::string& name, const gloox::DataForm* infoForm ) override ;
	void handleMUCInviteDecline( gloox::MUCRoom* room, const gloox::JID& invitee, const std::string& reason ) override ;
	void handleMUCItems( gloox::MUCRoom* room, const Disco::ItemList& items ) override ;
	void handleMUCMessage( gloox::MUCRoom* room, const gloox::Message& msg, bool priv ) override ;
	void handleMUCParticipantPresence( gloox::MUCRoom* room, const gloox::MUCRoomParticipant participant, const gloox::Presence& presence ) override ;
	bool handleMUCRoomCreation( gloox::MUCRoom* room ) override ;
	void handleMUCSubject( gloox::MUCRoom* room, const std::string& nick, const std::string& subject ) override;

private:
	Messenger* parent;
	unique_ptr<JID> nick;
	unique_ptr<MUCRoom> room;
};

}

#endif // MUCSENDER_H
