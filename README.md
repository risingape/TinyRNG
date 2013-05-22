= TinyRNG - a tiny random number library

= Description:
  TinyRNG is a very small and simplistic random number generation and 
  transformation library implemented in C and only C. It is built around two very
  simple random number generators (RNGs): Marsaglia's XORSHIFT RNG and KISS 
  (Keep It Simple Stupid) RNG.

  TinyRNG generates random numbers with 32-bit or 64-bit of randomness and is 
  able to transform them into samples from  the following distributions:

  * uniform [0,1),
  * exponential,
  * standard normal,
  * normal,
  * binomial,
  * Poisson,
  * gamma,
  * beta.

  The design goals of TinyRNG are:

  * keep it simple it simple and understandable,
  * simple and understandable algorithms,
  * references to all algorithms are available,
  * consistend call-by-reference interface for all functions, 
  * all functions are thread safe and re-entrant,
  * 32 / 64-bit awareness,
  * easy to extend by your own RNGs and transformations
 
  What TinyRNG is not:
  
  * TinyRNG is not a replacement for a tried and proven RNG library,
  * cross platform, I developed it on Linux and never tried it elsewhere,
  * complete set of transformations - I will add transformations for other 
     distributions over time. Any suggestions are very welcome!

= Installation:

  make

  make install

= Usage:

  Have a look at the Makefile for an example how to link your programme to
  TinyRNG. If you do not want or you are not able to install the library to your
  system, please add the local path to LD_LIBRARY_PATH by:

  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.



