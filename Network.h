#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <iostream>
#include "Packet.h"
#include "Client.h"

using namespace std;

class Network {
public:
    Network();
    ~Network();

    // Executes commands given as a vector of strings while utilizing the remaining arguments.
    void process_commands(vector<Client> &clients, vector<string> &commands, int message_limit, const string &sender_port,
                     const string &receiver_port);

    // Initialize the network from the input files.
    vector<Client> read_clients(string const &filename);
    void read_routing_tables(vector<Client> & clients, string const &filename);
    vector<string> read_commands(const string &filename);

    vector<Client> * network_clients = nullptr;

    int find_by_id(vector<Client> & clients, string client_id);

    int find_by_mac(vector<Client> & clients, string client_mac);

    void message_command(vector<Client> & clients, string message, int message_limit, string sender_id,
                         string receiver_id, string sender_port, string receiver_port);

    void show_frame(vector<Client> & clients, string client_id, string q_type, int frame_num);

    void show_q(vector<Client> & clients, string client_id, string q_type);

    void send(vector<Client> & clients);

    void receive(vector<Client> & clients);

    static string getCurrentTimestamp();

    void print_log(Client client);
};

#endif  // NETWORK_H
