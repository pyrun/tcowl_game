#ifndef CONFIG_H
#define CONFIG_H

#include <nlohmann/json.hpp>

#define CONFIG_FILE "config.json"

namespace engine {
    class config {
        public:
            config();
            ~config();

            void load();
            void save();

            template< class T>
            T get(std::string name) { return p_json[name]; }

            nlohmann::json *getJson() { return &p_json; }
        private:
            nlohmann::json p_json;
    };
};

#endif