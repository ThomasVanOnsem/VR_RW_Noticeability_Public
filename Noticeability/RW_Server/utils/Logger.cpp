#include "Logger.h"
#include "../config.h"


Logger::Logger(const std::string& name, const std::string& output_loc) {
    this->name = name;
    this->output_loc = output_loc;
}

Logger::~Logger() {
    std::map<std::string, std::ofstream>::iterator it;
    for (it = user_streams.begin(); it != user_streams.end(); it++){
        it->second.close();
    }
}

void Logger::log(const std::string& data, const std::string& user) {
    if(!config::logging){
        return;
    }
    if(user_streams.find(user) == user_streams.end()){
        newStream(user);
    }
    user_streams[user] << getTimeStr() + " | " + data + "\n";
    user_streams[user].flush();  //TODO less frequently?
}

void Logger::newStream(const std::string& user) {
    auto time_str = getTimeStr();
    user_streams[user].open(output_loc + user + "_" + time_str + ".txt");
}

std::string Logger::getTimeStr() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");
    return oss.str();
}