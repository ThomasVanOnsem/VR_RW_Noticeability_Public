#ifndef RW_SERVER_CONFIG_H
#define RW_SERVER_CONFIG_H

#include <string>

namespace config
{
    /**
     * The port on which to listen for incoming data.
     */
    extern int port;
    /**
     * The headers used to distinguish different kinds of received data.
     */
    extern std::string indicator_border;
    extern std::string indicator_player_pos;
    extern std::string indicator_player_rot;
    /**
     * Get border data from game or use fixed border data.
     */
    extern bool use_guardian;
    /**
     * Size of the square playroom (in m) if guardian not used
     */
    extern int room_size;
    /**
     * Delay (in ms) of sending message from headset wireless to server. Used to estimate real-time user positions.
     */
    extern float message_delay;
    /**
     * Use different improvements to the algorithms like drift ignoring, extrapolation, reset countdown...
     */
    extern bool use_improvements;
    /**
     * Log all activity.
     */
    extern bool logging;
    /**
     * Positional error for determining whether user just turns head and stands still or is moving.
     */
    extern float standing_still_error;
    /**
     * Only start recommending after getting x amount of positional points.
     */
    extern int startup_counter;
    /**
     * Amount of expected players in the game (needed for different start positions).
     */
    extern int player_amount;
    /**
     * Frequency of messages being sent from headset (ms between every message).
     */
     extern int msg_frequency;
}
#endif //RW_SERVER_CONFIG_H
