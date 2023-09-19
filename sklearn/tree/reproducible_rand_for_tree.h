#include <random>
#include <iostream>

namespace rep {

  class cReproducibleRandomNumberGenerator {
    size_t mSeed;
    ::std::mt19937 mGenerator;

  public:
    cReproducibleRandomNumberGenerator() : mSeed(1789), mGenerator(){
      mGenerator.seed(mSeed);
    }

    ~cReproducibleRandomNumberGenerator() {

    }

    void set_seed(size_t iSeed) {
      ::std::cout << "cReproducibleRandomNumberGenerator SET_SEED " << iSeed << std::endl << std::flush;
      mSeed = iSeed;
      mGenerator.seed(mSeed);
    }

    size_t rand_int(size_t low, size_t high) {
      auto lDist = ::std::uniform_int_distribution<size_t>(low, high - 1);
      auto int_rand = lDist(mGenerator);
      ::std::cout << "cReproducibleRandomNumberGenerator RAND_INT " << low << " " << high
		  << " " << int_rand << std::endl << std::flush;
      return int_rand;
    }

    double rand_uniform(double min_value, double max_value) {
      auto lDist = ::std::uniform_real_distribution<double>(min_value,max_value);
      auto real_rand = lDist(mGenerator);
      ::std::cout << "cReproducibleRandomNumberGenerator RAND_UNIFORM " << min_value << " " << max_value
		  << " " << real_rand << std::endl << std::flush;
      return real_rand;
    }

  
  };

} // eof anonymous namepace

