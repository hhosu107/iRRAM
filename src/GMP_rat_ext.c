/*

GMP_rat_ext.c -- extension to the GMP rational functions for the 
                 iRRAM library with backend GMP
 
Copyright (C) 2001/2003 Norbert Mueller
 
This file is part of the iRRAM Library.
 
The iRRAM Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.
 
The iRRAM Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.
 
You should have received a copy of the GNU Library General Public License
along with the iRRAM Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. 
*/

#include <iRRAM/GMP_intrat.h>

#include <strings.h>

#ifndef DEBUG
#define DEBUG 0
#endif
#if DEBUG > 0
  int DEBUG_OP = 1;
  unsigned int op_count=0;
  int minshowop = DEBUG ;
#endif


iRRAM_TLS mpq_ptr rat_gmp_FreeVarsi[rat_MaxFreeVars];
iRRAM_TLS int rat_gmp_FreeVarCount=0L;
iRRAM_TLS int rat_gmp_var_count=0;

char* rat_gmp_swritee(const mpq_t z, int w)
{
  char *n, *s;
  int l;
  if (w<1)w=1;
  s=malloc(w+1);
  n=mpq_get_str(NULL, 10, z);
  l=strlen(n);
  if (l>w){
    strncpy(s,n,w);
    s[w-1]='*';
    s[w]='\0';
  }else{
    memset(s,' ',w-l);
    strncpy(&s[w-l],n,l);
    s[w]='\0';
  } 
  free(n); 
  return s;
}


void rat_gmp_shift(const mpq_t z1, mpq_t z, int n)
{
	mpz_ptr num,den;
	num=int_gmp_init();
	den=int_gmp_init();
	mpq_get_num(num,z1);
	mpq_get_den(den,z1);

  	if (n>=0) mpz_mul_2exp(num, num, n);
	else mpz_mul_2exp(den, den, -n);	

	mpq_set_num(z,num);
	mpq_set_den(z,den);
	mpq_canonicalize(z);

	int_gmp_free(num);
	int_gmp_free(den);
}

void rat_gmp_string_2_rat(mpq_t z, const char* s)
{
	mpq_set_str(z,s,10);
	mpq_canonicalize(z);
}

