#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "TinyRNG.h"

#define NSAMPLES 20000

int main() {
    uint32_t *seeds;
    uint32_t err = 0;
    uint32_t i = 0;
    float fsample = 0.0f;
    int32_t isample = 0;
    float u2 = 1.0f;
    
    FILE *file;
    
    /*
     * 32-bit XORSHIFT generator
     */

    // seed the RNG
    seeds = (uint32_t*)malloc(5 * sizeof(uint32_t));
    err = seed(seeds, 5);
    
    // uniform (0, 1]
    file = fopen("testdata/unif01_xorshift32.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = unifrnd32(xorshift32, seeds, &fsample);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);

    /*
     * 32-bit KISS generator
     */

    // seed the RNG
    err = seedkiss(seeds);
    
    // uniform (0, 1]
    file = fopen("testdata/unif01_kiss32.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = unifrnd32(kiss32, seeds, &fsample);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);

    /*
     * test the transformations
     */
    
    // exponential lambda = 1
    file = fopen("testdata/exp1_32.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = exprnd32(kiss32, seeds, &fsample, 1.0f);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);

    // standard normal, mu = 0, sigma = 1
    file = fopen("testdata/normstd_32.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = normstdrnd32(kiss32, seeds, &fsample, &u2);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);

    // normal, mu = 5, sigma = 5
    file = fopen("testdata/norm55_32.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = normrnd32(kiss32, seeds, &fsample, &u2, 5.0f, 5.0f);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);

    // binomial
    file = fopen("testdata/bino2005_32.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = binomialrnd32(kiss32, seeds, &isample, &u2, 20, 0.5);
        fprintf(file, "%d\n", isample);
    }
    fclose(file);     

    // Poisson, lambda = 5
    file = fopen("testdata/poiss5_32.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = poissrnd32(kiss32, seeds, &fsample, &u2, 5.0f);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);

    // gamma
    file = fopen("testdata/gamm51_32.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = gammarnd32(kiss32, seeds, &fsample, &u2, 9.0f, 0.5f);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);

    // beta, alpha = 2, beta = 5
    file = fopen("testdata/beta25_32.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = betarnd32(kiss32, seeds, &fsample, 2.0f, 5.0f);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);

    free(seeds);
    
    return(0);
}
