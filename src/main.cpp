#include "./GameEngine/GameEngine.h"

std::vector<std::string> splitString(const std::string &str, char del);
Config readConfigFromFile(const std::string& filename);

int main() {

    GameEngine game(1280, 720, "COMP4300 Assignment 2", readConfigFromFile("./resources/config.txt"));
    game.run();

    return 0;
}

std::vector<std::string> splitString(const std::string &str, char del) {
    std::vector<std::string> words;
    int last = 0;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == del) {
            words.push_back(str.substr(last, i - last));
            last = i + 1;
        }
    }
    words.push_back(str.substr(last, str.length() - last));
    return words;
}

Config readConfigFromFile(const std::string& filename) {
    Config config;

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Cannot read config from file " << filename << std::endl;
        exit(-1);
    }

    std::string line;
    while (getline(file, line)) {
        std::vector<std::string> tokens = splitString(line, ' ');
        if (tokens[0] == "Player") {
            config.playerConfig.shapeRadius = std::atoi(tokens[1].c_str());
            config.playerConfig.speed = std::atof(tokens[2].c_str());
            config.playerConfig.fillRed = std::atoi(tokens[3].c_str());
            config.playerConfig.fillGreen = std::atoi(tokens[4].c_str());
            config.playerConfig.fillBlue = std::atoi(tokens[5].c_str());
            config.playerConfig.outRed = std::atoi(tokens[6].c_str());
            config.playerConfig.outGreen = std::atoi(tokens[7].c_str());
            config.playerConfig.outBlue = std::atoi(tokens[8].c_str());
            config.playerConfig.outThickness = std::atoi(tokens[9].c_str());
            config.playerConfig.vertices = std::atoi(tokens[10].c_str());
        } else if (tokens[0] == "Enemy") {
            config.enemyConfig.shapeRadius = std::atoi(tokens[1].c_str());
            config.enemyConfig.minSpeed = std::atof(tokens[2].c_str());
            config.enemyConfig.maxSpeed = std::atof(tokens[3].c_str());
            config.enemyConfig.minVertices = std::atoi(tokens[4].c_str());
            config.enemyConfig.maxVertices = std::atoi(tokens[5].c_str());
            config.enemyConfig.lifespan = std::atoi(tokens[6].c_str());
            config.enemyConfig.spawnInterval = std::atoi(tokens[7].c_str());
        } else if (tokens[0] == "Bullet") {
            config.bulletConfig.shapeRadius = std::atoi(tokens[1].c_str());
            config.bulletConfig.speed = std::atof(tokens[2].c_str());
            config.bulletConfig.fillRed = std::atoi(tokens[3].c_str());
            config.bulletConfig.fillGreen = std::atoi(tokens[4].c_str());
            config.bulletConfig.fillBlue = std::atoi(tokens[5].c_str());
            config.bulletConfig.outRed = std::atoi(tokens[6].c_str());
            config.bulletConfig.outGreen = std::atoi(tokens[7].c_str());
            config.bulletConfig.outBlue = std::atoi(tokens[8].c_str());
            config.bulletConfig.outThickness = std::atoi(tokens[9].c_str());
            config.bulletConfig.vertices = std::atoi(tokens[10].c_str());
            config.bulletConfig.lifespan = std::atoi(tokens[11].c_str());
        }
    }

    file.close();

    return config;
}