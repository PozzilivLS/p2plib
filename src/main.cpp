#include <iostream>
#include <vector>

#include "client/client.h"
#include "client/clientmaster.h"
#include "server/server.h"
#include "server/servermaster.h"

using namespace std;

int main() {
  int type;
  cin >> type;

  in_addr ip_to_num;

  std::vector<char> rBuff(1024);
  std::vector<char> sBuff(1024);

  if (type == 1) {
    ServerMaster serverMaster;
    serverMaster.startServer();
    // er = server.startConnection("127.0.0.1", 7000);
    while (true) {
      auto clients = serverMaster.getAllClients();

      if (clients.size() > 0) {
        auto msgs = serverMaster.getIncomingMessagesFrom(clients[0]);

        if (msgs.size() > 0) {
          for (auto &msg : msgs) {
            std::string s;
            msg >> s;

            std::cout << "Recieve: " << s << std::endl;
          }
        }
      }

      std::vector<char> outMsg(1024);
      fgets(outMsg.data(), 1024, stdin);

      Packet outPck;
      outPck << static_cast<int16_t>(Protocols::Message) << outMsg.data();

      serverMaster.sendMessage(outPck);
    }

  } else {
    ClientMaster clientMaster;
    clientMaster.startClient();
    while (true) {
      auto msgs = clientMaster.getIncomingMessages();

      if (msgs.size() > 0) {
        for (auto &msg : msgs) {
          std::string s;
          msg >> s;

          std::cout << "Recieve: " << s << std::endl;
        }
      }

      if (clientMaster.isConnected()) {
        std::vector<char> outMsg(1024);
        fgets(outMsg.data(), 1024, stdin);

        Packet outPck;
        outPck << static_cast<int16_t>(Protocols::Message) << outMsg.data();

        clientMaster.sendMessage(outPck);
      }
    }
    // er = client.startConnection("127.0.0.1", 7000);

    /*while (er == 0) {
      std::cout << "Your msg: ";
      fgets(sBuff.data(), sBuff.size(), stdin);
      er = client.sendMsg(sBuff);
    }*/
  }

  return 0;
}
