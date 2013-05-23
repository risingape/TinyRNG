#include <stdint.h>
#include <math.h>
#include <fcntl.h>
#include <TinyRNG.h>

/**************************************************************************
 *  This file is part of TinyRNG. Copyright Guido Klingbeil (2013).
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
 *  along with TinyRNG.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/


/*
 * Generate a single 32-bit seed for the the RNG.
 * This is the platform dependent bit since we 
 * read 4 bytes from /dev/urandom.
 */
uint32_t getSeed(uint32_t *seed) {
   int32_t fn;
   uint32_t r;
   
   fn = open("/dev/urandom", O_RDONLY);
   if(fn == -1) {
       return(TINYRNG_SEED_ERROR);
   }
   
   if(read(fn, &r, 4) != 4) {
       return(TINYRNG_SEED_ERROR);
   }
   
   close(fn);
   
   *seed = r;

   return(0);
}


/*
 * The platform independent part of seeding the RNG.
 * Simply filling up the seed array.
 */
uint32_t seed(uint32_t *seeds, uint32_t l) {
	uint32_t _i;
        uint32_t _err = 0;

	for (_i = 0; _i < l; _i++) {
		_err = getSeed(&seeds[_i]);
                if(_err) break;
	}

	return(_err);
}


/*
 * The KISS generator needs its own seeding function since
 * seed[1] must not be 0. seed[4] is actually just the carry
 * bit and we do not need to seed it.
 */
uint32_t seedkiss(uint32_t *seeds) {
    uint32_t _i;
    uint32_t _err = 0;

    _err = getSeed(&seeds[0]);
    if(_err) return(_err);

    while(seeds[1] == 0) {
        _err = getSeed(&seeds[1]);
        if(_err) return(_err);
    }
	
    _err = getSeed(&seeds[2]);
    if(_err) return(_err);

    _err = getSeed(&seeds[3]);
    if(_err) return(_err);

    return(_err);
}


/*
 * RNG itself. We generate 32-bit of randomness in each iteration.
 * If 64-bits of randomness are required, we simple pad them together.
 */

// 32-bit XORSHIFT RNG
// this RNG uses 5 seeds, change the number of seeds changes the structure of the RNG
uint32_t xorshift32(uint32_t *seeds, uint32_t *sample) {
	uint32_t t;

	t = (seeds[0] ^ (seeds[0] >> 7));
	seeds[0] = seeds[1];
	seeds[1] = seeds[2];
	seeds[2] = seeds[3];
	seeds[3] = seeds[4];
	seeds[4] = (seeds[4] ^ (seeds[4] << 6)) ^ (t ^ (t << 13));
	*sample = (seeds[1] + seeds[1] + 1) * seeds[4];

        return(0);
}


// this makes a rather unelegant appearence
// I have to pass a pointer to the generator and
// a second pointer to the seeds all the time

// 64-bit wrapper
// calls the 32-bit generator twice
uint32_t xorshift64(uint32_t *seeds, uint64_t *sample) {
     uint32_t _err = 0;
     uint32_t f1 = 0, f2 = 0;

     // generate 64-bits of randomness
     _err = xorshift32(seeds, &f1);
     _err = xorshift32(seeds, &f2);
     
     *sample = ((uint64_t)f1 << 32) + f2;
     return(_err);
}


/*
 * Marsaglia's KISS (Keep it simple, stupid) generators.
 */
uint32_t kiss32(uint32_t *seeds, uint32_t *sample) {
    uint32_t t = 0;

    seeds[1] ^= (seeds[1] << 5);
    seeds[1] ^= (seeds[1] >> 7);
    seeds[1] ^= (seeds[1] << 22);
    t = seeds[2] + seeds[3] + seeds[4];
    seeds[2] = seeds[3];
    seeds[4] = t < 0;
    seeds[4] = t & 2147483647;
    seeds[0] += 1411392427;
    *sample = seeds[0] + seeds[1] + seeds[3];

    return(0);
}


uint32_t kiss64(uint32_t *seeds, uint64_t *sample) {
     uint32_t _err = 0;
     uint32_t f1 = 0, f2 = 0;

     // generate 64-bits of randomness
     _err = kiss32(seeds, &f1);
     _err = kiss32(seeds, &f2);
     
     *sample = ((uint64_t)f1 << 32) + f2;
     return(_err);
}

// -----------------
// transformations
// -----------------

// uniform random sample on (0, 1]
uint32_t unifrnd32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, float *sample) {
    uint32_t _err = 0;    
    uint32_t f1 = 0;
    float f2 = 0.0f;

    _err = u(seeds, &f1);

    // convert to float

    // the fraction of a single precision floating point number is 23 bits
    f1 = f1 >> 9;

    // set the exponent to 127, since 2(127 - 127) = 1
    *((uint32_t *) &f2) = f1 | 0x3F800000;
    *sample = f2 - 1.0f;

    return(_err);
}


uint32_t unifrnd64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double *sample) {
    uint32_t _err = 0;
    uint64_t f1 = 0;
    double f2 = 0.0;

    // generate 64-bits of randomness
    _err = u(seeds, &f1);

    // convert to double

    // the mantissa of normalised floating point values is always > 1
    // so we have to shift it by 1.0
    f1 = (f1 >> 12) | 0x3FF0000000000000ULL; /* Take upper 52 bits */
    *((uint64_t *)&f2) = f1; /* Make a double from bits */
    *sample = f2 - 1.0;

    return(_err);
}


// 32-bit uniform random sample on (a, b]
uint32_t unifrndint32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, float *sample, float a, float b) {
    uint32_t _err = 0;
    float f = 0.0f;

    // check b > a
    if(a <= b) return(TINYRNG_INTERVAL_ERROR);

    _err = unifrnd32(u, seeds, &f);

    // scaling
    *sample = a + (b - a) * f;

    return(_err);
}


// 64-bit uniform random sample on (a, b]
uint32_t unifrndint64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double *sample, double a, double b) {
    uint32_t _err = 0;   
    double f = 0.0f;

    // check b > a
    if(a <= b) return(TINYRNG_INTERVAL_ERROR);

    _err = unifrnd64(u, seeds, &f);

    // scaling
    *sample = a + (b - a) * f;

    return(_err);
}


// 32-bit exponential random sample with mean lambda
uint32_t exprnd32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, float *sample, float lambda) {
    uint32_t _err = 0;
    float f = 0.0f;

    _err = unifrnd32(u, seeds, &f);

    *sample = -logf(f) / lambda;

    return(_err);
}

// 64-bit exponential random sample with mean lambda
uint32_t exprnd64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double *sample, double lambda) {
    uint32_t _err = 0;
    double f = 0.0;

    _err = unifrnd64(u, seeds, &f);

    *sample = -log(f) / lambda;

    return(_err);
}


// standard normal random sample
// u2 to make the function re-entrant (thread safe)
uint32_t normstdrnd32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, float *sample, float *u2) {
    uint32_t _err = 0;
    float r = 0.0f, phi = 0.0f, *ftmp;

    if(isnan(*u2)) {
        _err = unifrnd32(u, seeds, &r);
        r = sqrtf(-2.0f * logf(r));
        _err = unifrnd32(u, seeds, &phi);
        phi *= TWOPI32;

        *sample = r * cosf(phi);
        *u2 = r * sinf(phi);
    } else { 
        *sample = *u2;
        *u2 = NAN;
    }
    
    return(_err);
}


// 64-bit standard normal random sample
// u2 to make the function re-entrant (thread safe)
uint32_t normstdrnd64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double *sample, double *u2) {
    uint32_t _err = 0;
    double r = 0.0f, phi = 0.0f;
      
    if(isnan(*u2)) {
        _err = unifrnd64(u, seeds, &r);
        r = sqrt(-2.0f * log(r));
        _err = unifrnd64(u, seeds, &phi);
        phi *= TWOPI64;

        *sample = r * cos(phi);
        *u2 = r * sin(phi);
    } else {
        *sample = *u2;
        *u2 = NAN;
    }
    
    return(_err);
}


// normal random sample with mean nu and variance sigma
// u2 to make the function re-entrant (thread safe)
uint32_t normrnd32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, float *sample, float *u2, float mu, float sigma) {
    uint32_t _err = 0;
    float f = 0.0f;

    _err = normstdrnd32(u, seeds, &f, u2);
  
    *sample = mu + sigma * f;
    
    return(_err);
}

uint32_t normrnd64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double *sample, double *u2, double mu, double sigma) {
    uint32_t _err = 0;
    double f = 0.0f;

    _err = normstdrnd64(u, seeds, &f, u2);
  
    *sample = mu + sigma * f;
    
    return(_err);
}

/********************************
 * gamma random sample generation
 *******************************/

/*
 * GAMMA RND based on Marsaglia (2000)
 * reuse one normal random number via an additional float / double u2
 */
uint32_t gammarnd32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, float* sample, float* u2, float alpha, float beta) {
    uint32_t _err = 0;   
    float prefix = 1.0f, f = 0.0f, d = 0.0f, x = 0.0f, v = 0.0f, u1 = 0.0f, c = 0.0f;       

    if (alpha <= 1.0f) {
        _err = unifrnd32(u, seeds, &f);   
        prefix = powf(f, 1.0f / alpha);
        alpha += 1.0f;
    }
    d = alpha -  0.33333333333333f;
    c = 0.33333333333333f / sqrtf(d);

    do {
        _err = normstdrnd32(u, seeds, &x, u2);
        v = 1.0f + c * x;
        v = v * v * v;

        _err = unifrnd32(u, seeds, &u1);
    } while (v <= 0.0f || logf(u1) >= 0.5f * x * x + d * (1.0f - v + logf(v)));
    *sample = prefix * d * v * beta;

    return(_err);
}


uint32_t gammarnd64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double* sample, double* u2, double alpha, double beta) {
    uint32_t _err = 0;   
    double prefix = 1.0, f = 0.0, d = 0.0, x = 0.0, v = 0.0, u1 = 0.0, c = 0.0;       

    if (alpha <= 1.0) {
        _err = unifrnd64(u, seeds, &f);   
        prefix = pow(f, 1.0 / alpha);
        alpha += 1.0;
    }
    d = alpha -  0.33333333333333;
    c = 0.33333333333333 / sqrt(d);

    do {
        _err = normstdrnd64(u, seeds, &x, u2);
        v = 1.0 + c * x;
        v = v * v * v;

        _err = unifrnd64(u, seeds, &u1);
    } while (v <= 0.0 || log(u1) >= 0.5 * x * x + d * (1.0 - v + log(v)));
    *sample = prefix * d * v * beta;

    return(_err);
}


/*****************************************************************
 * binomial random sample with number of trail n and probability p
 ****************************************************************/
uint32_t geometricMethod32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, uint32_t *sample, float n, float p) {
    uint32_t _err = 0;
    float _f = 0.0f;
    float c =  logf(1 - p);
    float x = -1.0f;
    float y = 0.0f;

    if(c == 0.0f) {
        *sample = 0.0f;
    } else {
        do {
            _err = unifrnd32(u, seeds, &_f);
            y += ceilf(logf(_f) / c);
            x ++;
        } while(y <= n);
    }    
    *sample = x;

    return(_err);
}


uint32_t geometricMethod64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, int64_t *sample, double n, double p) {
    uint32_t _err = 0;
    double _f = 0.0f;
    double c =  logf(1 - p);
    double x = -1.0f;
    double y = 0.0f;

    if(c == 0.0f) {
        *sample = 0.0f;
    } else {
        do {
            _err = unifrnd64(u, seeds, &_f);
            y += ceil(log(_f) / c);
            x ++;
        } while(y <= n);
    }    
    *sample = x;

    return(_err);
}


uint32_t binomialrnd32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, int32_t *sample, float *u2, uint32_t n, float p) {
    uint32_t _err = 0;
    uint32_t i = 0;
    float _ftmp = 0.0f;
    int32_t f = 0.0f;
    
    if (n < 10) {
        for (i = 0; i < n; i++) {
            _err = unifrnd32(u, seeds, &_ftmp);
            f += (_ftmp < p);
        }
    } else  if (n < 200.0f || n * p < 70.0f) {
        _err = geometricMethod32(u, seeds, &f, n, p);
    } else {
        // approximate the binomial distribution with the normal distribution
        _err = normrnd32(u, seeds, &_ftmp, u2, n * p, sqrtf(n * p * (1 - p)));
        f = (int32_t)floorf(_ftmp);
    }
    *sample = f;

    return(_err);
}


uint32_t binomialrnd64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, int64_t *sample, double *u2, uint64_t n, double p) {
    uint32_t _err = 0;
    uint32_t i = 0;
    double _ftmp = 0.0f;
    int64_t f = 0.0f;
    
    if (n < 10) {
        for (i = 0; i < n; i++) {
            _err = unifrnd64(u, seeds, &_ftmp);
            f += (_ftmp < p);
        }
    } else  if (n < 200.0f || n * p < 70.0f) {
        _err = geometricMethod64(u, seeds, &f, n, p);
    } else {
        // approximate the binomial distribution with the normal distribution
        _err = normrnd64(u, seeds, &_ftmp, u2, n * p, sqrtf(n * p * (1 - p)));
        f = (int64_t)floor(_ftmp);
    }
    *sample = f;

    return(_err);
}


/***************************************************
 * Poisson random sample generation with mean lambda
 **************************************************/

/*
 * re-write of the log factorial code
 */
float logFactorial32(float N) {
    float f = 0.0f;

    if (N <= 32) {
        // pre-computed binary search tree
        if (N <= 16) {
            if (N <= 8) {
	        if (N <= 4) {
		    if (N <= 2) {
		        if (N <= 1) {
			    f = 0.0f; // 0, 1
			} else {
			    f = 0.693147180559945f; // 2 
			}
		    } else {
		        if (N <= 3) {
			    f = 1.791759469228055f; // 3
			} else {
			    f = 3.178053830347946f; // 4
			}
		    }
		} else {
		    if (N <= 6) {
			if (N <= 5) {
			    f = 4.787491742782046f; // 5
			} else {
			    f = 6.579251212010101f; // 6
			}
		    } else {
		        if (N <= 7) {
			    f = 8.525161361065415f; // 7
			} else {
			    f = 10.604602902745251f; // 8
			}
		    }
		}
	    } else {
		if (N <= 12) {
		    if (N <= 10) {
			if (N <= 9) {
			    f = 12.801827480081469f; // 9
			} else {
			    f = 15.104412573075516f; // 10
		        }
		    } else {
			if (N <= 11) {
			    f = 17.502307845873887f; // 11
			} else {
			    f = 19.987214495661885f; // 12
			}
		    }
		} else {
		    if (N <= 14) {
		        if (N <= 13) {
			    f = 22.552163853123425f; // 13
			} else {
			    f = 25.191221182738680f; // 14
			}
		    } else {
			if (N <= 15) {
			    f = 27.899271383840890f; // 15
			} else {
			    f = 30.671860106080672f; // 16
			}
		    }
		}
            }
	} else {
	    if (N <= 24) {
	        if (N <= 20) {
		    if (N <= 18) {
		        if (N <= 17) {
			    f = 33.505073450136891f; // 17
			} else {
			    f = 36.395445208033053f; // 18
			}
		    } else {
			if (N <= 19) {
			    f = 39.339884187199495f; // 19
			} else {
			    f = 42.335616460753485f; // 20
			}
		    }
		} else {
		    if (N <= 22) {
			if (N <= 21) {
			    f = 45.380138898476908f; // 21
			} else {
			    f = 48.471181351835227f; // 22
		        }
		    } else {
		        if (N <= 23) {
			    f = 51.606675567764377f; // 23
			} else {
			    f = 54.784729398112319f; // 24
			}
		    }
		}
	    } else {
		if (N <= 28) {
		    if (N <= 26) {
			if (N <= 25) {
			    f = 58.003605222980518f; // 25
			} else {
			    f = 61.261701761002001f; // 26
			}
		    } else {
			if (N <= 27) {
			    f = 64.557538627006338f; // 27
			} else {
			    f = 67.889743137181540f; // 28
			}
		    }
		} else {
		    if (N <= 30) {
			if (N <= 29) {
			    f = 71.257038967168015f; // 29
			} else {
			    f = 74.658236348830158f; // 30
			}
		    } else {
			if (N <= 32) {
			    f = 78.092223553315307f; // 31
			} else {
			    f = 81.557959456115043f; // 32
			}
		    }
		}
	    }
	}
	// end of hard-coded binary tree
    } else {
	// use Stirling's approximation:
	// ln(n!) = n ln(n) + 0.5ln(2 PI n) - n + 1/(12 n)

	f = (N + 0.5) * logf(N) + 0.5 * LN2PI32 - N + 1 / (12 * N);
    }

    return (f);
}



/*
 * re-write of the log factorial code
 */
double logFactorial64(double N) {
    double f = 0.0;

    if (N <= 32) {
        // pre-computed binary search tree
        if (N <= 16) {
            if (N <= 8) {
	        if (N <= 4) {
		    if (N <= 2) {
		        if (N <= 1) {
			    f = 0.0f;  // 0, 1
			} else {
			    f = 0.693147180559945; // 2
			}
		    } else {
		        if (N <= 3) {
			    f = 1.791759469228055; // 3
			} else {
			    f = 3.178053830347946; // 4
			}
		    }
		} else {
		    if (N <= 6) {
			if (N <= 5) {
			    f = 4.787491742782046; // 5
			} else {
			    f = 6.579251212010101; // 6
			}
		    } else {
		        if (N <= 7) {
			    f = 8.525161361065415; // 7
			} else {
			    f = 10.604602902745251; // 8
			}
		    }
		}
	    } else {
		if (N <= 12) {
		    if (N <= 10) {
			if (N <= 9) {
			    f = 12.801827480081469; // 9
			} else {
			    f = 15.104412573075516; // 10
		        }
		    } else {
			if (N <= 11) {
			    f = 17.502307845873887; // 11
			} else {
			    f = 19.987214495661885; // 12
			}
		    }
		} else {
		    if (N <= 14) {
		        if (N <= 13) {
			    f = 22.552163853123425; // 13
			} else {
			    f = 25.191221182738680; // 14
			}
		    } else {
			if (N <= 15) {
			    f = 27.899271383840890; // 15
			} else {
			    f = 30.671860106080672; // 16
			}
		    }
		}
            }
	} else {
	    if (N <= 24) {
	        if (N <= 20) {
		    if (N <= 18) {
		        if (N <= 17) {
			    f = 33.505073450136891; // 17
			} else {
			    f = 36.395445208033053; // 18
			}
		    } else {
			if (N <= 19) {
			    f = 39.339884187199495; // 19
			} else {
			    f = 42.335616460753485; // 20
			}
		    }
		} else {
		    if (N <= 22) {
			if (N <= 21) {
			    f = 45.380138898476908; // 21
			} else {
			    f = 48.471181351835227; // 22
		        }
		    } else {
		        if (N <= 23) {
			    f = 51.606675567764377; // 23
			} else {
			    f = 54.784729398112319; // 24
			}
		    }
		}
	    } else {
		if (N <= 28) {
		    if (N <= 26) {
			if (N <= 25) {
			    f = 58.003605222980518; // 25
			} else {
			    f = 61.261701761002001; // 26
			}
		    } else {
			if (N <= 27) {
			    f = 64.557538627006338; // 27
			} else {
			    f = 67.889743137181540; // 28
			}
		    }
		} else {
		    if (N <= 30) {
			if (N <= 29) {
			    f = 71.257038967168015; // 29
			} else {
			    f = 74.658236348830158; // 30
			}
		    } else {
			if (N <= 32) {
			    f = 78.092223553315307; // 31
			} else {
			    f = 81.557959456115043; // 32
			}
		    }
		}
	    }
	}
	// end of hard-coded binary tree
    } else {
	// use Stirling's approximation:
	// ln(n!) = n ln(n) + 0.5ln(2 PI n) - n + 1/(12 n)

	f = (N + 0.5) * log(N) + 0.5 * LN2PI32 - N + 1 / (12 * N);
    }

    return (f);
}


uint32_t poissMultiply32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, float* sample, float lambda) {
    uint32_t _err = 0;
    float L = expf(-lambda);
    float p = 1.0f, k = 0.0f, ftmp = 0.0f;

    do {
        k++;
        _err = unifrnd32(u, seeds, &ftmp); 
        p *= ftmp;
    } while (p > L);
    *sample = k - 1;

    return(_err);
}


uint32_t poissMultiply64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double* sample, double lambda) {
    uint32_t _err = 0;
    double L = exp(-lambda);
    double p = 1.0, k = 0.0, ftmp = 0.0;

    do {
        k++;
        _err = unifrnd64(u, seeds, &ftmp); 
        p *= ftmp;
    } while (p > L);
    *sample = k - 1;

    return(_err);
}


uint32_t poissRejection32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, float* sample,float lambda) {
    uint32_t _err = 0;

    // values depending on lambda:
    float beta = PI32 / sqrtf(3.0f * lambda);
    float alpha = beta * lambda;
    float c = 0.767f - 3.36f / lambda;
    float k = logf(c) - lambda - logf(beta);

    float U1 = 0.0f, U2 = 0.0f, X = 0.0f, N = 0.0f, tmp1 = 0.0f, tmp2 = 0.0f;

    do {
         _err = unifrnd32(u, seeds, &U1);
         X = (alpha - logf((1 - U1) / U1)) / beta;

         if (X < 0.0f)
             continue;

         N = floorf(X);
        _err = unifrnd32(u, seeds, &U2);

        tmp1 = alpha - beta * X + logf(U2 / ((1 + expf(alpha - beta * X)) * (1 + expf(alpha - beta * X))));
        tmp2 = k + N * logf(lambda) - logFactorial32(N);
    } while (tmp1 > tmp2);
    *sample = N;

    return(_err);
}


uint32_t poissRejection64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double* sample, double lambda) {
    uint32_t _err = 0;

    // values depending on lambda:
    double beta = PI32 / sqrtf(3.0f * lambda);
    double alpha = beta * lambda;
    double c = 0.767f - 3.36f / lambda;
    double k = logf(c) - lambda - logf(beta);

    double U1 = 0.0f, U2 = 0.0f, X = 0.0f, N = 0.0f, tmp1 = 0.0f, tmp2 = 0.0f;

    do {
         _err = unifrnd64(u, seeds, &U1);
         X = (alpha - logf((1 - U1) / U1)) / beta;

         if (X < 0.0f)
             continue;

         N = floorf(X);
        _err = unifrnd64(u, seeds, &U2);

        tmp1 = alpha - beta * X + logf(U2 / ((1 + expf(alpha - beta * X)) * (1 + expf(alpha - beta * X))));
        tmp2 = k + N * logf(lambda) - logFactorial64(N);
    } while (tmp1 > tmp2);
    *sample = N;

    return(_err);
}


// for large lambda approximate the Poisson distribution with the
// normal distribution with mu = lambda and sigma^2 = lambda
uint32_t poissrnd32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, float* sample, float* u2, float lambda) {
    uint32_t _err = 0;
    float _ftmp = 0.0f;

    if (lambda <= 30.0f) {
        // for small values of N use the direct multiplication method
        _err = poissMultiply32(u, seeds, &_ftmp, lambda);
    } else if (lambda <= 100.0f) {
        // for intermediate values of N apply rejection sampling
        _err = poissRejection32(u, seeds, &_ftmp, lambda);
    } else {
        // approximation using the central limit theorem (CLT)
        // using the positive part of the CDF Phi((x - lambda) / sqrt(lambda))
        _err = normrnd32(u, seeds, &_ftmp, u2, lambda, sqrtf(lambda));
        _ftmp = fmaxf(0.0f, floorf(_ftmp));
    }
    *sample = _ftmp;

    return(_err);
}


// for large lambda approximate the Poisson distribution with the
// normal distribution with mu = lambda and sigma^2 = lambda
uint32_t poissrnd64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double* sample, double* u2, double lambda) {
    uint32_t _err = 0;
    double _ftmp = 0.0f;

    if (lambda <= 30.0f) {
        // for small values of N use the direct multiplication method
        _err = poissMultiply64(u, seeds, &_ftmp, lambda);
    } else if (lambda <= 100.0f) {
        // for intermediate values of N apply rejection sampling
        _err = poissRejection64(u, seeds, &_ftmp, lambda);
    } else {
        // approximation using the central limit theorem (CLT)
        // using the positive part of the CDF Phi((x - lambda) / sqrt(lambda))
        _err = normrnd64(u, seeds, &_ftmp, u2, lambda, sqrtf(lambda));
        _ftmp = fmaxf(0.0f, floorf(_ftmp));
    }
    *sample = _ftmp;

    return(_err);
}


/*******************************
 * beta random sample generation
 ******************************/

// by simple rejection sampling using Cheng's rejection algorithm BA (Cheng1978)
uint32_t betarnd32(uint32_t (*u)(uint32_t *, uint32_t *), uint32_t *seeds, float_t *sample, float alpha, float beta) {
    uint32_t _err = 0;
    float _s = 0.0f, _lambda = 0.0f, _u = 0.0f, _v = 0.0f, _y = 0.0f, _u1 = 0.0f, _u2 = 0.0f;

    _s = alpha + beta;
    _u = fminf(alpha, beta);

    if(_u <= 1.0f) {
        _lambda = _u;
    } else {
        _lambda = sqrtf((2 * alpha * beta - _s) / (_s - 2));
    }

    _u = alpha + _lambda;

    do {
        _err = unifrnd32(u, seeds, &_u1);
        _err = unifrnd32(u, seeds, &_u2);

        _v = logf(_u1 / (1 - _u1)) / _lambda;
        _y = alpha * expf(_v);
    }while(_s * logf(_s / (beta + _y)) + _u * _v - logf(4) < logf(_u1 * _u1 * _u2) );

    *sample = _y / (beta + _y);

    return(_err);
}


uint32_t betarnd64(uint32_t (*u)(uint32_t *, uint64_t *), uint32_t *seeds, double *sample, double alpha, double beta) {
    uint32_t _err = 0;
    double _s = 0.0f, _lambda = 0.0f, _u = 0.0f, _v = 0.0f, _y = 0.0f, _u1 = 0.0f, _u2 = 0.0f;

    _s = alpha + beta;
    _u = fmin(alpha, beta);

    /*
     * set-up part of the algorithm
     */
    if(_u <= 1.0f) {
        _lambda = _u;
    } else {
        _lambda = sqrt((2 * alpha * beta - _s) / (_s - 2));
    }
    _u = alpha + _lambda;

    /*
     * Rejection step
     */
    do {
        _err = unifrnd64(u, seeds, &_u1);
        _err = unifrnd64(u, seeds, &_u2);

        _v = log(_u1 / (1 - _u1)) / _lambda;
        _y = alpha * exp(_v);
    }while(_s * log(_s / (beta + _y)) + _u * _v - log(4) < log(_u1 * _u1 * _u2) );

    *sample = _y / (beta + _y);

    return(_err);
}

