This fork is for development only. PLEASE DO NOT USE IN PRODUCTION.

The goal is to build a custom version of scikit-learn which uses a random number generation that is reproducible, even outside the official python version.

# Reproducible randoms

The goal is to make the random number generators used in scikit-learn reproducible even outside scikit-learn.

The level of reproducibility we want to reach here makes it possible to write a C++ piece of software that generates the same sequence of random numbers when implementing the same algorithm (congruential, mersenne twister etc) and using the same seeds.

At least four ways of generating random numbers in scikit-learn are observed. There is nothing wrong with this, but having a common way is better. Even numpy random number generator use a Mersenne twister internally but in a way that makes it incompatible with the Mersenne twister used in C++ standard generator (std::mt19937).

Scikit-learn uses a set of random number generators with their specific seeding methods. Some come from numpy/scipy other  are more specific (hacks ?) and implemented only for some models (legacy code ?).

A "fix" would be to have  a C++-compatible interface and plug-it it everywhere (replacing the existing methods).

1. numpy.random.RandomState

   Used almost everywhere in a uniform way for training models (fit methods).
   Container for the slow Mersenne Twister pseudo-random number generator ???.
   Consider using a different BitGenerator with the Generator container instead ???.
   https://numpy.org/doc/stable/reference/random/legacy.html#numpy.random.RandomState

2. std::mt19937. The C++ starndard random number generator (Mersenne Twister).

   Defined in svm/src/newrand/newrand.h

// Scikit-Learn-specific random number generator replacing `rand()` originally
// used in LibSVM / LibLinear, to ensure the same behaviour on windows-linux,
// with increased speed
// - (1) Init a `mt_rand` object
```
std::mt19937 mt_rand(std::mt19937::default_seed);
```

https://github.com/scikit-learn/scikit-learn/blob/d90dd114ab85b85e49facdb7b37242b7362696c8/sklearn/svm/src/newrand/newrand.h#L21

3. bounded_rand_int

   Used in svms. Uses lrand48()

   Defined in svm/src/newrand/newrand.h

```
   uint32_t bounded_rand_int(uint32_t iUpperLimit) {
        auto out = (lrand48() + iUpperLimit) % iUpperLimit;
        return out; 
  }
```

  https://github.com/scikit-learn/scikit-learn/blob/d90dd114ab85b85e49facdb7b37242b7362696c8/sklearn/svm/src/newrand/newrand.h#L29

4. our_rand_r
   rand_r replacement using a 32bit XorShift generator
   See http://www.jstatsoft.org/v08/i14/paper for details

   Defined in ./utils/_random.pxd

   Used in decision tree models, linear models etc
   
https://github.com/scikit-learn/scikit-learn/blob/d90dd114ab85b85e49facdb7b37242b7362696c8/sklearn/utils/_random.pxd#L26

```
   RAND_R_MAX = 2147483647
   cdef inline UINT32_t our_rand_r(UINT32_t* seed) nogil:
    """Generate a pseudo-random np.uint32 from a np.uint32 seed"""
    # seed shouldn't ever be 0.
    if (seed[0] == 0):
        seed[0] = DEFAULT_SEED

    seed[0] ^= <UINT32_t>(seed[0] << 13)
    seed[0] ^= <UINT32_t>(seed[0] >> 17)
    seed[0] ^= <UINT32_t>(seed[0] << 5)

    # Use the modulo to make sure that we don't return a values greater than the
    # maximum representable value for signed 32bit integers (i.e. 2^31 - 1).
    # Note that the parenthesis are needed to avoid overflow: here
    # RAND_R_MAX is cast to UINT32_t before 1 is added.
    return seed[0] % ((<UINT32_t>RAND_R_MAX) + 1)
```

