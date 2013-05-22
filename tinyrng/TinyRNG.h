#ifndef TINYRNG_H
#define TINYRNG_H

/**************************************************************************
 *  This file is part of TinyRNG.  Copyright Guido Klingbeil (2013).
 *
 *  TinyRNG is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  TinyRNG is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/


/*************************************************************************
 * TinyRNG is a very small and simplistic random number generation and 
 * transformation library implemented in C and only C.
 *
 * The design goals of Tiny RNG are:
 *     - keep it simple and easy to use
 *     - solid assumptions
 *     - thread safe
 *     - 32 / 64-bit awareness
 *     - easy to extend by your own RNGs and transformations
 *
 * What TiniRNG is not:
 * Tiny RNG is not a full blown replacement for a tried and proven RNG library,
 * Cross platform, I developed it on Linux and never tried it elsewhere,
 * Complete set of distributions - I will add transformations for other 
 * distributions over time. However, suggestions are very welcome!
 *
 *************************************************************************/


// define some numerical constants
#define TWOPI32 6.283185307179586f
#define TWOPI64 6.283185307179586
#define PI32 3.141592653589793f
#define PI64 3.141592653589793
#define LN2PI32 1.83787706641f
#define LN2PI64 1.83787706641


// define library specific error code
#define TINYRNG_OK 0
#define TINYRNG_SEED_ERROR 10001
#define TINYRNG_INTERVAL_ERROR 10002


/***********************************
 * Seed the random number generator
 **********************************/

// seed the RNG, to be more flexible, we are giving the number of seeds 
uint32_t seed(uint32_t *seeds, uint32_t l);

// For internal use only
// this part is actually platform dependent, 
// gets a single seed from /dev/urandom
uint32_t getSeed(uint32_t *seed);

uint32_t seedkiss(uint32_t *seeds);

/****************************************************************
 * Generate 32-bits / 64-bits of uniformly distributed randomness
 ***************************************************************/

// XORSHIFT - 32-bit uniform random number generators
uint32_t xorshift32(uint32_t *seeds, uint32_t *sample);

// XORSHIFT - true 64-bit of randomness
uint32_t xorshift64(uint32_t *seeds, uint64_t *sample);

// KISS - 32-bit uniform random number generators
uint32_t kiss32(uint32_t *seeds, uint32_t *sample);

// KISS - true 64-bit of randomness
uint32_t kiss64(uint32_t *seeds, uint64_t *sample);


/*****************************************
 * Transformations into other distributions
 * Implemented so for:
 *   uniform (0, 1],
 *   uniform (a, b],
 *   exponential distribution
 *   standard normal distribution,
 *   normal distribution
 *   binomial distribution
 *   Poisson distribution
 *   gamma distribution
 *   beta distribution
 *****************************************/

/*********************************
 * Uniform random sample on (0, 1]
 ********************************/
uint32_t unifrnd32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, float *sample);
uint32_t unifrnd64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double *sample);


/*********************************
 * Uniform random sample on (a, b]
 ********************************/
uint32_t unifrndint32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, float *sample, float a, float b);
uint32_t unifrndint64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double *sample, double a, double b);


/********************************************
 * Exponential random sample with mean lambda
 *******************************************/
uint32_t exprnd32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, float *sample, float lambda);
uint32_t exprnd64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double *sample, double lambda);


/**************************************************
 * Standard normal random sample, mu = 0, sigma = 1
 *
 * The normal random samples are generated by the
 * Muller transform.
 **************************************************/
// u2 to make the function re-entrant (thread safe)
uint32_t normstdrnd32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, float *sample, float *u2);  
uint32_t normstdrnd64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double *sample, double *u2);


/******************************************************
 * normal random sample with mean mu and variance sigma
 ******************************************************/
// u2 to make the function re-entrant (thread safe)
uint32_t normrnd32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds,float *sample, float *u2, float mu, float sigma);
uint32_t normrnd64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double *sample, double *u2, double mu, double sigma);


// gamma random sample
uint32_t gammarnd32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, float* sample, float* u2, float alpha, float beta);
uint32_t gammarnd64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double* sample, double* u2, double alpha, double beta);


// binomial random sample with number of trail n and probability p

// internal only
uint32_t geometricMethod32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, uint32_t *sample, float n, float p);
uint32_t geometricMethod64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, int64_t *sample, double n, double p);

// this one should be used
uint32_t binomialrnd32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, int32_t *sample, float *u2, uint32_t n, float p);
uint32_t binomialrnd64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, int64_t *sample, double *u2, uint64_t n, double p);



// Poisson random sample with mean lambda

// internal only
float logFactorial32(float N);
double logFactorial64(double N);

uint32_t poissMultiply32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, float* sample, float lambda);
uint32_t poissMultiply64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double* sample, double lambda);

uint32_t poissRejection32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, float* sample,float lambda); 
uint32_t poissRejection64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double* sample, double lambda);

// this one should be used
uint32_t poissrnd32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, float* sample, float* u2, float lambda);
uint32_t poissrnd64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double* sample, double* u2, double lambda);


// beta random sample
uint32_t betarnd32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, float *sample, float alpha, float beta);
uint32_t betarnd64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double *sample, double alpha, double beta);

#endif


