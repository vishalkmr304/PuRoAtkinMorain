#include "utility.h"

/*
Algoritmo  2.3.8 - pag. 114
Given an odd prime p and an integer a with (a/p) = 1, this algorithm returns a solution x to x^2 ≡ a (mod p).
*/
//restituisce 0 in caso di errore
//restituisec 1 se tutto ok
int radix_GMP(mpz_t radix, mpz_t p, mpz_t a_a)
{
	mpz_t a; //faccio una copia del valore di a_a altrimenti viene modificata la variabile originale
	mpz_init_set(a, a_a);

	mpz_mod(a, a, p); //trasforma resti negativi automaticamente in resti positivi!
		
	//gmp_printf("a: %Zd\n", a);
	//gmp_printf("p: %Zd\n", p);

	//printf("jacobi: %d\n", mpz_jacobi(a, p));

	if(mpz_jacobi(a, p) != 1) //controlla che (a/p) == 1, così siamo sicuri che esista una radice 		
		return 0;	
	
	mpz_t two;
	mpz_init_set_ui(two, 2);

	mpz_t residue;
	mpz_init(residue);
	
	mpz_mod_ui(residue, p, 8);
	//gmp_printf("residue: %Zd\n", residue);
	if(mpz_cmp_ui(residue, 3) == 0 || mpz_cmp_ui(residue, 7) == 0)
	{
		mpz_t temp_exp;
		mpz_init(temp_exp);	
		mpz_add_ui(temp_exp, p, 1);

		mpz_tdiv_q_ui(temp_exp, temp_exp, 4);

		/*gmp_printf("temp_exp: %Zd\n", temp_exp);
		gmp_printf("a: %Zd\n", a);
		gmp_printf("radix: %Zd\n", radix);
		gmp_printf("p: %Zd\n", p);*/

		mpz_powm(radix, a, temp_exp, p);
		//mpz_abs(radix, radix);				

		mpz_clear(temp_exp);
		mpz_clear(residue);
		mpz_clear(two);
		mpz_clear(a);

		return 1;
	}
	else if(mpz_cmp_ui(residue, 5) == 0)
	{
		mpz_t temp_exp;
		mpz_init(temp_exp);	
		mpz_add_ui(temp_exp, p, 3);
		mpz_tdiv_q_ui(temp_exp, temp_exp, 8);

		mpz_powm(radix, a, temp_exp, p);

		mpz_t c;
		mpz_init(c);	
		mpz_powm_ui(c, radix, 2, p);
		
		mpz_t r;
		mpz_init(r);	
		mpz_mod(r, a, p);
		
		int cmp = mpz_cmp (c, r);

		if(cmp != 0) //c != a%p
		{
			mpz_t radix_temp;
			mpz_init(radix_temp);

			mpz_sub_ui(temp_exp, p, 1);
			mpz_tdiv_q_ui(temp_exp, temp_exp, 4);

			mpz_powm(radix_temp, two, temp_exp, p); //2^((p-1)/4) - il fatto che venga fatta già l'operazione di modulo non cambia i risultati
			mpz_mul(radix, radix_temp, radix); //(x * 2^((p-1)/4)) mod p

			mpz_mod(radix, radix, p);
			mpz_clear(radix_temp);
		}
		
		//Clear
		mpz_clear(r);		
		mpz_clear(c);		
		mpz_clear(temp_exp);
		mpz_clear(residue);
		mpz_clear(two);
		mpz_clear(a);		

		//mpz_abs(radix, radix);
		return 1;
	}
	else if(mpz_cmp_ui(residue, 1) == 0)
	{	
		gmp_randstate_t state;
		gmp_randinit_mt(state);

		mpz_t temp_add;
		mpz_init(temp_add);
		mpz_t temp_mul;
		mpz_init(temp_mul);
		mpz_t temp_div;
		mpz_init(temp_div);
		mpz_t temp_mod;
		mpz_init(temp_mod);
		mpz_t temp_exp;
		mpz_init(temp_exp);

		mpz_t d;
		mpz_init(d);		

		mpz_t sup;
		mpz_init(sup);
		mpz_sub_ui(sup, p, 2); //p-2 è corretto
				
		mpz_t counter;
		mpz_init_set_ui(counter, 0);

		mpz_t p_2;
		mpz_init(p_2);
		mpz_mul_ui(p_2, p, 2);

		int found = 1;	
		
		do
		{		
			if(mpz_cmp(counter, p_2) == 0) //aggiunto un controllo il cui verifico se siano stati estratti tutti i numeri e in tal caso esco, per evitare cicli infiniti (metto p*2 per sicurezza)
			{
				found = 0;
				break;
			}
	
			mpz_urandomm(d, state, sup); //estrae numero casuale in [0, p-3], è corretto mettere p-2 come terzo parametro
			mpz_add_ui(d, d, 2); //per avere numero casuale in [2, p-1]

			mpz_add_ui(counter, counter, 1);	
		}
		while(mpz_jacobi(d, p) != -1); //esce quando trova (d/p) = -1 - il fatto che venga prima eseguito il controllo su (a/p) == 1 ci assicura che una radice esista, evitando cicli infiniti
		
		mpz_clear(p_2);
		mpz_clear(counter);

		if(!found)
		{
			mpz_clear(a);
			mpz_clear(two);
			mpz_clear(residue);
			mpz_clear(temp_add);
			mpz_clear(temp_mul);
			mpz_clear(temp_div);
			mpz_clear(temp_mod);
			mpz_clear(temp_exp);
			mpz_clear(d);	
			mpz_clear(sup);

			return 0;
		}
		
		//////////////////////////////////////////////////////////////////////////
		//calcolo s e t tali che p-1 = 2^s * t
		mpz_t p_minus1;
		mpz_init(p_minus1);
		mpz_sub_ui(p_minus1, p, 1);
		mpz_t s; //esponente
		mpz_init_set_ui(s, 0);		
		mpz_t pow_two;
		mpz_init_set_ui(pow_two, 1);
		mpz_t t;
		mpz_init_set_ui(t, 1);
		do
		{
			mpz_mul_ui(pow_two, pow_two, 2); //potenze di 2			
			mpz_add_ui(s, s, 1); //esponenziale di 2	
		}
		while(mpz_cmp(pow_two, p_minus1) < 0);		

		if(mpz_cmp(pow_two, p_minus1) != 0) //nel caso contrario lascio t = 1 e p-1=2^s
		{
			mpz_sub_ui (s, s, 1); //perchè sono andato oltre p-1 con l'ultima operazione
			mpz_tdiv_q_ui(pow_two, pow_two, 2); //perchè sono andato oltre p-1 con l'ultima operazione
			
			mpz_tdiv_q(t, p_minus1, pow_two); //divisione fra numeri interi!!!! - inoltre t deve essere dispari!

			mpz_mul(temp_mul, pow_two, t);
			mpz_mod_ui(temp_mod, t, 2);

			while(mpz_cmp(temp_mul, p_minus1) != 0 || mpz_cmp_ui(temp_mod, 0) == 0)//finchè non trovo un numero t che sia dispari e tale per cui pow_two*t == p_minus1, continuo il ciclo
			{
				mpz_tdiv_q_ui(pow_two, pow_two, 2);
				mpz_sub_ui (s, s, 1);
				mpz_tdiv_q(t, p_minus1, pow_two);

				mpz_mul(temp_mul, pow_two, t);
				mpz_mod_ui(temp_mod, t, 2);
			}
			//negazione condizione = (pow_two*t == p_minus1 && t%2 != 0) DE MORGAN
		} 

		//////////////////////////////////////////////////////////////////////////
		
		//gmp_printf("pow_two: %Zd\n", pow_two);
		//gmp_printf("s: %Zd\n", s);
		//gmp_printf("t: %Zd\n", t);

		mpz_t A;
		mpz_t D;
		mpz_t D_m;
		mpz_t A_D_m;		
		//mpz_t oper;		
		mpz_t m_big;

		mpz_init(A);
		mpz_init(D);
		mpz_init(D_m);
		mpz_init(A_D_m);
		//mpz_init(oper);	
		mpz_init(m_big);		

		mpz_powm(A, a, t, p);					
		mpz_powm(D, d, t, p);
		
		//long m = 0;
		mpz_set_ui(m_big, 0);	
		//unsigned long int temp_exp_l = 0;
		unsigned long int s_l = mpz_get_ui(s); //s è un esponente, quindi non è mai troppo grande! (in questo caso non dovrebbe mai essere troppo grande)
		unsigned int i;
		
		//gmp_printf("s: %Zd\n", s);
		//printf("s_l: %lu\n", s_l);
	
		for(i = 0; i < s_l; i++)
		{	
			//printf("i: %d\n", i);
			//printf("m: %lu\n", m);
			//printf("aaaaa\n");

			//mpz_pow_ui(D_m, D, m);
			//mpz_set_ui(m_big, m);
			
			mpz_powm(D_m, D, m_big, p);

			//printf("ccccc\n");
			mpz_ui_pow_ui(temp_exp, 2, s_l - 1 - i);			
			mpz_mul(A_D_m, A, D_m);			
			
			mpz_powm(temp_mod, A_D_m, temp_exp, p);			

			//gmp_printf("temp_mod: %Zd\n", temp_mod);
			//gmp_printf("p: %Zd\n", p);

			//temp_exp_l = mpz_get_ui(temp_exp); //temp_exp è un esponente, quindi non è mai troppo grande!			
			//mpz_pow_ui(oper, A_D_m, temp_exp_l);
			//mpz_mod(temp_mod, oper, p);

			if(mpz_cmp(temp_mod, p_minus1) == 0) // -1 == p-1, secondo controllo probabilmente non serve	// || mpz_cmp(temp_mod, -1) == 0
			{					
				//m += pow(2, i);
				mpz_ui_pow_ui(temp_exp, 2, i);
				mpz_add(m_big, m_big, temp_exp);
				//printf("m: %lu\n", m);
				//gmp_printf("M: %Zd\n", m_big);
			}												
		}
		//getchar();		

		/*printf("----------\n");
		mpz_powm(A, a, t, p);
		mpz_set_ui(m_big, m);
		mpz_powm(D_m, D, m_big, p);
		mpz_mul(A_D_m, A, D_m);	
		mpz_mod(A_D_m, A_D_m, p);
		gmp_printf("A_D_m: %Zd\n", A_D_m);
		gmp_printf("m_big: %Zd\n", m_big);
		printf("----------\n");*/

		mpz_t term_1;
		mpz_init(term_1);
		mpz_add_ui(temp_add, t, 1);
		mpz_tdiv_q_ui(temp_exp, temp_add, 2);
		//temp_exp_l = mpz_get_ui(temp_exp);
		mpz_powm(term_1, a, temp_exp, p);
		//mpz_pow_ui(term_1, a, temp_exp_l); //dava come errore gmp: overflow in mpz type, meglio fare tutto in un'unica istruzione
		//mpz_mod(term_1, term_1, p);

		mpz_t term_2;
		mpz_init(term_2);
		mpz_t mdiv2;
		mpz_init_set(mdiv2, m_big);
		mpz_tdiv_q_ui(mdiv2, mdiv2, 2);

		mpz_powm(term_2, D, mdiv2, p);
		//mpz_pow_ui(term_2, D, m/2);
		//mpz_mod(term_2, term_2, p);

		mpz_mul(temp_mul, term_1, term_2);
		mpz_mod(radix, temp_mul, p);
		//mpz_abs(radix, radix);

		//Clear
		mpz_clear(mdiv2);
		mpz_clear(term_2);
		mpz_clear(term_1);
		mpz_clear(m_big);
		mpz_clear(A_D_m);
		mpz_clear(D_m);
		mpz_clear(D);
		mpz_clear(A);
		mpz_clear(t);
		mpz_clear(pow_two);
		mpz_clear(s);
		mpz_clear(p_minus1);
		mpz_clear(sup);
		mpz_clear(d);
		mpz_clear(temp_exp);
		mpz_clear(temp_mod);
		mpz_clear(temp_div);
		mpz_clear(temp_mul);
		mpz_clear(temp_add);
		gmp_randclear(state);
		mpz_clear(residue);
		mpz_clear(two);
		mpz_clear(a);

		return 1;
	}
	else //se per errore gli passo un numero pari, viene resto pari
	{
		//printf("ERRORE: Resto Pari\n");
		mpz_clear(residue);
		mpz_clear(two);
		mpz_clear(a);
		return 0;
	}	
}

//forse sarebbe meglio fare semplicemente floor(sqrt(N)), ogni volta sclera xD
void integerpart_GMP(mpz_t result, mpz_t N)
{
	//printf("START\n");
	//gmp_printf("N: %Zd\n", N);

	mpf_t x;
	mpf_t y;
	mpf_init(x);
	mpf_init(y);
	
	//[Initialize] bitcount
	size_t bit_number = mpz_sizeinbase(N, 2);	//unsigned int bit_number
	//printf("bit_number: %zu\n", bit_number);	

	unsigned int exp = (unsigned int) ceil( ( (float)bit_number)/2);
	//printf("exp: %u\n", exp);

	mpz_t base;
	mpz_init(base);
	mpz_set_ui(base, 2);
	mpf_t f_base;
	mpf_init(f_base);
	mpf_set_z(f_base, base);
	
	mpf_pow_ui(x, f_base, exp);
	
	mpf_t sum;	//float per la somma nel while
	mpf_init(sum);
	
	mpf_t N_float;	//float per il castin di N
	mpf_init(N_float);
	mpf_set_z(N_float, N);
	
	mpf_t Ndivx;
	mpf_init(Ndivx);
	mpf_t f_Ndivx;
	mpf_init(f_Ndivx);
	
	mpf_t sumdiv2;	//float per la somma nel while
	mpf_init(sumdiv2);
	
	
	while(1)
	{
		
		mpf_div(Ndivx, N_float, x);			//divisione tra N(float) e x
		mpf_floor(f_Ndivx, Ndivx);			//floor della divisione tra N e x
		mpf_add(sum, x, f_Ndivx);			//somma tra x e il floor di N_float/x
		mpf_div_ui(sumdiv2, sum, 2);		//divisione di sum per 2
		mpf_floor(y, sumdiv2);				//floor della somma/2
		
		//printf("y : %f\n", y);
		
		if(mpf_cmp(y, x) >= 0)
		{	
			//printf("END\n");
			//Assegno il risultato
			mpz_set_f(result, x);

			//Fase di rilascio variabili
			mpf_clear(sumdiv2);
			mpf_clear(f_Ndivx);
			mpf_clear(Ndivx);			
			mpf_clear(N_float);
			mpf_clear(sum);
			mpf_clear(f_base);
			mpz_clear(base);
			mpf_clear(y);
			mpf_clear(x);			
			
			return;
		}
		else
		{
			mpf_set(x, y);
		}
	}
}

//restituisce 1 se tutto ok
//restituisce 0 se fallisce
int cornacchia_GMP(resultset* r, mpz_t p, mpz_t D)
{	
	int i = 0;
	for(i = 0; i < 4; i++)
		mpz_init(r->results[i]);
	r->num_elements = 0;


	//assert(D > -4*p && D < 0);
	//assert(D_pos%4 == 0 || D_pos%4 == 1);

	mpz_t neg_p4;
	mpz_init(neg_p4);
	mpz_mul_ui(neg_p4, p, 4);	
	mpz_neg(neg_p4, neg_p4); //neg_p4 = -4*p
	//gmp_printf("%Zd\n", neg_p4);
	//gmp_printf("%Zd\n", D);
	
	if(mpz_cmp(D, neg_p4) <= 0 || mpz_cmp_ui(D, 0) >= 0) //intervallo valido: -4*p < D < 0
	{
		for(i = 0; i < 4; i++)
			mpz_clear(r->results[i]);
		r->num_elements = 0;

		mpz_clear(neg_p4);
		return 0;
	}
	
	//assert(mpz_cmp(D, neg_p4) > 0 && mpz_cmp_ui(D, 0) < 0); //D > -4*p && D < 0
	
	mpz_t D_mod;
	mpz_init(D_mod);
	mpz_mod_ui(D_mod, D, 4);	

	if(mpz_cmp_ui(D_mod, 0) != 0 && mpz_cmp_ui(D_mod, 1) != 0)	
	{
		for(i = 0; i < 4; i++)
			mpz_clear(r->results[i]);
		r->num_elements = 0;

		mpz_clear(D_mod);
		mpz_clear(neg_p4);
		return 0;
	}
	
	//assert(mpz_cmp_ui(D_mod, 0) == 0 || mpz_cmp_ui(D_mod, 1) == 0);
	

	//1. [Case p = 2] ---- 4*2 = (rad(D+8))^2 + 1^2 * |D| per D = -4 viene, ma se fai D = -4 = 0 non funziona!Quindi lascia negativo
	if(mpz_cmp_ui(p, 2) == 0) { 	//if p==2
	
		mpz_t newD;
		mpz_init(newD);
		mpz_add_ui(newD, D, 8);		//newD = D+8
		
		if(mpz_perfect_square_p(newD) != 0){
			//printf("a\n");

			mpz_t r0;
			mpz_init(r0);
			mpz_sqrt(r0, newD);	
			mpz_set(r->results[0], r0);				
			mpz_set_ui(r->results[1], 1);
			r->num_elements = 2;

			mpz_clear(r0);
			mpz_clear(newD);
			mpz_clear(D_mod);
			mpz_clear(neg_p4);

			return 1;	
		}
	
		else{		// Return empty: no solution.
			//printf("b\n");

			for(i = 0; i < 4; i++)
				mpz_clear(r->results[i]);
			r->num_elements = 0;
			
			mpz_clear(newD);
			mpz_clear(D_mod);
			mpz_clear(neg_p4);

			return 0;
		}

	}
	
	//2. [Test for solvability]
	
	if(mpz_jacobi(D, p) < 1){
		//printf("c\n");
		for(i = 0; i < 4; i++)
			mpz_clear(r->results[i]);
		r->num_elements = 0;

		mpz_clear(D_mod);
		mpz_clear(neg_p4);

		return 0;	
	}	
	
	//3. [Initial square root]
	mpz_t x_zero;
	mpz_init(x_zero);
	
	int ret = radix_GMP(x_zero, p, D);
	//printf("retRADIXGMP: %d\n", ret);
	//gmp_printf("x_zero: %Zd\n", x_zero);
	//gmp_printf("D: %Zd\n", D);
	//gmp_printf("p: %Zd\n", p);
	//getchar();
	if(ret == 0)
	{
		for(i = 0; i < 4; i++)
			mpz_clear(r->results[i]);
		r->num_elements = 0;

		mpz_clear(x_zero);
		mpz_clear(D_mod);
		mpz_clear(neg_p4);
		return 0;
	}

	//gmp_printf("x_zero: %Zd\n", x_zero);

	mpz_t op;
	mpz_init(op);
	mpz_t xzero2;
	mpz_init(xzero2);
	mpz_t p4;
	mpz_init(p4);
		
	mpz_mul(xzero2, x_zero, x_zero);	//xzero2 = x_zero*x_zero
	mpz_mul_ui(p4, p, 4);				//p4 = 4*p	
	mpz_mod(op, xzero2, p4);			//op = xzero2 % p4
	
	//gmp_printf("op: %Zd\n", op);
	
	mpz_t diff;
	mpz_init(diff);
	mpz_sub(diff, op, p4);
	
	if(mpz_cmp(diff, D) != 0)			//op - 4*p != D	
		mpz_sub(x_zero, p, x_zero);		//x_zero = p - x_zero	//Ensure x^2 ≡ D (mod 4p).			
	
	//gmp_printf("x_zero: %Zd\n", x_zero);

	
	//4. [Initialize Euclid chain]
	mpz_t a;
	mpz_init(a);
	mpz_t b;
	mpz_init(b);
	mpz_t c;
	mpz_init(c);
	
	mpz_mul_ui(a, p, 2);	//a = 2*p	//numeri grandi...
	mpz_set(b, x_zero);		//b = x_zero
	
	integerpart_GMP(c, p4);	//integer_part_sqrt(4*p)	//[2*sqrt(p)]	
	//mpz_sqrt (c, p4);	


	//gmp_printf("4p: %Zd\n", p4);
	//gmp_printf("a: %Zd\n", a);
	//gmp_printf("b: %Zd\n", b);
	//gmp_printf("c: %Zd\n", c);
	
	//5. [Euclid chain]
	mpz_t a_old;
	mpz_init(a_old);
	
	while(mpz_cmp(b, c) > 0){	//while(b > c)
		mpz_set(a_old, a);	//a_old = a;
		mpz_set(a, b);		//a = b
		mpz_mod(b, a_old, b);	//b = a_old % b
		//gmp_printf("a: %Zd\n", a);
		//gmp_printf("b: %Zd\n", b);
	}

	//6. [Final report]
	
	mpz_t t;
	mpz_init(t);
	mpz_t powb2;
	mpz_init(powb2);
	
	mpz_pow_ui(powb2, b, 2);	//pow(b,2)
	mpz_sub(t, p4, powb2);		//t = 4*p - pow(b,2) //numeri grandi...
	
	mpz_t D_abs;
	mpz_init(D_abs);
	mpz_abs(D_abs, D);			//D_abs = abs(D)

	mpz_t t_Dabs;
	mpz_init(t_Dabs);
	mpz_mod(t_Dabs, t, D_abs);
	
	//gmp_printf("t: %Zd\n", t);
	//gmp_printf("D: %Zd\n", D);
	//gmp_printf("D_ABS: %Zd\n", D_abs);

	if(mpz_cmp_ui(t_Dabs, 0) != 0)	//if(t%D_abs != 0)
	{
		//printf("d\n");
		for(i = 0; i < 4; i++)
			mpz_clear(r->results[i]);
		r->num_elements = 0;

		mpz_clear(t_Dabs);
		mpz_clear(D_abs);
		mpz_clear(powb2);
		mpz_clear(t);
		mpz_clear(a_old);
		mpz_clear(c);
		mpz_clear(b);
		mpz_clear(a);
		mpz_clear(diff);
		mpz_clear(p4);
		mpz_clear(xzero2);
		mpz_clear(op);
		mpz_clear(x_zero);
		mpz_clear(D_mod);
		mpz_clear(neg_p4);

		return 0;	
	}

	mpz_t temp_div;
	mpz_init(temp_div);
	mpz_tdiv_q(temp_div, t, D_abs);

	if(mpz_perfect_square_p(temp_div) == 0){
		//printf("e\n");
		for(i = 0; i < 4; i++)
			mpz_clear(r->results[i]);
		r->num_elements = 0;	

		mpz_clear(temp_div);
		mpz_clear(t_Dabs);
		mpz_clear(D_abs);
		mpz_clear(powb2);
		mpz_clear(t);
		mpz_clear(a_old);
		mpz_clear(c);
		mpz_clear(b);
		mpz_clear(a);
		mpz_clear(diff);
		mpz_clear(p4);
		mpz_clear(xzero2);
		mpz_clear(op);
		mpz_clear(x_zero);
		mpz_clear(D_mod);
		mpz_clear(neg_p4);
	
		return 0;	
	}

	mpz_t temp_sqrt;
	mpz_init(temp_sqrt);
	mpz_sqrt(temp_sqrt, temp_div);	

	mpz_set(r->results[0], b);				
	mpz_set(r->results[1], temp_sqrt);
	mpz_neg(r->results[2], b);
	mpz_neg(r->results[3], temp_sqrt);

	r->num_elements = 4;

	//printf("f\n");

	mpz_clear(temp_sqrt);
	mpz_clear(temp_div);
	mpz_clear(t_Dabs);
	mpz_clear(D_abs);
	mpz_clear(powb2);
	mpz_clear(t);
	mpz_clear(a_old);
	mpz_clear(c);
	mpz_clear(b);
	mpz_clear(a);
	mpz_clear(diff);
	mpz_clear(p4);
	mpz_clear(xzero2);
	mpz_clear(op);
	mpz_clear(x_zero);
	mpz_clear(D_mod);
	mpz_clear(neg_p4);

	return 1;

}
