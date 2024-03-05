#include "PhysicalLayerPacket.h"

PhysicalLayerPacket::PhysicalLayerPacket(int _layer_ID, const string& _sender_MAC, const string& _receiver_MAC)
        : Packet(_layer_ID) {
    sender_MAC_address = _sender_MAC;
    receiver_MAC_address = _receiver_MAC;
    numberOfHops = 0;
    numberOfChunks = 0;
}

void PhysicalLayerPacket::print() {
    // TODO: Override the virtual print function from Packet class to additionally print layer-specific properties.
    std::cout << "Sender MAC address: " << sender_MAC_address << ", Receiver MAC address: "
              << receiver_MAC_address << endl;
}

PhysicalLayerPacket::~PhysicalLayerPacket() {
    // TODO: Free any dynamically allocated memory if necessary.
}

void PhysicalLayerPacket::setNumberOfChunks(int numberOfChunks) {
    PhysicalLayerPacket::numberOfChunks = numberOfChunks;
}

void PhysicalLayerPacket::setNumberOfHops(int numberOfHops) {
    PhysicalLayerPacket::numberOfHops = numberOfHops;
}

void PhysicalLayerPacket::setSenderMacAddress(const string &senderMacAddress) {
    sender_MAC_address = senderMacAddress;
}

void PhysicalLayerPacket::setReceiverMacAddress(const string &receiverMacAddress) {
    receiver_MAC_address = receiverMacAddress;
}
