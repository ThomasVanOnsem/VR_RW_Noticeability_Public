#include "config.h"


namespace config
{
    int port = 5555;
    std::string indicator_border = "BORDER";
    std::string indicator_player_pos = "PLAYER_POS";
    std::string indicator_player_rot = "PLAYER_ROT";
    bool use_guardian = false;
    int room_size = 5;
    float message_delay = 255.71;
    bool use_improvements = true;
    bool logging = true;
    float standing_still_error = 0.001;
    int startup_counter = 5;
    int player_amount = 1;
    int msg_frequency = 50;
}

