#ifndef RW_SERVER_GAME_H
#define RW_SERVER_GAME_H

#include <vector>
#include <map>
#include "Player.h"
#include "algorithms/RWAlgorithm.h"


class Game {
public:
    Game();
    ~Game() = default;
    /**
     * Adds a new player to the game.
     */
    void addPlayer(const std::string&);
    /**
     * Updates the player's position.
     */
    void updatePlayerPos(const std::string&, Coordinate);
    /**
     * Updates the player's rotation.
     */
    void updatePlayerRot(const std::string&, Coordinate);
    /**
     * Add border coordinates.
     */
    void addToBorder(Coordinate);
    /**
     * Get the complete border.
     * @return Vector of coordinates sequentially ordered to represent the border.
     */
    std::vector<Coordinate> getBorder();
    /**
     * Add an obstacle to the game.
     */
    void addObstacle(const std::string&, Coordinate);
    /**
     * Add a redirected walking algorithm used by the game.
     */
    void setRWAlgorithm(RWAlgorithm*);
    /**
     * Get the used RW algorithm.
     * @return An RW algorithm.
     */
    RWAlgorithm* getRWAlgorithm();
    /**
     * Gets all the users active in the game.
     * @return A map of all usernames and their Player object.
     */
    std::map<std::string, Player*> getUsers();
    Player* getPlayer(const std::string&);
    /**
     * Indicates the game was properly setup by looking at a proper amount of players and border points.
     * @return
     */
    bool setupCompleted() const;
    bool getEnvAdded() const;
    void setEnvAdded();
    std::vector<Coordinate> getOffsets();
private:
    /**
     * All the players active in the game.
     */
    std::map<std::string, Player*> players;
    /**
     * All the physical obstacles in the game.
     */
    std::map<std::string, Coordinate> obstacles;
    /**
     * The complete border of the game in sequential order.
     */
    std::vector<Coordinate> border;
    /**
     * Indicates whether the game is ready to be player.
     */
    bool env_added = false;
    /**
     * The RW algorithm used to steer users.
     */
    RWAlgorithm* rw_algorithm;
    /**
     * Collection of offsets for every player when starting with multiple players in 1 environment.
     */
    std::vector<Coordinate> player_offsets;
};


#endif //RW_SERVER_GAME_H
