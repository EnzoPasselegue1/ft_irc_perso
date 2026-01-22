import socket
import unittest
import time

# Configuration
HOST = "127.0.0.1"
PORT = 6667
PASSWORD = "pass"

class IRCTestClient:
    def __init__(self, nick, user="user"):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.nick = nick
        self.user = user
        self.buffer = ""

    def connect(self):
        self.sock.connect((HOST, PORT))
        self.sock.settimeout(0.5)
        self.send(f"PASS {PASSWORD}")
        self.send(f"NICK {self.nick}")
        self.send(f"USER {self.user} 0 * :Realname")

    def send(self, msg):
        self.sock.send((msg + "\r\n").encode())

    def receive(self):
        try:
            data = self.sock.recv(4096).decode()
            self.buffer += data
            return data
        except:
            return ""

    def wait_for(self, code, timeout=1.5):
        start = time.time()
        while time.time() - start < timeout:
            self.receive()
            if f" {code} " in self.buffer:
                lines = self.buffer.split("\r\n")
                for l in lines:
                    if f" {code} " in l:
                        self.buffer = "\r\n".join([x for x in lines if x != l])
                        return l
            time.sleep(0.05)
        return None

    def close(self):
        self.sock.close()

class TestIRCRFCFull(unittest.TestCase):
    def setUp(self):
        self.clients = []

    def tearDown(self):
        for c in self.clients:
            c.close()

    def create_client(self, nick, user="u"):
        c = IRCTestClient(nick, user)
        c.connect()
        self.clients.append(c)
        return c

    # --- AUTHENTICATION (PASS, NICK, USER, QUIT) ---
    def test_auth_full(self):
        c = self.create_client("AuthUser")
        self.assertIsNotNone(c.wait_for("001"), "RPL_WELCOME (001) missing")
        c.send("PASS dummy")
        self.assertIsNotNone(c.wait_for("462"), "ERR_ALREADYREGISTRED (462) expected")
        c.send("NICK AuthUser") # Same nick
        self.assertIsNotNone(c.wait_for("433"), "ERR_NICKNAMEINUSE (433) expected")
        c.send("QUIT :Bye")

    # --- CHANNELS (JOIN, PART, TOPIC, NAMES) ---
    def test_channel_basics(self):
        c1 = self.create_client("ChanUser1")
        # JOIN multiple
        c1.send("JOIN #chan1,#chan2")
        self.assertIsNotNone(c1.wait_for("366"), "NAMES list end missing for #chan1")
        self.assertIsNotNone(c1.wait_for("366"), "NAMES list end missing for #chan2")

        # NAMES
        c1.send("NAMES #chan1")
        self.assertIn("ChanUser1", c1.wait_for("353"), "RPL_NAMREPLY (353) should have nick")

        # TOPIC
        c1.send("TOPIC #chan1 :New Topic")
        self.assertIsNotNone(c1.wait_for("TOPIC"), "Topic change notification missing")
        c1.send("TOPIC #chan1")
        self.assertIn("New Topic", c1.wait_for("332"), "RPL_TOPIC (332) mismatch")

        # PART multiple
        c1.send("PART #chan1,#chan2 :Leaving")
        self.assertIsNotNone(c1.wait_for("PART"), "PART notification missing for #chan1")
        self.assertIsNotNone(c1.wait_for("PART"), "PART notification missing for #chan2")

    # --- CHANNEL MODES & KICK ---
    def test_modes_and_kick(self):
        admin = self.create_client("Admin")
        admin.send("JOIN #modetest")
        admin.wait_for("366")

        user = self.create_client("User")
        user.send("JOIN #modetest")
        user.wait_for("366")

        # KICK error (not admin)
        user.send("KICK #modetest Admin :Try")
        self.assertIsNotNone(user.wait_for("482"), "ERR_CHANOPRIVSNEEDED (482) expected")

        # MODE Invite-only
        admin.send("MODE #modetest +i")
        self.assertIsNotNone(admin.wait_for("MODE"), "MODE +i broadcast missing")

        # JOIN error (Invite only)
        c3 = self.create_client("Stranger")
        c3.send("JOIN #modetest")
        self.assertIsNotNone(c3.wait_for("473"), "ERR_INVITEONLYCHAN (473) expected")

        # KICK success
        admin.send("KICK #modetest User :Out")
        self.assertIsNotNone(admin.wait_for("KICK"), "KICK broadcast missing")

    # --- MESSAGING (PRIVMSG, NOTICE, INVITE) ---
    def test_messaging_and_invite(self):
        c1 = self.create_client("Sender")
        c2 = self.create_client("Receiver")
        c1.send("JOIN #msgchan")
        c1.wait_for("366")

        # PRIVMSG success
        c1.send("PRIVMSG Receiver :Hello Private")
        self.assertIn("Hello Private", c2.wait_for("PRIVMSG"), "Private message not received")

        # NOTICE success
        c1.send("NOTICE Receiver :Hello Notice")
        self.assertIn("Hello Notice", c2.wait_for("NOTICE"), "Notice not received")

        # INVITE
        c1.send("INVITE Receiver #msgchan")
        self.assertIsNotNone(c1.wait_for("341"), "RPL_INVITING (341) missing")
        self.assertIn("INVITE", c2.wait_for("INVITE"), "Invite notification missing for target")

    # --- QUERIES (LIST, WHOIS, WHO, PING) ---
    def test_queries(self):
        c = self.create_client("QueryUser")
        c.send("JOIN #listchan")
        c.wait_for("366")

        # LIST
        c.send("LIST")
        self.assertIn("#listchan", c.wait_for("322"), "RPL_LIST (322) missing channel")
        self.assertIsNotNone(c.wait_for("323"), "RPL_LISTEND (323) missing")

        # WHOIS
        c.send("WHOIS QueryUser")
        self.assertIn("QueryUser", c.wait_for("311"), "RPL_WHOISUSER (311) mismatch")
        self.assertIsNotNone(c.wait_for("318"), "RPL_ENDOFWHOIS (318) missing")

        # WHO
        c.send("WHO #listchan")
        self.assertIn("QueryUser", c.wait_for("352"), "RPL_WHOREPLY (352) missing")
        self.assertIsNotNone(c.wait_for("315"), "RPL_ENDOFWHO (315) missing")

        # PING
        c.send("PING mytoken")
        self.assertIn("PONG", c.wait_for("PONG"), "PONG reply missing")

    # --- STRESS TEST ---
    def test_stress_load(self):
        count = 100
        print(f"\n[Stress] Connecting {count} users...")
        s_list = []
        for i in range(count):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect((HOST, PORT))
                s.send(f"PASS {PASSWORD}\r\nNICK Bot{i}\r\nUSER b{i} 0 * :b\r\n".encode())
                s_list.append(s)
            except: pass
        print(f"[Stress] {len(s_list)} successfully connected.")
        for s in s_list: s.close()

if __name__ == "__main__":
    unittest.main()