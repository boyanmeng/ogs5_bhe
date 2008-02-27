/* Copyright (C) 2002-2007 The SSI Project. All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
   3. Neither the name of the project nor the names of its contributors 
      may be used to endorse or promote products derived from this software 
      without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE SCALABLE SOFTWARE INFRASTRUCTURE PROJECT
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
   TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE SCALABLE SOFTWARE INFRASTRUCTURE
   PROJECT BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
   OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef HAVE_CONFIG_H
	#include "config.h"
#else
#ifdef HAVE_CONFIG_WIN32_H
	#include "config_win32.h"
#endif
#endif

#include <math.h>
#ifdef _OPENMP
	#include <omp.h>
#endif
#ifdef USE_MPI
	#include <mpi.h>
#endif
#include "lislib.h"

/************************************************
 * lis_vector_dot			v   <- x' * y
 * lis_vector_nrm2			v   <- ||x||_2
 * lis_vector_sum			v   <- sum x_i
 ************************************************/

/**********************/
/* v <- x' * y        */
/**********************/
#undef __FUNC__
#define __FUNC__ "lis_vector_dot"
int lis_vector_dot(LIS_VECTOR vx, LIS_VECTOR vy, LIS_SCALAR *val)
{
	int i,n;
	LIS_SCALAR dot;
	LIS_SCALAR *x,*y;
	LIS_SCALAR tmp;
	#ifdef _OPENMP
		int nprocs,my_rank;
	#endif
	#ifdef USE_MPI
		MPI_Comm   comm;
	#endif

	LIS_DEBUG_FUNC_IN;

	n = vx->n;
	#ifndef NO_ERROR_CHECK
		if( n!=vy->n )
		{
			LIS_SETERR(LIS_ERR_ILL_ARG,"length of vector x and y is not equal\n");
			return LIS_ERR_ILL_ARG;
		}
	#endif

	x      = vx->value;
	y      = vy->value;
	#ifdef USE_MPI
		comm   = vx->comm;
	#endif
	#ifdef _OPENMP
		nprocs = omp_get_max_threads();
		#pragma omp parallel private(i,tmp,my_rank)
		{
			my_rank = omp_get_thread_num();
			tmp     = 0.0;
			#ifdef USE_VEC_COMP
		    #pragma cdir nodep
			#endif
			#pragma omp for
			for(i=0; i<n; i++)
			{
				tmp += x[i]*y[i];
			}
			lis_vec_tmp[my_rank*LIS_VEC_TMP_PADD] = tmp;
		}
		dot = 0.0;
		for(i=0;i<nprocs;i++)
		{
			dot += lis_vec_tmp[i*LIS_VEC_TMP_PADD];
		}
	#else
		dot  = 0.0;
		#ifdef USE_VEC_COMP
	    #pragma cdir nodep
		#endif
		for(i=0; i<n; i++)
		{
			dot += x[i]*y[i];
		}
	#endif
	#ifdef USE_MPI
		MPI_Allreduce(&dot,&tmp,1,MPI_DOUBLE,MPI_SUM,comm);
		*val = tmp;
	#else
		*val = dot;
	#endif

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

/**********************/
/* v <- ||x||_2       */
/**********************/
#undef __FUNC__
#define __FUNC__ "lis_vector_nrm2"
int lis_vector_nrm2(LIS_VECTOR vx, LIS_REAL *val)
{
	int i,n;
	LIS_SCALAR dot;
	LIS_SCALAR *x;
	LIS_SCALAR tmp;
	#ifdef _OPENMP
		int nprocs,my_rank;
	#endif
	#ifdef USE_MPI
		MPI_Comm   comm;
	#endif

	LIS_DEBUG_FUNC_IN;

	n      = vx->n;

	x      = vx->value;
	#ifdef USE_MPI
		comm   = vx->comm;
	#endif
	#ifdef _OPENMP
		nprocs = omp_get_max_threads();
		#pragma omp parallel private(i,tmp,my_rank)
		{
			my_rank = omp_get_thread_num();
			tmp     = 0.0;
			#ifdef USE_VEC_COMP
		    #pragma cdir nodep
			#endif
			#pragma omp for
			for(i=0; i<n; i++)
			{
				tmp += x[i]*x[i];
			}
			lis_vec_tmp[my_rank*LIS_VEC_TMP_PADD] = tmp;
		}
		dot = 0.0;
		for(i=0;i<nprocs;i++)
		{
			dot += lis_vec_tmp[i*LIS_VEC_TMP_PADD];
		}
	#else
		dot  = 0.0;
		#ifdef USE_VEC_COMP
	    #pragma cdir nodep
		#endif
		for(i=0; i<n; i++)
		{
			dot += x[i]*x[i];
		}
	#endif
	#ifdef USE_MPI
		MPI_Allreduce(&dot,&tmp,1,MPI_DOUBLE,MPI_SUM,comm);
		*val = sqrt(tmp);
	#else
		*val = sqrt(dot);
	#endif

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

/**********************/
/* v <- ||x||_2       */
/**********************/
#undef __FUNC__
#define __FUNC__ "lis_vector_nrm1"
int lis_vector_nrm1(LIS_VECTOR vx, LIS_REAL *val)
{
	int i,n;
	LIS_SCALAR sum;
	LIS_SCALAR *x;
	#ifdef _OPENMP
		int nprocs,my_rank;
		LIS_SCALAR tmp;
	#endif
	#ifdef USE_MPI
		MPI_Comm   comm;
	#endif

	LIS_DEBUG_FUNC_IN;

	n   = vx->n;

	x      = vx->value;
	#ifdef USE_MPI
		comm   = vx->comm;
	#endif
	sum    = 0.0;
	#ifdef _OPENMP
		nprocs = omp_get_max_threads();
		#pragma omp parallel private(i,tmp,my_rank)
		{
			my_rank = omp_get_thread_num();
			tmp     = 0.0;
			#ifdef USE_VEC_COMP
		    #pragma cdir nodep
			#endif
			#pragma omp for
			for(i=0; i<n; i++)
			{
				tmp += fabs(x[i]);
			}
			lis_vec_tmp[my_rank*LIS_VEC_TMP_PADD] = tmp;
		}
		for(i=0;i<nprocs;i++)
		{
			sum += lis_vec_tmp[i*LIS_VEC_TMP_PADD];
		}
	#else
		#ifdef USE_VEC_COMP
	    #pragma cdir nodep
		#endif
		for(i=0; i<n; i++)
		{
			sum += fabs(x[i]);
		}
	#endif
	#ifdef USE_MPI
		MPI_Allreduce(&sum,val,1,MPI_DOUBLE,MPI_SUM,comm);
	#else
		*val = sum;
	#endif

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

/**********************/
/* v <- sum x_i       */
/**********************/
#undef __FUNC__
#define __FUNC__ "lis_vector_sum"
int lis_vector_sum(LIS_VECTOR vx, LIS_SCALAR *val)
{
	int i,n;
	LIS_SCALAR sum;
	LIS_SCALAR *x;
	#ifdef _OPENMP
		int nprocs,my_rank;
		LIS_SCALAR tmp;
	#endif
	#ifdef USE_MPI
		MPI_Comm   comm;
	#endif

	LIS_DEBUG_FUNC_IN;

	n   = vx->n;

	x      = vx->value;
	#ifdef USE_MPI
		comm   = vx->comm;
	#endif
	sum    = 0.0;
	#ifdef _OPENMP
		nprocs = omp_get_max_threads();
		#pragma omp parallel private(i,tmp,my_rank)
		{
			my_rank = omp_get_thread_num();
			tmp     = 0.0;
			#ifdef USE_VEC_COMP
		    #pragma cdir nodep
			#endif
			#pragma omp for
			for(i=0; i<n; i++)
			{
				tmp += x[i];
			}
			lis_vec_tmp[my_rank*LIS_VEC_TMP_PADD] = tmp;
		}
		for(i=0;i<nprocs;i++)
		{
			sum += lis_vec_tmp[i*LIS_VEC_TMP_PADD];
		}
	#else
		#ifdef USE_VEC_COMP
	    #pragma cdir nodep
		#endif
		for(i=0; i<n; i++)
		{
			sum += x[i];
		}
	#endif
	#ifdef USE_MPI
		MPI_Allreduce(&sum,val,1,MPI_DOUBLE,MPI_SUM,comm);
	#else
		*val = sum;
	#endif

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

