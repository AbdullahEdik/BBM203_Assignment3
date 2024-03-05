#include "Network.h"

Network::Network() {

}

void Network::process_commands(vector<Client> &clients, vector<string> &commands, int message_limit,
                      const string &sender_port, const string &receiver_port) {
    // TODO: Execute the commands given as a vector of strings while utilizing the remaining arguments.
    /* Don't use any static variables, assume this method will be called over and over during testing.
     Don't forget to update the necessary member variables after processing each command. For example,
     after the MESSAGE command, the outgoing queue of the sender must have the expected frames ready to send. */
    network_clients = &clients;
    for (const auto & command : commands) {
        //Print the command.
        string command_x = "Command: " + command;
        int length = (int) command_x.length();
        string z;
        for (int j = 0; j < length; ++j) {
            z += "-";
        }
        std::cout << z <<"\n" << command_x << "\n" << z << endl;
        if (command.substr(0, 7) == "MESSAGE"){
            string message;
            for (int i = command.length() - 2; i >= 0 ; --i) {
                if (command[i] == '#'){
                    message = command.substr(i+1);
                    message = message.substr(0, message.length()-1);
                }
            }
            istringstream iss(command);
            vector<string> partsOfMessage;
            string part;
            while (getline(iss, part, ' ')){
                partsOfMessage.push_back(part);
            }
            cout << "Message to be sent: \"" + message + "\"\n" << endl;
            message_command(clients, message, message_limit, partsOfMessage[1], partsOfMessage[2],
                            sender_port, receiver_port);

        }
        else if (command.substr(0, 15) == "SHOW_FRAME_INFO"){
            istringstream iss(command);
            vector<string> partsOfCommand;
            string part;
            while (getline(iss, part, ' ')){
                partsOfCommand.push_back(part);
            }

            show_frame(clients, partsOfCommand[1], partsOfCommand[2],
                       stoi(partsOfCommand[3]));
        }
        else if (command.substr(0, 11) == "SHOW_Q_INFO"){
            istringstream iss(command);
            vector<string> partsOfCommand;
            string part;
            while (getline(iss, part, ' ')){
                partsOfCommand.push_back(part);
            }

            show_q(clients, partsOfCommand[1], partsOfCommand[2]);
        }
        else if (command.substr(0, 4) == "SEND"){
            send(clients);
        }
        else if (command.substr(0, 7) == "RECEIVE"){
            receive(clients);
        }
        else if (command.substr(0, 9) == "PRINT_LOG"){
            string client_id = command.substr(10);
            print_log(clients[find_by_id(clients, client_id)]);
        }
        else{
            cout << "Invalid command." << endl;
        }
    }

}

vector<Client> Network::read_clients(const string &filename) {
    vector<Client> clients;
    // TODO: Read clients from the given input file and return a vector of Client instances.
    ifstream file(filename);
    string line;
    getline(file, line);
    while(getline(file, line)){
        istringstream iss(line);
        vector<string> client_info;
        string info;
        while (getline(iss, info, ' ')) {
            client_info.push_back(info);
        }
        Client client(client_info[0], client_info[1], client_info[2]);
        clients.push_back(client);
    }
    file.close();
    return clients;
}

void Network::read_routing_tables(vector<Client> &clients, const string &filename) {
    // TODO: Read the routing tables from the given input file and populate the clients' routing_table member variable.
    ifstream file(filename);
    vector<vector<string>> routing_tables;
    string line;
    routing_tables.emplace_back();
    while(getline(file, line)){
        if(line == "-"){
            routing_tables.emplace_back();
        }
        else{
            routing_tables.back().push_back(line);
        }
    }
    file.close();
    for (int i = 0; i < routing_tables.size(); ++i) {
        for (int j = 0; j < routing_tables[i].size(); ++j) {
            istringstream iss(routing_tables[i][j]);
            vector<string> route;
            string id;
            while (getline(iss, id, ' ')) {
                route.push_back(id);
            }
            string receiverID = route[0];
            string nextHopID = route[1];
            clients[i].routing_table[receiverID] = nextHopID;
        }
    }
}

// Returns a list of token lists for each command
vector<string> Network::read_commands(const string &filename) {
    vector<string> commands;
    // TODO: Read commands from the given input file and return them as a vector of strings.
    ifstream file(filename);
    string line;
    getline(file, line);
    while(getline(file, line)){
        commands.push_back(line);
    }
    file.close();
    return commands;
}

int Network::find_by_id(vector<Client> &clients, std::string client_id) {
    for (int i = 0; i < clients.size(); ++i) {
        if(clients[i].client_id == client_id){
            return i;
        }
    }
    return -1;
}

int Network::find_by_mac(vector<Client> &clients, std::string client_mac) {
    for (int i = 0; i < clients.size(); ++i) {
        if(clients[i].client_mac == client_mac){
            return i;
        }
    }
    return -1;
}

void Network::message_command(vector<Client> &clients, std::string message, int message_limit, std::string sender_id,
                      std::string receiver_id, string sender_port, string receiver_port) {
    int numOfChunks = (int)((message.length())/message_limit);
    if((int)message.length() % message_limit != 0){
        numOfChunks += 1;
    }
    string original_message = message;
    vector<string> message_chunks;
    message_chunks.push_back(message.substr(0, message_limit));
    while(message.length()> message_limit){
        message = message.substr(message_limit);
        message_chunks.push_back(message.substr(0, message_limit));
    }

    int sender_index = find_by_id(clients, sender_id);
    int receiver_index = find_by_id(clients, receiver_id);
    int next_hop_index = find_by_id(clients, clients[sender_index].routing_table[receiver_id]);

    for (int i = 0; i < message_chunks.size(); ++i) {
        stack<Packet*> frame;
        frame.push(new ApplicationLayerPacket(0, sender_id, receiver_id,message_chunks[i]));
        frame.push(new TransportLayerPacket(1, sender_port, receiver_port));
        frame.push(new NetworkLayerPacket(2, clients[sender_index].client_ip,
                                          clients[receiver_index].client_ip));
        frame.push( new PhysicalLayerPacket(
                3,clients[sender_index].client_mac,
                clients[next_hop_index].client_mac));
        PhysicalLayerPacket * physicalPacket = dynamic_cast<PhysicalLayerPacket*>(frame.top());
        physicalPacket->setNumberOfChunks(numOfChunks);
        clients[sender_index].outgoing_queue.push(frame);
        cout << "Frame #" << i+1 << endl;
        stack<Packet*> frame2 = frame;
        while(!frame2.empty()){
            frame2.top()->print();
            frame2.pop();
        }
        cout << "Message chunk carried: \"" + message_chunks[i] << "\"" << endl;
        cout << "Number of hops so far: " << physicalPacket->numberOfHops << "\n--------" << endl;
    }
    Log client_log(getCurrentTimestamp(), original_message, numOfChunks, 0,
                   sender_id, receiver_id, true, ActivityType::MESSAGE_SENT);
    clients[sender_index].log_entries.push_back(client_log);


}

void Network::show_frame(vector<Client> &clients, std::string client_id, std::string q_type, int frame_num) {
    queue<stack<Packet*>> client_queue;
    string queue_text;
    int client_index = find_by_id(clients, client_id);
    if(q_type == "in"){
        client_queue = clients[client_index].incoming_queue;
        queue_text = " on the incoming queue of client ";
    }
    else{
        client_queue = clients[client_index].outgoing_queue;
        queue_text = " on the outgoing queue of client ";
    }
    stack<Packet*> frame;
    int n = 1;
    while(!client_queue.empty()){
        if(n == frame_num){
            frame = client_queue.front();
            break;
        }
        client_queue.pop();
        ++n;
    }
    if(!frame.empty()){
        cout << "Current Frame #" << frame_num << queue_text << client_id << endl;
        PhysicalLayerPacket * physicalLayerPacket = dynamic_cast<PhysicalLayerPacket*>(frame.top());
        frame.pop();
        NetworkLayerPacket * networkLayerPacket = dynamic_cast<NetworkLayerPacket*>(frame.top());
        frame.pop();
        TransportLayerPacket * transportLayerPacket = dynamic_cast<TransportLayerPacket*>(frame.top());
        frame.pop();
        ApplicationLayerPacket * applicationLayerPacket = dynamic_cast<ApplicationLayerPacket*>(frame.top());
        cout << "Carried Message: \"" << applicationLayerPacket->message_data << "\"\n";
        cout << "Layer 0 info: "; applicationLayerPacket->print();
        cout << "Layer 1 info: "; transportLayerPacket->print();
        cout << "Layer 2 info: "; networkLayerPacket->print();
        cout << "Layer 3 info: "; physicalLayerPacket->print();
        cout << "Number of hops so far: " << physicalLayerPacket->numberOfHops << endl;
    }
    else{
        cout << "No such frame." << endl;
    }
}

void Network::show_q(vector<Client> &clients, std::string client_id, std::string q_type) {
    queue<stack<Packet*>> client_queue;
    string queue_text;
    if(q_type == "in"){
        client_queue = clients[find_by_id(clients, client_id)].incoming_queue;
        queue_text = " Incoming Queue Status";
    }
    else{
        client_queue = clients[find_by_id(clients, client_id)].outgoing_queue;
        queue_text = " Outgoing Queue Status";
    }
    int numOfFrames = 0;
    while(!client_queue.empty()){
        client_queue.pop();
        ++numOfFrames;
    }

    cout << "Client " << client_id << queue_text << endl;
    cout << "Current total number of frames: " << numOfFrames << endl;
}

void Network::send(vector<Client> &clients) {
    for (auto & client : clients) {
        int frame_num = 1;
        while(!client.outgoing_queue.empty()){
            stack<Packet*> frame = client.outgoing_queue.front();
            PhysicalLayerPacket * physicalPacket = dynamic_cast<PhysicalLayerPacket*>(frame.top());
            frame.pop();
            NetworkLayerPacket * networkPacket = dynamic_cast<NetworkLayerPacket*>(frame.top());
            frame.pop();
            TransportLayerPacket * transportPacket = dynamic_cast<TransportLayerPacket*>(frame.top());
            frame.pop();
            ApplicationLayerPacket * applicationPacket = dynamic_cast<ApplicationLayerPacket*>(frame.top());
            frame.pop();
            int receiver_index = find_by_mac(clients, physicalPacket->receiver_MAC_address);
            int sender_index = find_by_mac(clients, physicalPacket->sender_MAC_address);
            cout << "Client "<< clients[sender_index].client_id <<" sending frame #"<< frame_num <<" to client "
                 << clients[receiver_index].client_id << endl;
            physicalPacket->print();
            networkPacket->print();
            transportPacket->print();
            applicationPacket->print();
            physicalPacket->setNumberOfHops(physicalPacket->numberOfHops + 1);
            cout << "Message chunk carried: \"" << applicationPacket->message_data << "\"\n";
            cout << "Number of hops so far: " << physicalPacket->numberOfHops << endl;
            cout << "--------" << endl;

            clients[receiver_index].incoming_queue.push(client.outgoing_queue.front());
            client.outgoing_queue.pop();

            if(frame_num == physicalPacket->numberOfChunks){
                frame_num = 1;
            }
            else{
                ++frame_num;
            }
        }
    }
}

void Network::receive(vector<Client> &clients) {
    for (auto & client : clients) {
        string message;
        int frame_num = 1;
        while(!client.incoming_queue.empty()){
            stack<Packet*> frame = client.incoming_queue.front();
            PhysicalLayerPacket * physicalPacket = dynamic_cast<PhysicalLayerPacket*>(frame.top());
            frame.pop();
            NetworkLayerPacket * networkPacket = dynamic_cast<NetworkLayerPacket*>(frame.top());
            frame.pop();
            TransportLayerPacket * transportPacket = dynamic_cast<TransportLayerPacket*>(frame.top());
            frame.pop();
            ApplicationLayerPacket * applicationPacket = dynamic_cast<ApplicationLayerPacket*>(frame.top());
            frame.pop();
            string sender_id = clients[find_by_mac(clients, physicalPacket->sender_MAC_address)].client_id;

            if(client.client_id == applicationPacket->receiver_ID){
                cout << "Client "<< client.client_id <<" receiving frame #" << frame_num << " from client " << sender_id
                << ", originating from client "<< applicationPacket->sender_ID << endl;
                physicalPacket->print();
                networkPacket->print();
                transportPacket->print();
                applicationPacket->print();
                cout << "Message chunk carried: \"" << applicationPacket->message_data << "\"\n";
                cout << "Number of hops so far: " << physicalPacket->numberOfHops << "\n--------" << endl;
                message += applicationPacket->message_data;
                if(frame_num == physicalPacket->numberOfChunks){
                    frame_num = 0;
                    cout << "Client " <<client.client_id << " received the message \"" << message <<"\" from client " <<
                    applicationPacket->sender_ID <<".\n--------" << endl;
                    Log client_log(getCurrentTimestamp(), message, physicalPacket->numberOfChunks,
                                   physicalPacket->numberOfHops, applicationPacket->sender_ID, applicationPacket->receiver_ID,true, ActivityType::MESSAGE_RECEIVED);
                    client.log_entries.push_back(client_log);
                    message = "";
                }
                ++frame_num;

                while(!client.incoming_queue.front().empty()){
                    Packet * packet = client.incoming_queue.front().top();
                    client.incoming_queue.front().pop();
                    delete packet;
                }
            }
            else if(find_by_id(clients, client.routing_table[applicationPacket->receiver_ID]) == -1){
                cout << "Client " << client.client_id << " receiving frame #" << frame_num << " from client " <<
                sender_id << ", but intended for client "<< applicationPacket->receiver_ID
                <<". Forwarding... " << endl;

                cout << "Error: Unreachable destination. Packets are dropped after " << physicalPacket->numberOfHops
                <<" hops!" << endl;
                if(frame_num == physicalPacket->numberOfChunks){
                    frame_num = 0;
                    cout << "--------" << endl;
                    Log client_log(getCurrentTimestamp(), "", physicalPacket->numberOfChunks,
                                   physicalPacket->numberOfHops, applicationPacket->sender_ID, applicationPacket->receiver_ID,false, ActivityType::MESSAGE_DROPPED);
                    client.log_entries.push_back(client_log);
                }
                ++frame_num;

                while(!client.incoming_queue.front().empty()){
                    Packet * packet = client.incoming_queue.front().top();
                    client.incoming_queue.front().pop();
                    delete packet;
                }
            }
            else{
                if(frame_num == 1){
                    cout << "Client " << client.client_id << " receiving a message from client " << sender_id
                    <<", but intended for client " << applicationPacket->receiver_ID << ". Forwarding... " << endl;
                }
                int nextHop_index = find_by_id(clients,
                                               client.routing_table[applicationPacket->receiver_ID]);
                physicalPacket->setSenderMacAddress(physicalPacket->receiver_MAC_address);
                physicalPacket->setReceiverMacAddress(clients[nextHop_index].client_mac);
                cout << "Frame #" << frame_num << " MAC address change: New sender MAC " <<
                physicalPacket->sender_MAC_address<< ", new receiver MAC " <<physicalPacket->receiver_MAC_address<<endl;
                if(frame_num == physicalPacket->numberOfChunks){
                    frame_num = 0;
                    cout << "--------" << endl;
                    Log client_log(getCurrentTimestamp(), "", physicalPacket->numberOfChunks,
                                   physicalPacket->numberOfHops, applicationPacket->sender_ID, applicationPacket->receiver_ID,true, ActivityType::MESSAGE_FORWARDED);
                    client.log_entries.push_back(client_log);
                }
                ++frame_num;
                client.outgoing_queue.push(client.incoming_queue.front());

            }
            client.incoming_queue.pop();
        }
    }
}

void Network::print_log(Client client) {
    if(!client.log_entries.empty()){
        cout << "Client " << client.client_id << " Logs:" << endl;
        for (int i = 0; i < client.log_entries.size(); ++i) {
            cout << "--------------\nLog Entry #" << i + 1 << ":" << endl;
            client.log_entries[i].print();
        }
    }
}

string Network::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&currentTime));
    return std::string(buffer);
}

Network::~Network() {
    // TODO: Free any dynamically allocated memory if necessary.
    if(network_clients != nullptr){
        for (auto &client: *network_clients) {
            while (!client.outgoing_queue.empty()) {
                while (!client.outgoing_queue.front().empty()) {
                    Packet *packet = client.outgoing_queue.front().top();
                    client.outgoing_queue.front().pop();
                    delete packet;
                }
                client.outgoing_queue.pop();
            }
            while (!client.incoming_queue.empty()) {
                while (!client.incoming_queue.front().empty()) {
                    Packet *packet = client.incoming_queue.front().top();
                    client.incoming_queue.front().pop();
                    delete packet;
                }
                client.incoming_queue.pop();
            }
        }
    }
}
