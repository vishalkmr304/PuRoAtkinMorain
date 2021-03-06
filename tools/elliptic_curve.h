#include <stdio.h>
#include <gmp.h>
#include "utility.h"

#ifndef ELLIPTIC_CURVE_H
#define ELLIPTIC_CURVE_H

typedef struct curve_point
{
	mpz_t x;
	mpz_t y;
	mpz_t z; //z = 0 --> P = O ||| z = 1 --> P != O  dove O è il punto all'infinito	
}curve_point;

void init(curve_point* P);
void set(curve_point* P, mpz_t c[3]);
void copy(curve_point* Q, curve_point P);
void clear(curve_point* P);

//Algorithm 7.2.1 (Finding two points on a given elliptic curve).
//points deve essere inizializzato come un array a due elementi, ognuno con parametri a e b già inizializzati con mpz_init
void find_two_points(curve_point points[2], mpz_t A, mpz_t B, mpz_t p_mod); //E = x^3 + Ax + B, p > 3

//Algorithm 7.2.2 (Elliptic addition: Affine coordinates). p deve essere maggiore di 3
void negate(curve_point* Q, curve_point P); //Q = -P = (x, -y, z)
int doubleP(curve_point* Q, curve_point P, mpz_t p_mod, mpz_t param_a); //Q = P + P
int add(curve_point* Q, curve_point P1, curve_point P2, mpz_t p_mod, mpz_t param_a); //Q = P1 + P2
int sub(curve_point* Q, curve_point P1, curve_point P2, mpz_t p_mod, mpz_t param_a); //Q = P1 + (-P2)

//Algorithm 7.2.4 (Elliptic multiplication: Addition–subtraction ladder).
int mul(curve_point* Q, curve_point P, mpz_t n, mpz_t p_mod, mpz_t param_a); //Q = [n]P

#endif