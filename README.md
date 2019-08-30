# xmppsend
Simple tool to send xmpp messages from command line.

## Using:
xmppsend [options] 

## Options:
    -h, --help                          Help message.
    -v, --verbose                       Be verbose and print debug information.
    -c, --config <file>                 Configuration file. Default is ~/.config/xmppsend.cfg
    -t, --root <config root>            Configuration root. Default is 'xmpp'.
    -j, --jid <user JID>                Use JID instead of the one in the configuration file.
    -P, --password <user password>      Use password instead of the one in the configuration file.
    -s, --server <server addr>          Alternative host if it is different from the server part of your JID.
    -p, --port <port>                   Alternative port.
    -m, --message <message file>        Use supplied file as source of message instead of reading from stdin.
    -a, --chatroom <jid>                Add chatroom destination.
    -d, --destination <jid>             Add normal destination.

## Example config:

```
xmpp = {
	jid = "test@localhost"; // mandatory parameter
	password = "testpwd";   // mandatory parameter
	server = "localhost"; // optional parameter
	port = 5222;          // optional parameter
};

```
