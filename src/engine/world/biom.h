#ifndef BIOM_H
#define BIOM_H

namespace engine {
    class biom {
        public:
            biom();
            ~biom();

            void setName( std::string name) { p_name = name; }
            std::string getName() { return p_name; }
        private:
            std::string p_name;
    };
};

#endif