#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <gmp.h>

#ifndef UTILITY_H
#define UTILITY_H

typedef struct resultset
{
	mpz_t results[4]; //da inizializzare!!!
	int num_elements;	
}resultset;

//Algoritmo  2.3.8 - pag. 114
//Given an odd prime p and an integer a with (a/p) = 1, this algorithm returns a solution x to x^2 ≡ a (mod p).
int radix_GMP(mpz_t radix, mpz_t p, mpz_t a);

void integerpart_GMP(mpz_t result, mpz_t N);

int cornacchia_GMP(struct resultset* r, mpz_t p, mpz_t D);

#endif
