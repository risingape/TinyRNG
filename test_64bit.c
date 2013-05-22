// 64-bits of randomness test

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
    double fsample = 0.0f;
    int64_t isample = 0;
    double u2 = 0.0f;
    
    FILE *file;
    
    /*
     * 64-bit XORSHIFT generator
     */

    // seed the RNG
    seeds = (uint32_t*)malloc(5 * sizeof(uint32_t));
    err = seed(seeds, 5);
    
    // uniform (0, 1]
    file = fopen("testdata/unif01_xorshift64.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = unifrnd64(xorshift64, seeds, &fsample);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);

    /*
     * 64-bit KISS generator
     */

    // seed the RNG
    err = seedkiss(seeds);
    
    // uniform (0, 1]
    file = fopen("testdata/unif01_kiss64.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = unifrnd64(kiss64, seeds, &fsample);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);

    /*
     * test the transformations
     */

    // exponential lambda = 1
    file = fopen("testdata/exp1_64.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = exprnd64(kiss64, seeds, &fsample, 1.0);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);

    // standard normal, mu = 0, sigma = 1
    file = fopen("testdata/normstd_64.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = normstdrnd64(kiss64, seeds, &fsample, &u2);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);

    // normal, mu = 5, sigma = 5
    file = fopen("testdata/norm55_64.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = normrnd64(kiss64, seeds, &fsample, &u2, 5.0, 5.0);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);

    // binomial
    file = fopen("testdata/bino2005_64.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = binomialrnd64(kiss64, seeds, &isample, &u2, 20, 0.5);
        fprintf(file, "%d\n", isample);
    }
    fclose(file);    

    // Poisson, lambda = 5
    file = fopen("testdata/poiss5_64.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = poissrnd64(kiss64, seeds, &fsample, &u2, 5.0);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);

    // gamma
    file = fopen("testdata/gamm51_64.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = gammarnd64(kiss64, seeds, &fsample, &u2, 5.0, 1.0);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);    

    // beta, alpha = 2, beta = 5
    file = fopen("testdata/beta25_64.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = betarnd64(kiss64, seeds, &fsample, 2.0, 5.0);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);





















    // uniform (0, 1]
    file = fopen("unif01_64.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = unifrnd64(kiss64, seeds, &fsample);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);
    
    // exponential lambda = 1
    file = fopen("exp1_64.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = exprnd64(kiss64, seeds, &fsample, 1.0);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);

    // standard normal, mu = 0, sigma = 1
    file = fopen("normstd_64.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = normstdrnd64(kiss64, seeds, &fsample, &u2);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);

    // normal, mu = 5, sigma = 5
    file = fopen("norm55_64.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = normrnd64(kiss64, seeds, &fsample, &u2, 5.0, 5.0);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);

    // binomial
    file = fopen("bino2005_64.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = binomialrnd64(kiss64, seeds, &isample, &u2, 20, 0.5);
        fprintf(file, "%d\n", isample);
    }
    fclose(file);    

    // Poisson, lambda = 5
    file = fopen("poiss5_64.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = poissrnd64(kiss64, seeds, &fsample, &u2, 5.0);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);

    // gamma
    file = fopen("gamm51_64.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = gammarnd64(kiss64, seeds, &fsample, &u2, 5.0, 1.0);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);    

    // beta, alpha = 2, beta = 5
    file = fopen("beta25_64.txt", "w");
    for(i = 0; i < NSAMPLES; i ++) {
        err = betarnd64(kiss64, seeds, &fsample, 2.0, 5.0);
        fprintf(file, "%f\n", fsample);
    }
    fclose(file);

    free(seeds);
    
    return(0);
}
