//
// Created by alperen on 2.10.2023.
//

#include "Log.h"

Log::Log(const string &_timestamp, const string &_message, int _number_of_frames, int _number_of_hops, const string &_sender_id,
         const string &_receiver_id, bool _success, ActivityType _type) {
    timestamp = _timestamp;
    message_content = _message;
    number_of_frames = _number_of_frames;
    number_of_hops = _number_of_hops;
    sender_id = _sender_id;
    receiver_id = _receiver_id;
    success_status = _success;
    activity_type = _type;
}

Log::~Log() {
    // TODO: Free any dynamically allocated memory if necessary.
}

void Log::print() {
    string activity_text;
    switch (activity_type) {
        case ActivityType::MESSAGE_SENT:
            activity_text = "Message Sent";
            break;
        case ActivityType::MESSAGE_FORWARDED:
            activity_text = "Message Forwarded";
            break;
        case ActivityType::MESSAGE_DROPPED:
            activity_text = "Message Dropped";
            break;
        case ActivityType::MESSAGE_RECEIVED:
            activity_text = "Message Received";
            break;
        default:
            activity_text = "UNKNOWN";
    }

    cout << "Activity: " << activity_text << "\nTimestamp: " << timestamp << "\nNumber of frames: " <<
    number_of_frames << "\nNumber of hops: " << number_of_hops << "\nSender ID: " << sender_id << "\nReceiver ID: " <<
    receiver_id << "\nSuccess: " << (success_status ? "Yes\n" : "No\n") <<
    (message_content.length() > 0 ? "Message: \"" + message_content + "\"\n" : "");
}