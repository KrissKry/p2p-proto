#ifndef TIN_RANDOMGENERATOR
#define TIN_RANDOMGENERATOR

#include <random>

class RandomGenerator {
    
    private:
        std::mt19937 m_mt;

        double transmission_error_threshold = 0.995; 

        bool error_enabled = true;

    public:

        RandomGenerator() : m_mt( (std::random_device()) ()) {} //init marsenne-twister rg
        ~RandomGenerator() {}

        bool transmissionFailure() {
            if (error_enabled) {

                std::uniform_real_distribution<double> dist(0.0, 1.0);

                return dist(m_mt) >= transmission_error_threshold;
            
            } else 
                return false;
        }

};

#endif