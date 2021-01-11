
#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include <ctime>

// this generator is now thread safe so we can use it with OpenMP.
static std::random_device rd;
static std::mt19937 generator(rd());

class Random {
public:
//    static void seed(uint64_t seed = std::mt19937_64::default_seed) {
//        generator.seed(seed);
//    }
//
//    static void randomSeed() {
//        //generator.seed(static_cast<unsigned int>( time(nullptr) ));
//    }

    static float nextFloat(const float &min, const float &max) {
        // Random float between min and max
        std::uniform_real_distribution<float> distribution(min, max);
        return distribution(generator);
    }

    static float nextFloat() {
        // Random float between 0 and 1
        std::uniform_real_distribution<float> distribution(0, 1);
        return distribution(generator);
    }

    static double nextDouble(const double &min, const double &max) {
        // Random double between min and max
        std::uniform_real_distribution<double> distribution(min, max);
        return distribution(generator);
    }

    static double nextDouble() {
        // Random double between 0 and 1
        std::uniform_real_distribution<double> distribution(0, 1);
        return distribution(generator);
    }

    static double nextGaussian(const double &mean, const double &sigma) {
        // Normally distributed random number.
        std::normal_distribution<double> distribution(mean, sigma);
        return distribution(generator);
    }

    static int nextInt(const int &min, const int &max) {
        // Random int between min and max
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(generator);
    }

    static int nextInt(const int &max) {
        return nextInt(0, max);
    }

    static long nextLong(const long &min, const long &max) {
        // Random long between min and max
        std::uniform_int_distribution<long> distribution(min, max);
        return distribution(generator);
    }

    static bool nextBool() {
        // Random bool
        std::uniform_int_distribution<int> distribution(0, 1);
        return distribution(generator);
    }

};

#endif // RANDOM_H