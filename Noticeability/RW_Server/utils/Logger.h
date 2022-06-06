#ifndef RW_SERVER_LOGGER_H
#define RW_SERVER_LOGGER_H

#include <string>
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <map>

class Logger {
public:
    Logger(const std::string&, const std::string&);
    ~Logger();
    void log(const std::string&, const std::string& = "env");
private:
    void newStream(const std::string&);
    static std::string getTimeStr();
    std::string name;
    std::string output_loc;
    std::map<std::string,std::ofstream> user_streams;
};


#endif //RW_SERVER_LOGGER_H
