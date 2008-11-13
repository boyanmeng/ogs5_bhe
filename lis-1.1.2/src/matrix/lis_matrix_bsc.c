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

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#ifdef _OPENMP
	#include <omp.h>
#endif
#ifdef USE_MPI
	#include <mpi.h>
#endif
#include "lislib.h"

/************************************************
 * lis_matrix_set
 * lis_matrix_malloc
 * lis_matrix_copy
 * lis_matrix_convert
 * lis_matrix_get_diagonal
 * lis_matrix_scaling
 * lis_matrix_scaling_symm
 * lis_matrix_normf
 * lis_matrix_transpose
 ************************************************/

#undef __FUNC__
#define __FUNC__ "lis_matrix_set_bsc"
int lis_matrix_set_bsc(int bnr, int bnc, int bnnz, int *bptr, int *bindex, LIS_SCALAR *value, LIS_MATRIX A)
{
	int err;

	LIS_DEBUG_FUNC_IN;

	err = lis_matrix_check(A,LIS_MATRIX_CHECK_SET);
	if( err ) return err;

	A->bptr        = bptr;
	A->bindex      = bindex;
	A->value       = value;
	A->is_copy     = LIS_FALSE;
	A->status      = -LIS_MATRIX_BSC;
	A->is_block    = LIS_TRUE;
	A->bnnz        = bnnz;
	A->nr          = (A->n-1)/bnr+1;
	A->nc          = (A->gn-1)/bnc+1;
	if( A->n==A->np )
	{
		A->nc      = 1 + (A->n - 1)/bnc;
		A->pad     = (bnc - A->n%bnc)%bnc;
	}
	else
	{
		A->nc      = 2 + (A->n - 1)/bnc + (A->np - A->n - 1)/bnc;
		A->pad     = (bnc - A->n%bnc)%bnc + (bnc - (A->np-A->n)%bnc)%bnc;
	}
	A->bnr         = bnr;
	A->bnc         = bnc;

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_setDLU_bsc"
int lis_matrix_setDLU_bsc(int bnr, int bnc, int lbnnz, int ubnnz, LIS_MATRIX_DIAG D, int *lbptr, int *lbindex, LIS_SCALAR *lvalue,
						  int *ubptr, int *ubindex, LIS_SCALAR *uvalue, LIS_MATRIX A)
{
	int				err;

	LIS_DEBUG_FUNC_IN;

	err = lis_matrix_check(A,LIS_MATRIX_CHECK_SET);
	if( err ) return err;

	A->L = lis_calloc(sizeof(struct LIS_MATRIX_CORE_STRUCT),"lis_matrix_setDLU_bsc::A->L");
	if( A->L==NULL )
	{
		LIS_SETERR_MEM(sizeof(struct LIS_MATRIX_CORE_STRUCT));
		return LIS_OUT_OF_MEMORY;
	}
	A->U = lis_calloc(sizeof(struct LIS_MATRIX_CORE_STRUCT),"lis_matrix_setDLU_bsc::A->U");
	if( A->U==NULL )
	{
		LIS_SETERR_MEM(sizeof(struct LIS_MATRIX_CORE_STRUCT));
		lis_matrix_DLU_destroy(A);
		return LIS_OUT_OF_MEMORY;
	}

	A->D           = D;
	A->L->bnnz     = lbnnz;
	A->L->bptr     = lbptr;
	A->L->bindex   = lbindex;
	A->L->value    = lvalue;
	A->U->bnnz     = ubnnz;
	A->U->bptr     = ubptr;
	A->U->bindex   = ubindex;
	A->U->value    = uvalue;
	A->is_copy     = LIS_FALSE;
	A->status      = -LIS_MATRIX_BSC;
	A->is_splited  = LIS_TRUE;
	A->is_block    = LIS_TRUE;
	A->nr          = (A->n-1)/bnr+1;
	A->nc          = (A->gn-1)/bnc+1;
	if( A->n==A->np )
	{
		A->nc      = 1 + (A->n - 1)/bnc;
		A->pad     = (bnc - A->n%bnc)%bnc;
	}
	else
	{
		A->nc      = 2 + (A->n - 1)/bnc + (A->np - A->n - 1)/bnc;
		A->pad     = (bnc - A->n%bnc)%bnc + (bnc - (A->np-A->n)%bnc)%bnc;
	}
	A->bnr         = bnr;
	A->bnc         = bnc;

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_malloc_bsc"
int lis_matrix_malloc_bsc(int n, int bnr, int bnc, int bnnz, int **bptr, int **bindex, LIS_SCALAR **value)
{
	int		nc;

	LIS_DEBUG_FUNC_IN;

	nc        = 1 + (n -1)/bnc;
	*bptr     = NULL;
	*bindex   = NULL;
	*value    = NULL;

	*bptr = (int *)lis_malloc( (nc+1)*sizeof(int),"lis_matrix_malloc_bsc::bptr" );
	if( *bptr==NULL )
	{
		LIS_SETERR_MEM((nc+1)*sizeof(int));
		lis_free2(3,*bptr,*bindex,*value);
		return LIS_FAILS;
	}
	*bindex = (int *)lis_malloc( bnnz*sizeof(int),"lis_matrix_malloc_bsc::bindex" );
	if( *bindex==NULL )
	{
		LIS_SETERR_MEM(bnnz*sizeof(int));
		lis_free2(3,*bptr,*bindex,*value);
		return LIS_OUT_OF_MEMORY;
	}
	*value = (LIS_SCALAR *)lis_malloc( bnnz*bnr*bnc*sizeof(LIS_SCALAR),"lis_matrix_malloc_bsc::value" );
	if( *value==NULL )
	{
		LIS_SETERR_MEM(bnnz*bnr*bnc*sizeof(LIS_SCALAR));
		lis_free2(3,*bptr,*bindex,*value);
		return LIS_OUT_OF_MEMORY;
	}
	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_elements_copy_bsc"
int lis_matrix_elements_copy_bsc(int n, int bnr, int bnc, int bnnz, int *ptr, int *index, LIS_SCALAR *value,
								 int *o_ptr, int *o_index, LIS_SCALAR *o_value)
{
	int		i,j,k;
	int		nc,bs;

	LIS_DEBUG_FUNC_IN;

	nc  = 1 + (n - 1)/bnc;
	bs  = bnr*bnc;
	#ifdef _OPENMP
	#pragma omp parallel private(i,j,k)
	#endif
	{
		#ifdef _OPENMP
		#pragma omp for
		#endif
		for(i=0;i<nc+1;i++)
		{
			o_ptr[i] = ptr[i];
		}
		#ifdef _OPENMP
		#pragma omp for
		#endif
		for(i=0;i<nc;i++)
		{
			for(j=ptr[i];j<ptr[i+1];j++)
			{
				for(k=0;k<bs;k++)
				{
					o_value[j*bs+k]   = value[j*bs+k];
				}
				o_index[j]   = index[j];
			}
		}
	}

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_copy_bsc"
int lis_matrix_copy_bsc(LIS_MATRIX Ain, LIS_MATRIX Aout)
{
	int			err;
	int			np,bnnz,bnr,bnc;
	int			lbnnz,ubnnz;
	int			*bptr,*bindex;
	int			*lbptr,*lbindex;
	int			*ubptr,*ubindex;
	LIS_SCALAR	*value,*lvalue,*uvalue;
	LIS_MATRIX_DIAG D;

	LIS_DEBUG_FUNC_IN;

	np      = Ain->np;
	bnnz    = Ain->bnnz;
	bnr     = Ain->bnr;
	bnc     = Ain->bnc;
    
	if( Ain->is_splited )
	{
		lbnnz    = Ain->L->bnnz;
		ubnnz    = Ain->U->bnnz;
		lbptr    = NULL;
		lbindex  = NULL;
		lvalue   = NULL;
		ubptr    = NULL;
		ubindex  = NULL;
		uvalue   = NULL;
		D        = NULL;

		err = lis_matrix_malloc_bsc(np,bnr,bnc,lbnnz,&lbptr,&lbindex,&lvalue);
		if( err )
		{
			return err;
		}
		err = lis_matrix_malloc_bsc(np,bnr,bnc,ubnnz,&ubptr,&ubindex,&uvalue);
		if( err )
		{
			lis_free2(6,ubptr,lbptr,ubindex,lbindex,uvalue,lvalue);
			return err;
		}
		err = lis_matrix_diag_duplicateM(Ain,&D);
		if( err )
		{
			lis_free2(6,ubptr,lbptr,ubindex,lbindex,uvalue,lvalue);
			return err;
		}

		lis_matrix_diag_copy(Ain->D,D);
		lis_matrix_elements_copy_bsc(np,bnr,bnc,lbnnz,Ain->L->bptr,Ain->L->bindex,Ain->L->value,lbptr,lbindex,lvalue);
		lis_matrix_elements_copy_bsc(np,bnr,bnc,ubnnz,Ain->U->bptr,Ain->U->bindex,Ain->U->value,ubptr,ubindex,uvalue);

		err = lis_matrix_setDLU_bsc(bnr,bnc,lbnnz,ubnnz,D,lbptr,lbindex,lvalue,ubptr,ubindex,uvalue,Aout);
		if( err )
		{
			lis_free2(6,ubptr,lbptr,ubindex,lbindex,uvalue,lvalue);
			return err;
		}
	}
	if( !Ain->is_splited || (Ain->is_splited && Ain->is_save) )
	{
		bptr    = NULL;
		bindex  = NULL;
		value   = NULL;

		err = lis_matrix_malloc_bsc(np,bnr,bnc,bnnz,&bptr,&bindex,&value);
		if( err )
		{
			return err;
		}

		lis_matrix_elements_copy_bsc(np,bnr,bnc,bnnz,Ain->bptr,Ain->bindex,Ain->value,bptr,bindex,value);

		err = lis_matrix_set_bsc(bnr,bnc,bnnz,bptr,bindex,value,Aout);
		if( err )
		{
			lis_free2(3,bptr,bindex,value);
			return err;
		}
	}

	err = lis_matrix_assemble(Aout);
	if( err )
	{
		lis_matrix_storage_destroy(Aout);
		return err;
	}
	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_convert_crs2bsc"
int lis_matrix_convert_ccs2bsc(LIS_MATRIX Ain, LIS_MATRIX Aout)
{
	int			i,j,k,n,bnr,bnc;
	int			ii,jj,kk,pad;
	int			bnnz,bj,nr,nc,jpos,nnz,ij,kv,bi;
	int			err;
	int			np,nprocs,my_rank;
	int			*iw,*iw2;
	int			*bptr,*bindex;
	LIS_SCALAR	*value;

	LIS_DEBUG_FUNC_IN;

	bnr   = Aout->conv_bnr;
	bnc   = Aout->conv_bnc;

	n       = Ain->n;
	np      = Ain->np;
	nr      = 1 + (n - 1)/bnr;
	pad     = (bnc - n%bnc)%bnc;
	if( n==np )
	{
		nc      = 1 + (n - 1)/bnc;
	}
	else
	{
		nc      = 2 + (n - 1)/bnc + (np - n - 1)/bnc;
	}

	bptr    = NULL;
	bindex  = NULL;
	value   = NULL;
	iw      = NULL;
	iw2     = NULL;

	bptr = (int *)lis_malloc( (nc+1)*sizeof(int),"lis_matrix_convert_ccs2bsc::bptr" );
	if( bptr==NULL )
	{
		LIS_SETERR_MEM((nc+1)*sizeof(int));
		lis_free2(5,bptr,bindex,value,iw,iw2);
		return LIS_OUT_OF_MEMORY;
	}

	#ifdef _OPENMP
		nprocs = omp_get_max_threads();
	#else
		nprocs = 1;
	#endif
	iw    = (int *)lis_malloc( nprocs*nr*sizeof(int),"lis_matrix_convert_ccs2bsc::iw" );
	iw2   = (int *)lis_malloc( nprocs*nr*sizeof(int),"lis_matrix_convert_ccs2bsc::iw2" );

	#ifdef _OPENMP
	#pragma omp parallel private(i,k,ii,j,bj,kk,ij,jj,kv,jpos,my_rank)
	#endif
	{
		#ifdef _OPENMP
			my_rank = omp_get_thread_num();
		#else
			my_rank = 0;
		#endif
		memset(&iw[my_rank*nr],0,nr*sizeof(int));

		#ifdef _OPENMP
		#pragma omp for
		#endif
		for(i=0;i<nc;i++)
		{
			k = 0;
			kk   = bnc*i;
			jj   = 0;
			#ifdef USE_MPI
				for(ii=0;ii+kk<np&&ii<bnc;ii++)
				{
					for(j=Ain->ptr[kk+ii];j<Ain->ptr[kk+ii+1];j++)
					{
						bj   = Ain->index[j]/bnr;
						jpos = iw[my_rank*nr + bj];
						if( jpos==0 )
						{
							iw[my_rank*nr + bj] = 1;
							iw2[my_rank*nr + jj] = bj;
							jj++;
						}
					}
				}
			#else
				for(ii=0;ii+kk<np&&ii<bnc;ii++)
				{
					for(j=Ain->ptr[kk+ii];j<Ain->ptr[kk+ii+1];j++)
					{
						bj   = Ain->index[j]/bnr;
						jpos = iw[my_rank*nr + bj];
						if( jpos==0 )
						{
							iw[my_rank*nr + bj] = 1;
							iw2[my_rank*nr + jj] = bj;
							jj++;
						}
					}
				}
			#endif
			for(bj=0;bj<jj;bj++)
			{
				k++;
				ii = iw2[my_rank*nr + bj];
				iw[my_rank*nr + ii]=0;
			}
			bptr[i+1] = k;
		}
	}

	bptr[0] = 0;
	for(i=0;i<nc;i++)
	{
		bptr[i+1] += bptr[i];
	}
	bnnz = bptr[nc];
	nnz  = bnnz*bnr*bnc;
	
	bindex = (int *)lis_malloc( bnnz*sizeof(int),"lis_matrix_convert_ccs2bsc::bindex" );
	if( bindex==NULL )
	{
		LIS_SETERR_MEM((nr+1)*sizeof(int));
		lis_free2(5,bptr,bindex,value,iw,iw2);
		return LIS_OUT_OF_MEMORY;
	}
	value = (LIS_SCALAR *)lis_malloc( nnz*sizeof(LIS_SCALAR),"lis_matrix_convert_ccs2bsc::value" );
	if( value==NULL )
	{
		LIS_SETERR_MEM(nnz*sizeof(LIS_SCALAR));
		lis_free2(5,bptr,bindex,value,iw,iw2);
		return LIS_OUT_OF_MEMORY;
	}

	/* convert bsc */
	#ifdef _OPENMP
	#pragma omp parallel private(bi,i,ii,k,j,bj,jpos,kv,kk,ij,jj,my_rank)
	#endif
	{
		#ifdef _OPENMP
			my_rank = omp_get_thread_num();
		#else
			my_rank = 0;
		#endif
		memset(&iw[my_rank*nr],0,nr*sizeof(int));

		#ifdef _OPENMP
		#pragma omp for
		#endif
		for(bi=0;bi<nc;bi++)
		{
			i  = bi*bnc;
			ii = 0;
			kk = bptr[bi];
			while( i+ii<np && ii<=bnc-1 )
			{
				for( k=Ain->ptr[i+ii];k<Ain->ptr[i+ii+1];k++)
				{
					j    = Ain->index[k];
					bj   = j/bnr;
					j    = j%bnr;
					jpos = iw[my_rank*nr + bj];
					if( jpos==0 )
					{
						kv                  = kk * bnr * bnc;
						iw[my_rank*nr + bj] = kv+1;
						bindex[kk]          = bj;
						for(jj=0;jj<bnr*bnc;jj++) value[kv+jj] = 0.0;
						ij = j + ii*bnc;
						value[kv+ij]   = Ain->value[k];
						kk = kk+1;
					}
					else
					{
						ij = j + ii*bnc;
						value[jpos+ij-1]   = Ain->value[k];
					}
				}
				ii = ii+1;
			}
			for(j=bptr[bi];j<bptr[bi+1];j++)
			{
				iw[my_rank*nr + bindex[j]] = 0;
			}
		}
	}
	lis_free2(2,iw,iw2);

	err = lis_matrix_set_bsc(bnr,bnc,bnnz,bptr,bindex,value,Aout);
	if( err )
	{
		lis_free2(3,bptr,bindex,value);
		return err;
	}
	Aout->pad_comm = pad;
	err = lis_matrix_assemble(Aout);
	if( err )
	{
		lis_matrix_storage_destroy(Aout);
		return err;
	}
	#ifdef USE_MPI
		Aout->commtable->pad = pad;
		MPI_Barrier(Ain->comm);
	#endif
	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_convert_bsc2crs"
int lis_matrix_convert_bsc2crs(LIS_MATRIX Ain, LIS_MATRIX Aout)
{
	int			i,j,k,l;
	int			nr,nc,bnr,bnc,bs,bi,bj;
	int			err;
	int			n,nnz,is,nprocs,my_rank;
	int			*iw,*ptr,*index;
	LIS_SCALAR	*value;

	LIS_DEBUG_FUNC_IN;

	n       = Ain->n;
	nr      = Ain->nr;
	nc      = Ain->nc;
	bnr     = Ain->bnr;
	bnc     = Ain->bnc;
	bs      = bnr*bnc;
	is      = Ain->is;
	#ifdef _OPENMP
		nprocs  = omp_get_max_threads();
	#else
		nprocs  = 1;
	#endif


	iw      = NULL;
	ptr     = NULL;
	index   = NULL;
	value   = NULL;

	iw = (int *)lis_malloc( nprocs*n*sizeof(int),"lis_matrix_convert_bsc2crs::iw" );
	if( iw==NULL )
	{
		LIS_SETERR_MEM(nprocs*n*sizeof(int));
		return LIS_OUT_OF_MEMORY;
	}
	ptr = (int *)lis_malloc( (n+1)*sizeof(int),"lis_matrix_convert_bsc2crs::ptr" );
	if( ptr==NULL )
	{
		LIS_SETERR_MEM((n+1)*sizeof(int));
		lis_free2(4,ptr,index,value,iw);
		return LIS_OUT_OF_MEMORY;
	}

	/* check nnz */
	#ifdef _OPENMP
	#pragma omp parallel private(i,j,bi,bj,my_rank)
	#endif
	{
		#ifdef _OPENMP
			my_rank = omp_get_thread_num();
		#else
			my_rank = 0;
		#endif
		memset(&iw[my_rank*n],0,n*sizeof(int));
		#ifdef _OPENMP
		#pragma omp for
		#endif
		for(bj=0;bj<nc;bj++)
		{
			for(j=0;j<bnc;j++)
			{
				for(bi=Ain->bptr[bj];bi<Ain->bptr[bj+1];bi++)
				{
					for(i=0;i<bnr;i++)
					{
						if( Ain->value[bi*bs + j*bnr + i] != (LIS_SCALAR)0.0 )
						{
							iw[my_rank*n + Ain->bindex[bi]*bnr + i]++;
						}
					}
				}
			}
		}
		#ifdef _OPENMP
		#pragma omp for
		#endif
		for(i=0;i<n;i++)
		{
			j = 0;
			for(k=0;k<nprocs;k++)
			{
				j += iw[k*n + i];
			}
			ptr[i+1] = j;
		}
	}

	ptr[0] = 0;
	for(i=0;i<n;i++)
	{
		ptr[i+1] += ptr[i];
	}
	nnz = ptr[n];

	index = (int *)lis_malloc( nnz*sizeof(int),"lis_matrix_convert_bsc2crs::index" );
	if( index==NULL )
	{
		lis_free2(4,ptr,index,value,iw);
		LIS_SETERR_MEM(nnz*sizeof(int));
		return LIS_OUT_OF_MEMORY;
	}
	value = (LIS_SCALAR *)lis_malloc( nnz*sizeof(LIS_SCALAR),"lis_matrix_convert_bsc2crs::value" );
	if( value==NULL )
	{
		lis_free2(4,ptr,index,value,iw);
		LIS_SETERR_MEM(nnz*sizeof(LIS_SCALAR));
		return LIS_OUT_OF_MEMORY;
	}

	/* convert crs */
	#ifdef _OPENMP
	#pragma omp parallel private(i,j,bi,bj,k,l,my_rank)
	#endif
	{
		#ifdef _OPENMP
			my_rank = omp_get_thread_num();
		#else
			my_rank = 0;
		#endif
		#ifdef _OPENMP
		#pragma omp for
		#endif
		for(i=0;i<n;i++)
		{
			k = ptr[i];
			for(j=0;j<nprocs;j++)
			{
				l = iw[j*n + i];
				iw[j*n + i] = k;
				k = k + l;
			}
		}
		#ifdef _OPENMP
		#pragma omp for
		#endif
		for(bj=0;bj<nc;bj++)
		{
			for(j=0;j<bnc;j++)
			{
				if( bj*bnc+j==n ) break;
				for(bi=Ain->bptr[bj];bi<Ain->bptr[bj+1];bi++)
				{
					for(i=0;i<bnr;i++)
					{
						if( Ain->value[bi*bs + j*bnr + i] != (LIS_SCALAR)0.0 )
						{
							k        = iw[my_rank*n + Ain->bindex[bi]*bnr + i]++;
							value[k] = Ain->value[bi*bs + j*bnr + i];
							index[k] = bj*bnc + j;
						}
					}
				}
			}
		}
	}

	err = lis_matrix_set_crs(nnz,ptr,index,value,Aout);
	if( err )
	{
		lis_free2(4,ptr,index,value,iw);
		return err;
	}
	Aout->pad      = 0;
	Aout->pad_comm = 0;
	err = lis_matrix_assemble(Aout);
	if( err )
	{
		lis_matrix_storage_destroy(Aout);
		return err;
	}
	#ifdef USE_MPI
		Aout->commtable->pad = 0;
	#endif
	lis_free(iw);
	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_get_diagonal_bsc"
int lis_matrix_get_diagonal_bsc(LIS_MATRIX A, LIS_SCALAR d[])
{
	int i,j,k,bi,bj,bjj,nr,nc;
	int bnr,bnc,bs;
	int n;

	LIS_DEBUG_FUNC_IN;

	n   = A->n;
	nr  = A->nr;
	nc  = A->nc;
	bnr = A->bnr;
	bnc = A->bnc;
	bs  = bnr*bnc;
	if( A->is_splited )
	{
		#ifdef _OPENMP
		#pragma omp parallel for private(i,j)
		#endif
		for(i=0;i<nr;i++)
		{
			for(j=0;j<bnr;j++)
			{
				d[i*bnr+j] = A->D->value[i*bs+j*bnr+j];
			}
		}
	}
	else
	{
		#ifdef _OPENMP
		#pragma omp parallel for private(bi,bj,bjj,i,j,k)
		#endif
		for(bi=0;bi<nr;bi++)
		{
			k = 0;
			i = bi*bnr;
			for(bj=A->bptr[bi];bj<A->bptr[bi+1];bj++)
			{
				bjj = A->bindex[bj];
				if( i>=bjj*bnc && i<(bjj+1)*bnc )
				{
					for(j=i%bnc;j<bnc&&k<bnr&&i<n;j++)
					{
						d[i] = A->value[bj*bs + j*bnr + k];
						i++;
						k++;
					}
				}
				if( k==bnr ) break;
			}
		}
	}

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_scaling_bsc"
int lis_matrix_scaling_bsc(LIS_MATRIX A, LIS_SCALAR d[])
{
	int i,j;
	int bi,bj,bs;
	int nr,nc;
	int bnr,bnc;
	int n;

	LIS_DEBUG_FUNC_IN;

	n    = A->n;
	bnr  = A->bnr;
	bnc  = A->bnc;
	nr   = A->nr;
	nc   = A->nc;
	bs   = A->bnr*A->bnc;

	if( A->is_splited )
	{
		#ifdef _OPENMP
		#pragma omp parallel for private(bi,bj,i,j)
		#endif
		for(bi=0;bi<nr;bi++)
		{
			for(bj=A->L->bptr[bi];bj<A->L->bptr[bi+1];bj++)
			{
				for(j=0;j<bnc;j++)
				{
					for(i=0;i<bnr;i++)
					{
						A->L->value[bj*bs+j*bnr+i] *= d[bi*bnr+i];
					}
				}
			}
			for(bj=A->U->bptr[bi];bj<A->U->bptr[bi+1];bj++)
			{
				for(j=0;j<bnc;j++)
				{
					for(i=0;i<bnr;i++)
					{
						A->U->value[bj*bs+j*bnr+i] *= d[bi*bnr+i];
					}
				}
			}
			for(j=0;j<bnc;j++)
			{
				for(i=0;i<bnr;i++)
				{
					A->D->value[bi*bs+j*bnr+i] *= d[bi*bnr+i];
				}
			}
		}
	}
	else
	{
		#ifdef _OPENMP
		#pragma omp parallel for private(bi,bj,i,j)
		#endif
		for(bi=0;bi<nr;bi++)
		{
			for(bj=A->bptr[bi];bj<A->bptr[bi+1];bj++)
			{
				for(j=0;j<bnc;j++)
				{
					for(i=0;i<bnr;i++)
					{
						A->value[bj*bs+j*bnr+i] *= d[bi*bnr+i];
					}
				}
			}
		}
	}
	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_scaling_symm_bsc"
int lis_matrix_scaling_symm_bsc(LIS_MATRIX A, LIS_SCALAR d[])
{
	int i,j;
	int bi,bj,bjj,bs;
	int nr,nc;
	int bnr,bnc;
	int n;

	LIS_DEBUG_FUNC_IN;

	n    = A->n;
	bnr  = A->bnr;
	bnc  = A->bnc;
	nr   = A->nr;
	nc   = A->nc;
	bs   = A->bnr*A->bnc;

	if( A->is_splited )
	{
		#ifdef _OPENMP
		#pragma omp parallel for private(bi,bj,i,j)
		#endif
		for(bi=0;bi<nr;bi++)
		{
			for(bj=A->L->bptr[bi];bj<A->L->bptr[bi+1];bj++)
			{
				bjj = A->L->bindex[bj];
				for(j=0;j<bnc;j++)
				{
					for(i=0;i<bnr;i++)
					{
						A->L->value[bj*bs+j*bnr+i] *= d[bi*bnr+i]*d[bjj*bnc+j];
					}
				}
			}
			for(bj=A->U->bptr[bi];bj<A->U->bptr[bi+1];bj++)
			{
				bjj = A->U->bindex[bj];
				for(j=0;j<bnc;j++)
				{
					for(i=0;i<bnr;i++)
					{
						A->U->value[bj*bs+j*bnr+i] *= d[bi*bnr+i]*d[bjj*bnc+j];
					}
				}
			}
			for(j=0;j<bnc;j++)
			{
				for(i=0;i<bnr;i++)
				{
					A->D->value[bi*bs+j*bnr+i] *= d[bi*bnr+i]*d[bi*bnr+i];
				}
			}
		}
	}
	else
	{
		#ifdef _OPENMP
		#pragma omp parallel for private(bi,bj,bjj,i,j)
		#endif
		for(bi=0;bi<nr;bi++)
		{
			for(bj=A->bptr[bi];bj<A->bptr[bi+1];bj++)
			{
				bjj = A->bindex[bj];
				for(j=0;j<bnc;j++)
				{
					for(i=0;i<bnr;i++)
					{
						A->value[bj*bs+j*bnr+i] *= d[bi*bnr+i]*d[bjj*bnc+j];
					}
				}
			}
		}
	}
	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_normf_bsc"
int lis_matrix_normf_bsc(LIS_MATRIX A, LIS_SCALAR *nrm)
{
	int j;
	int bi,bj,bs;
	int nr,nc;
	int bnr,bnc;
	int n;
	LIS_SCALAR sum;

	LIS_DEBUG_FUNC_IN;

	n    = A->n;
	bnr  = A->bnr;
	bnc  = A->bnc;
	nr   = A->nr;
	nc   = A->nc;
	bs   = bnr*bnc;
	sum  = (LIS_SCALAR)0;

	if( A->is_splited )
	{
		#ifdef _OPENMP
		#pragma omp parallel for reduction(+:sum) private(bi,bj,j)
		#endif
		for(bi=0;bi<nr;bi++)
		{
			for(bj=A->L->bptr[bi];bj<A->L->bptr[bi+1];bj++)
			{
				for(j=0;j<bs;j++)
				{
					sum += A->L->value[bj+j]*A->L->value[bj+j];
				}
			}
			for(bj=A->U->bptr[bi];bj<A->U->bptr[bi+1];bj++)
			{
				for(j=0;j<bs;j++)
				{
					sum += A->U->value[bj+j]*A->U->value[bj+j];
				}
			}
		}
	}
	else
	{
		#ifdef _OPENMP
		#pragma omp parallel for reduction(+:sum) private(bi,bj,j)
		#endif
		for(bi=0;bi<nr;bi++)
		{
			for(bj=A->bptr[bi];bj<A->bptr[bi+1];bj++)
			{
				for(j=0;j<bs;j++)
				{
					sum += A->value[bj+j]*A->value[bj+j];
				}
			}
		}
	}
	*nrm = sqrt(sum);
	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_split_bsc"
int lis_matrix_split_bsc(LIS_MATRIX A)
{
	int				i,j,n,np;
	int				bnr,bnc,nr,nc,bs;
	int				nnzl,nnzu;
	int				err;
	int				*lptr,*lindex,*uptr,*uindex;
	LIS_SCALAR		*lvalue,*uvalue;
	LIS_MATRIX_DIAG	D;
	#ifdef _OPENMP
		int				kl,ku;
		int				*liw,*uiw;
	#endif

	LIS_DEBUG_FUNC_IN;

	n        = A->n;
	np       = A->np;
	bnr      = A->bnr;
	bnc      = A->bnc;
	nr       = A->nr;
	nc       = A->nc;
	bs       = A->bnr*A->bnc;
	nnzl     = 0;
	nnzu     = 0;
	D        = NULL;
	lptr     = NULL;
	lindex   = NULL;
	lvalue   = NULL;
	uptr     = NULL;
	uindex   = NULL;
	uvalue   = NULL;

	if( bnr!=bnc )
	{
		LIS_SETERR_IMP;
		return LIS_ERR_NOT_IMPLEMENTED;
	}
	#ifdef _OPENMP
		liw = (int *)lis_malloc((nc+1)*sizeof(int),"lis_matrix_split_bsc::liw");
		if( liw==NULL )
		{
			LIS_SETERR_MEM((nc+1)*sizeof(int));
			return LIS_OUT_OF_MEMORY;
		}
		uiw = (int *)lis_malloc((nc+1)*sizeof(int),"lis_matrix_split_bsc::uiw");
		if( uiw==NULL )
		{
			LIS_SETERR_MEM((nc+1)*sizeof(int));
			lis_free(liw);
			return LIS_OUT_OF_MEMORY;
		}
		#pragma omp parallel for private(i)
		for(i=0;i<nc+1;i++)
		{
			liw[i] = 0;
			uiw[i] = 0;
		}
		#pragma omp parallel for private(i,j)
		for(i=0;i<nc;i++)
		{
			for(j=A->bptr[i];j<A->bptr[i+1];j++)
			{
				if( A->bindex[j]<i )
				{
					liw[i+1]++;
				}
				else if( A->bindex[j]>i )
				{
					uiw[i+1]++;
				}
			}
		}
		for(i=0;i<nc;i++)
		{
			liw[i+1] += liw[i];
			uiw[i+1] += uiw[i];
		}
		nnzl = liw[nc];
		nnzu = uiw[nc];
	#else
		for(i=0;i<nc;i++)
		{
			for(j=A->bptr[i];j<A->bptr[i+1];j++)
			{
				if( A->bindex[j]<i )
				{
					nnzl++;
				}
				else if( A->bindex[j]>i )
				{
					nnzu++;
				}
			}
		}
	#endif

	err = lis_matrix_LU_create(A);
	if( err )
	{
		return err;
	}
	err = lis_matrix_malloc_bsc(np,bnr,bnc,nnzl,&lptr,&lindex,&lvalue);
	if( err )
	{
		return err;
	}
	err = lis_matrix_malloc_bsc(np,bnr,bnc,nnzu,&uptr,&uindex,&uvalue);
	if( err )
	{
		lis_free2(6,lptr,lindex,lvalue,uptr,uindex,uvalue);
		return err;
	}
	err = lis_matrix_diag_duplicateM(A,&D);
	if( err )
	{
		lis_free2(6,lptr,lindex,lvalue,uptr,uindex,uvalue);
		return err;
	}

	#ifdef _OPENMP
		#pragma omp parallel for private(i)
		for(i=0;i<nc+1;i++)
		{
			lptr[i] = liw[i];
			uptr[i] = uiw[i];
		}
		#pragma omp parallel for private(i,j,kl,ku)
		for(i=0;i<nc;i++)
		{
			kl = lptr[i];
			ku = uptr[i];
			for(j=A->bptr[i];j<A->bptr[i+1];j++)
			{
				if( A->bindex[j]<i )
				{
					lindex[kl]   = A->bindex[j];
					memcpy(&lvalue[bs*kl],&A->value[bs*j],bs*sizeof(LIS_SCALAR));;
					kl++;
				}
				else if( A->bindex[j]>i )
				{
					uindex[ku]   = A->bindex[j];
					memcpy(&uvalue[bs*ku],&A->value[bs*j],bs*sizeof(LIS_SCALAR));
					ku++;
				}
				else
				{
					memcpy(&D->value[bs*i],&A->value[bs*j],bs*sizeof(LIS_SCALAR));
				}
			}
		}
		lis_free2(2,liw,uiw);
	#else
		nnzl = 0;
		nnzu = 0;
		lptr[0] = 0;
		uptr[0] = 0;
		for(i=0;i<nc;i++)
		{
			for(j=A->bptr[i];j<A->bptr[i+1];j++)
			{
				if( A->bindex[j]<i )
				{
					lindex[nnzl]   = A->bindex[j];
					memcpy(&lvalue[bs*nnzl],&A->value[bs*j],bs*sizeof(LIS_SCALAR));;
					nnzl++;
				}
				else if( A->bindex[j]>i )
				{
					uindex[nnzu]   = A->bindex[j];
					memcpy(&uvalue[bs*nnzu],&A->value[bs*j],bs*sizeof(LIS_SCALAR));
					nnzu++;
				}
				else
				{
					memcpy(&D->value[bs*i],&A->value[bs*j],bs*sizeof(LIS_SCALAR));
				}
			}
			lptr[i+1] = nnzl;
			uptr[i+1] = nnzu;
		}
	#endif
	A->L->bnr     = bnr;
	A->L->bnc     = bnc;
	A->L->nr      = nr;
	A->L->nc      = nc;
	A->L->bnnz    = nnzl;
	A->L->bptr    = lptr;
	A->L->bindex  = lindex;
	A->L->value   = lvalue;
	A->U->bnr     = bnr;
	A->U->bnc     = bnc;
	A->U->nr      = nr;
	A->U->nc      = nc;
	A->U->bnnz    = nnzu;
	A->U->bptr    = uptr;
	A->U->bindex  = uindex;
	A->U->value   = uvalue;
	A->D          = D;
	A->is_splited = LIS_TRUE;

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_merge_bsc"
int lis_matrix_merge_bsc(LIS_MATRIX A)
{
	int				i,j,n,np,nr,nc;
	int				bnnz,bnr,bnc,bs;
	int				err;
	int				*bptr,*bindex;
	LIS_SCALAR		*value;

	LIS_DEBUG_FUNC_IN;


	n       = A->n;
	np      = A->np;
	nc      = A->nc;
	nr      = A->nr;
	bnr     = A->bnr;
	bnc     = A->bnc;
	bs      = bnr*bnc;
	bptr    = NULL;
	bindex  = NULL;
	value   = NULL;
	bnnz    = A->L->bnnz + A->U->bnnz + nr;

	err = lis_matrix_malloc_bsc(np,bnr,bnc,bnnz,&bptr,&bindex,&value);
	if( err )
	{
		return err;
	}

	bnnz    = 0;
	bptr[0] = 0;
	for(i=0;i<nc;i++)
	{
		for(j=A->L->bptr[i];j<A->L->bptr[i+1];j++)
		{
			bindex[bnnz]   = A->L->bindex[j];
			memcpy(&value[bs*bnnz],&A->L->value[bs*j],bs*sizeof(LIS_SCALAR));;
			bnnz++;
		}
		bindex[bnnz] = i;
		memcpy(&value[bs*bnnz],&A->D->value[bs*i],bs*sizeof(LIS_SCALAR));;
		bnnz++;
		for(j=A->U->bptr[i];j<A->U->bptr[i+1];j++)
		{
			bindex[bnnz]   = A->U->bindex[j];
			memcpy(&value[bs*bnnz],&A->U->value[bs*j],bs*sizeof(LIS_SCALAR));;
			bnnz++;
		}
		bptr[i+1] = bnnz;
	}

	A->bnnz       = bnnz;
	A->bptr       = bptr;
	A->value      = value;
	A->bindex      = bindex;

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_solve_bsc"
int lis_matrix_solve_bsc(LIS_MATRIX A, LIS_VECTOR B, LIS_VECTOR X, int flag)
{
	int i,j,k,ii,jj,nr,bnr,bnc,bs;
	LIS_SCALAR	t0,t1,t2;
	LIS_SCALAR	*b,*x,*w;

	LIS_DEBUG_FUNC_IN;

	nr  = A->nr;
	bnr = A->bnr;
	bnc = A->bnc;
	bs  = A->bnr*A->bnc;
	b   = B->value;
	x   = X->value;

	
	switch(flag)
	{
	case LIS_MATRIX_LOWER:
		switch(bnr)
		{
		case 1:
			for(i=0;i<nr;i++)
			{
				t0 = b[i];
				for(j=A->L->bptr[i];j<A->L->bptr[i+1];j++)
				{
					jj  = A->L->bindex[j];
					t0 -= A->L->value[j] * x[jj];
				}
				x[i] = A->WD->value[i] * t0;
			}
			break;
		case 2:
			for(i=0;i<nr;i++)
			{
				t0 = b[i*2];
				t1 = b[i*2+1];
				for(j=A->L->bptr[i];j<A->L->bptr[i+1];j++)
				{
					jj  = A->L->bindex[j];
					t0 -= A->L->value[j*4+0] * x[jj*2+0];
					t1 -= A->L->value[j*4+1] * x[jj*2+0];
					t0 -= A->L->value[j*4+2] * x[jj*2+1];
					t1 -= A->L->value[j*4+3] * x[jj*2+1];
				}
				x[i*2+0] = A->WD->value[4*i+0] * t0 + A->WD->value[4*i+2] * t1;
				x[i*2+1] = A->WD->value[4*i+1] * t0 + A->WD->value[4*i+3] * t1;
			}
			break;
		case 3:
			for(i=0;i<nr;i++)
			{
				t0 = b[i*3];
				t1 = b[i*3+1];
				t2 = b[i*3+2];
				for(j=A->L->bptr[i];j<A->L->bptr[i+1];j++)
				{
					jj  = A->L->bindex[j];
					t0 -= A->L->value[j*9+0] * x[jj*3+0];
					t1 -= A->L->value[j*9+1] * x[jj*3+0];
					t2 -= A->L->value[j*9+2] * x[jj*3+0];
					t0 -= A->L->value[j*9+3] * x[jj*3+1];
					t1 -= A->L->value[j*9+4] * x[jj*3+1];
					t2 -= A->L->value[j*9+5] * x[jj*3+1];
					t0 -= A->L->value[j*9+6] * x[jj*3+2];
					t1 -= A->L->value[j*9+7] * x[jj*3+2];
					t2 -= A->L->value[j*9+8] * x[jj*3+2];
				}
				x[i*3+0] = A->WD->value[9*i+0] * t0 + A->WD->value[9*i+3] * t1 + A->WD->value[9*i+6] * t2;
				x[i*3+1] = A->WD->value[9*i+1] * t0 + A->WD->value[9*i+4] * t1 + A->WD->value[9*i+7] * t2;
				x[i*3+2] = A->WD->value[9*i+2] * t0 + A->WD->value[9*i+5] * t1 + A->WD->value[9*i+8] * t2;
			}
			break;
		default:
			w = (LIS_SCALAR *)lis_malloc(bnr*sizeof(LIS_SCALAR),"lis_matrix_solve_bsc::w");
			for(i=0;i<nr;i++)
			{
				for(j=0;j<bnr;j++)
				{
					w[j] = b[i*bnr+j];
				}
				for(j=A->L->bptr[i];j<A->L->bptr[i+1];j++)
				{
					k   = A->L->bindex[j] * bnc;
					for(ii=0;ii<bnr;ii++)
					{
						t0   = w[ii];
						for(jj=0;jj<bnc;jj++)
						{
							t0 -= A->L->value[j*bs + jj*bnr+ii] * x[k + jj];
						}
						w[ii] = t0;
					}
				}
				for(ii=0;ii<bnr;ii++)
				{
					t0 = 0.0;
					for(jj=0;jj<bnc;jj++)
					{
						t0 += A->WD->value[i*bs + jj*bnr+ii] * w[jj];
					}
					x[i*bnr+ii] = t0;
				}
			}
			lis_free(w);
			break;
		}
		break;
	case LIS_MATRIX_UPPER:
		switch(bnr)
		{
		case 1:
			for(i=nr-1;i>=0;i--)
			{
				t0 = b[i];
				for(j=A->U->bptr[i];j<A->U->bptr[i+1];j++)
				{
					jj  = A->U->bindex[j];
					t0 -= A->U->value[j] * x[jj];
				}
				x[i] = A->WD->value[i] * t0;
			}
			break;
		case 2:
			for(i=nr-1;i>=0;i--)
			{
				t0 = b[i*2];
				t1 = b[i*2+1];
				for(j=A->U->bptr[i];j<A->U->bptr[i+1];j++)
				{
					jj  = A->U->bindex[j];
					t0 -= A->U->value[j*4+0] * x[jj*2+0];
					t1 -= A->U->value[j*4+1] * x[jj*2+0];
					t0 -= A->U->value[j*4+2] * x[jj*2+1];
					t1 -= A->U->value[j*4+3] * x[jj*2+1];
				}
				x[i*2+0] = A->WD->value[4*i+0] * t0 + A->WD->value[4*i+2] * t1;
				x[i*2+1] = A->WD->value[4*i+1] * t0 + A->WD->value[4*i+3] * t1;
			}
			break;
		case 3:
			for(i=nr-1;i>=0;i--)
			{
				t0 = b[i*3];
				t1 = b[i*3+1];
				t2 = b[i*3+2];
				for(j=A->U->bptr[i];j<A->U->bptr[i+1];j++)
				{
					jj  = A->U->bindex[j];
					t0 -= A->U->value[j*9+0] * x[jj*3+0];
					t1 -= A->U->value[j*9+1] * x[jj*3+0];
					t2 -= A->U->value[j*9+2] * x[jj*3+0];
					t0 -= A->U->value[j*9+3] * x[jj*3+1];
					t1 -= A->U->value[j*9+4] * x[jj*3+1];
					t2 -= A->U->value[j*9+5] * x[jj*3+1];
					t0 -= A->U->value[j*9+6] * x[jj*3+2];
					t1 -= A->U->value[j*9+7] * x[jj*3+2];
					t2 -= A->U->value[j*9+8] * x[jj*3+2];
				}
				x[i*3+0] = A->WD->value[9*i+0] * t0 + A->WD->value[9*i+3] * t1 + A->WD->value[9*i+6] * t2;
				x[i*3+1] = A->WD->value[9*i+1] * t0 + A->WD->value[9*i+4] * t1 + A->WD->value[9*i+7] * t2;
				x[i*3+2] = A->WD->value[9*i+2] * t0 + A->WD->value[9*i+5] * t1 + A->WD->value[9*i+8] * t2;
			}
			break;
		default:
			w = (LIS_SCALAR *)lis_malloc(bnr*sizeof(LIS_SCALAR),"lis_matrix_solve_bsc::w");
			for(i=nr-1;i>=0;i--)
			{
				for(j=0;j<bnr;j++)
				{
					w[j] = b[i*bnr+j];
				}
				for(j=A->U->bptr[i];j<A->U->bptr[i+1];j++)
				{
					k   = A->U->bindex[j] * bnc;
					for(ii=0;ii<bnr;ii++)
					{
						t0   = w[ii];
						for(jj=0;jj<bnc;jj++)
						{
							t0 -= A->U->value[j*bs + jj*bnr+ii] * x[k + jj];
						}
						w[ii] = t0;
					}
				}
				for(ii=0;ii<bnr;ii++)
				{
					t0 = 0.0;
					for(jj=0;jj<bnc;jj++)
					{
						t0 += A->WD->value[i*bs + jj*bnr+ii] * w[jj];
					}
					x[i*bnr+ii] = t0;
				}
			}
			lis_free(w);
			break;
		}
		break;
	case LIS_MATRIX_SSOR:
		switch(bnr)
		{
		case 1:
			for(i=0;i<nr;i++)
			{
				t0 = b[i];
				for(j=A->L->bptr[i];j<A->L->bptr[i+1];j++)
				{
					jj  = A->L->bindex[j];
					t0 -= A->L->value[j] * x[jj];
				}
				x[i] = A->WD->value[i] * t0;
			}
			for(i=nr-1;i>=0;i--)
			{
				t0 = 0.0;
				for(j=A->U->bptr[i];j<A->U->bptr[i+1];j++)
				{
					jj  = A->U->bindex[j];
					t0 += A->U->value[j] * x[jj];
				}
				x[i] -= A->WD->value[i] * t0;
			}
			break;
		case 2:
			for(i=0;i<nr;i++)
			{
				t0 = b[i*2];
				t1 = b[i*2+1];
				for(j=A->L->bptr[i];j<A->L->bptr[i+1];j++)
				{
					jj  = A->L->bindex[j];
					t0 -= A->L->value[j*4+0] * x[jj*2+0];
					t1 -= A->L->value[j*4+1] * x[jj*2+0];
					t0 -= A->L->value[j*4+2] * x[jj*2+1];
					t1 -= A->L->value[j*4+3] * x[jj*2+1];
				}
				x[i*2+0] = A->WD->value[4*i+0] * t0 + A->WD->value[4*i+2] * t1;
				x[i*2+1] = A->WD->value[4*i+1] * t0 + A->WD->value[4*i+3] * t1;
			}
			for(i=nr-1;i>=0;i--)
			{
				t0 = 0.0;
				t1 = 0.0;
				for(j=A->U->bptr[i];j<A->U->bptr[i+1];j++)
				{
					jj  = A->U->bindex[j];
					t0 += A->U->value[j*4+0] * x[jj*2+0];
					t1 += A->U->value[j*4+1] * x[jj*2+0];
					t0 += A->U->value[j*4+2] * x[jj*2+1];
					t1 += A->U->value[j*4+3] * x[jj*2+1];
				}
				x[i*2+0] -= A->WD->value[4*i+0] * t0 + A->WD->value[4*i+2] * t1;
				x[i*2+1] -= A->WD->value[4*i+1] * t0 + A->WD->value[4*i+3] * t1;
			}
			break;
		case 3:
			for(i=0;i<nr;i++)
			{
				t0 = b[i*bnr];
				t1 = b[i*bnr+1];
				t2 = b[i*bnr+2];
				for(j=A->L->bptr[i];j<A->L->bptr[i+1];j++)
				{
					jj  = A->L->bindex[j];
					t0 -= A->L->value[j*9+0] * x[jj*3+0];
					t1 -= A->L->value[j*9+1] * x[jj*3+0];
					t2 -= A->L->value[j*9+2] * x[jj*3+0];
					t0 -= A->L->value[j*9+3] * x[jj*3+1];
					t1 -= A->L->value[j*9+4] * x[jj*3+1];
					t2 -= A->L->value[j*9+5] * x[jj*3+1];
					t0 -= A->L->value[j*9+6] * x[jj*3+2];
					t1 -= A->L->value[j*9+7] * x[jj*3+2];
					t2 -= A->L->value[j*9+8] * x[jj*3+2];
				}
				x[i*bnr+0] = A->WD->value[9*i+0] * t0 + A->WD->value[9*i+3] * t1 + A->WD->value[9*i+6] * t2;
				x[i*bnr+1] = A->WD->value[9*i+1] * t0 + A->WD->value[9*i+4] * t1 + A->WD->value[9*i+7] * t2;
				x[i*bnr+2] = A->WD->value[9*i+2] * t0 + A->WD->value[9*i+5] * t1 + A->WD->value[9*i+8] * t2;
			}
			for(i=nr-1;i>=0;i--)
			{
				t0 = 0.0;
				t1 = 0.0;
				t2 = 0.0;
				for(j=A->U->bptr[i];j<A->U->bptr[i+1];j++)
				{
					jj  = A->U->bindex[j];
					t0 += A->U->value[j*9+0] * x[jj*3+0];
					t1 += A->U->value[j*9+1] * x[jj*3+0];
					t2 += A->U->value[j*9+2] * x[jj*3+0];
					t0 += A->U->value[j*9+3] * x[jj*3+1];
					t1 += A->U->value[j*9+4] * x[jj*3+1];
					t2 += A->U->value[j*9+5] * x[jj*3+1];
					t0 += A->U->value[j*9+6] * x[jj*3+2];
					t1 += A->U->value[j*9+7] * x[jj*3+2];
					t2 += A->U->value[j*9+8] * x[jj*3+2];
				}
				x[i*3+0] -= A->WD->value[9*i+0] * t0 + A->WD->value[9*i+3] * t1 + A->WD->value[9*i+6] * t2;
				x[i*3+1] -= A->WD->value[9*i+1] * t0 + A->WD->value[9*i+4] * t1 + A->WD->value[9*i+7] * t2;
				x[i*3+2] -= A->WD->value[9*i+2] * t0 + A->WD->value[9*i+5] * t1 + A->WD->value[9*i+8] * t2;
			}
			break;
		default:
			w = (LIS_SCALAR *)lis_malloc(bnr*sizeof(LIS_SCALAR),"lis_matrix_solve_bsc::w");
			for(i=0;i<nr;i++)
			{
				for(j=0;j<bnr;j++)
				{
					w[j] = b[i*bnr+j];
				}
				for(j=A->L->bptr[i];j<A->L->bptr[i+1];j++)
				{
					k   = A->L->bindex[j] * bnc;
					for(ii=0;ii<bnr;ii++)
					{
						t0   = w[ii];
						for(jj=0;jj<bnc;jj++)
						{
							t0 -= A->L->value[j*bs + jj*bnr+ii] * x[k + jj];
						}
						w[ii] = t0;
					}
				}
				for(ii=0;ii<bnr;ii++)
				{
					t0 = 0.0;
					for(jj=0;jj<bnc;jj++)
					{
						t0 += A->WD->value[i*bs + jj*bnr+ii] * w[jj];
					}
					x[i*bnr+ii] = t0;
				}
			}
			for(i=nr-1;i>=0;i--)
			{
				for(j=0;j<bnr;j++)
				{
					w[j] = 0.0;
				}
				for(j=A->U->bptr[i];j<A->U->bptr[i+1];j++)
				{
					k   = A->U->bindex[j] * bnc;
					for(ii=0;ii<bnr;ii++)
					{
						t0   = w[ii];
						for(jj=0;jj<bnc;jj++)
						{
							t0 += A->U->value[j*bs + jj*bnr+ii] * x[k + jj];
						}
						w[ii] = t0;
					}
				}
				for(ii=0;ii<bnr;ii++)
				{
					t0 = 0.0;
					for(jj=0;jj<bnc;jj++)
					{
						t0 += A->WD->value[i*bs + jj*bnr+ii] * w[jj];
					}
					x[i*bnr+ii] -= t0;
				}
			}
			lis_free(w);
			break;
		}
		break;
	}

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_solvet_bsc"
int lis_matrix_solvet_bsc(LIS_MATRIX A, LIS_VECTOR B, LIS_VECTOR X, int flag)
{
	int i,j,k,ii,jj,nr,bnr,bnc,bs;
	LIS_SCALAR	t0,t1,t2;
	LIS_SCALAR	*b,*x,*w;

	LIS_DEBUG_FUNC_IN;

	nr  = A->nr;
	bnr = A->bnr;
	bnc = A->bnc;
	bs  = A->bnr*A->bnc;
	b   = B->value;
	x   = X->value;

	lis_vector_copy(B,X);
	switch(flag)
	{
	case LIS_MATRIX_LOWER:
		switch(bnr)
		{
		case 1:
			for(i=0;i<nr;i++)
			{
				x[i] = x[i] * A->WD->value[i];
				for(j=A->U->bptr[i];j<A->U->bptr[i+1];j++)
				{
					jj     = A->U->bindex[j];
					x[jj] -= A->U->value[j] * x[i];
				}
			}
			break;
		case 2:
			for(i=0;i<nr;i++)
			{
				t0 = A->WD->value[4*i+0] * x[i*2] + A->WD->value[4*i+1] * x[i*2+1];
				t1 = A->WD->value[4*i+2] * x[i*2] + A->WD->value[4*i+3] * x[i*2+1];
				x[i*2+0] = t0;
				x[i*2+1] = t1;
				for(j=A->U->bptr[i];j<A->U->bptr[i+1];j++)
				{
					jj  = A->U->bindex[j];
					x[jj*2+0] -= A->U->value[j*4+0] * t0 + A->U->value[j*4+1] * t1;
					x[jj*2+1] -= A->U->value[j*4+2] * t0 + A->U->value[j*4+3] * t1;
				}
			}
			break;
		case 3:
			for(i=0;i<nr;i++)
			{
				t0 = A->WD->value[9*i+0] * x[i*3] + A->WD->value[9*i+1] * x[i*3+1] + A->WD->value[9*i+2] * x[i*3+2];
				t1 = A->WD->value[9*i+3] * x[i*3] + A->WD->value[9*i+4] * x[i*3+1] + A->WD->value[9*i+5] * x[i*3+2];
				t2 = A->WD->value[9*i+6] * x[i*3] + A->WD->value[9*i+7] * x[i*3+1] + A->WD->value[9*i+8] * x[i*3+2];
				x[i*3]   = t0;
				x[i*3+1] = t1;
				x[i*3+2] = t2;
				for(j=A->U->bptr[i];j<A->U->bptr[i+1];j++)
				{
					jj  = A->U->bindex[j];
					x[jj*3+0] -= A->U->value[j*9+0] * t0 + A->U->value[j*9+1] * t1 + A->U->value[j*9+2] * t2;
					x[jj*3+1] -= A->U->value[j*9+3] * t0 + A->U->value[j*9+4] * t1 + A->U->value[j*9+5] * t2;
					x[jj*3+2] -= A->U->value[j*9+6] * t0 + A->U->value[j*9+7] * t1 + A->U->value[j*9+8] * t2;
				}
			}
			break;
		default:
			w = (LIS_SCALAR *)lis_malloc(bnc*sizeof(LIS_SCALAR),"lis_matrix_solvet_bsc::w");
			for(i=0;i<nr;i++)
			{
				for(jj=0;jj<bnc;jj++)
				{
					t0 = 0.0;
					for(ii=0;ii<bnr;ii++)
					{
						t0 += A->WD->value[i*bs + jj*bnr+ii] * x[i*bnr + ii];
					}
					w[jj] = t0;
				}
				memcpy(&x[i*bnr],w,bnr*sizeof(LIS_SCALAR));
				for(j=A->U->bptr[i];j<A->U->bptr[i+1];j++)
				{
					k   = A->U->bindex[j] * bnc;
					for(jj=0;jj<bnc;jj++)
					{
						t0 = 0.0;
						for(ii=0;ii<bnr;ii++)
						{
							t0 += A->U->value[j*bs + jj*bnr+ii] * w[ii];
						}
						x[k + jj] -= t0;
					}
				}
			}
			lis_free(w);
			break;
		}
		break;
	case LIS_MATRIX_UPPER:
		switch(bnr)
		{
		case 1:
			for(i=nr-1;i>=0;i--)
			{
				x[i] = x[i] * A->WD->value[i];
				for(j=A->L->bptr[i];j<A->L->bptr[i+1];j++)
				{
					jj     = A->L->bindex[j];
					x[jj] -= A->L->value[j] * x[i];
				}
			}
			break;
		case 2:
			for(i=nr-1;i>=0;i--)
			{
				t0 = A->WD->value[4*i+0] * x[i*2] + A->WD->value[4*i+1] * x[i*2+1];
				t1 = A->WD->value[4*i+2] * x[i*2] + A->WD->value[4*i+3] * x[i*2+1];
				x[i*2+0] = t0;
				x[i*2+1] = t1;
				for(j=A->L->bptr[i];j<A->L->bptr[i+1];j++)
				{
					jj  = A->L->bindex[j];
					x[jj*2+0] -= A->L->value[j*4+0] * t0 + A->L->value[j*4+1] * t1;
					x[jj*2+1] -= A->L->value[j*4+2] * t0 + A->L->value[j*4+3] * t1;
				}
			}
			break;
		case 3:
			for(i=nr-1;i>=0;i--)
			{
				t0 = A->WD->value[9*i+0] * x[i*3] + A->WD->value[9*i+1] * x[i*3+1] + A->WD->value[9*i+2] * x[i*3+2];
				t1 = A->WD->value[9*i+3] * x[i*3] + A->WD->value[9*i+4] * x[i*3+1] + A->WD->value[9*i+5] * x[i*3+2];
				t2 = A->WD->value[9*i+6] * x[i*3] + A->WD->value[9*i+7] * x[i*3+1] + A->WD->value[9*i+8] * x[i*3+2];
				x[i*3]   = t0;
				x[i*3+1] = t1;
				x[i*3+2] = t2;
				for(j=A->L->bptr[i];j<A->L->bptr[i+1];j++)
				{
					jj  = A->L->bindex[j];
					x[jj*3+0] -= A->L->value[j*9+0] * t0 + A->L->value[j*9+1] * t1 + A->L->value[j*9+2] * t2;
					x[jj*3+1] -= A->L->value[j*9+3] * t0 + A->L->value[j*9+4] * t1 + A->L->value[j*9+5] * t2;
					x[jj*3+2] -= A->L->value[j*9+6] * t0 + A->L->value[j*9+7] * t1 + A->L->value[j*9+8] * t2;
				}
			}
			break;
		default:
			w = (LIS_SCALAR *)lis_malloc(bnr*sizeof(LIS_SCALAR),"lis_matrix_solvet_bsc::w");
			for(i=nr-1;i>=0;i--)
			{
				for(jj=0;jj<bnc;jj++)
				{
					t0 = 0.0;
					for(ii=0;ii<bnr;ii++)
					{
						t0 += A->WD->value[i*bs + jj*bnr+ii] * x[i*bnr + ii];
					}
					w[jj] = t0;
				}
				memcpy(&x[i*bnr],w,bnr*sizeof(LIS_SCALAR));
				for(j=A->L->bptr[i];j<A->L->bptr[i+1];j++)
				{
					k   = A->L->bindex[j] * bnc;
					for(jj=0;jj<bnc;jj++)
					{
						t0 = 0.0;
						for(ii=0;ii<bnr;ii++)
						{
							t0 += A->L->value[j*bs + jj*bnr+ii] * w[ii];
						}
						x[k + jj] -= t0;
					}
				}
			}
			lis_free(w);
			break;
		}
		break;
	case LIS_MATRIX_SSOR:
		switch(bnr)
		{
		case 1:
			for(i=0;i<nr;i++)
			{
				t0 = x[i] * A->WD->value[i];
				for(j=A->U->bptr[i];j<A->U->bptr[i+1];j++)
				{
					jj     = A->U->bindex[j];
					x[jj] -= A->U->value[j] * t0;
				}
			}
			for(i=nr-1;i>=0;i--)
			{
				t0   = x[i] * A->WD->value[i];
				x[i] = t0;
				for(j=A->L->bptr[i];j<A->L->bptr[i+1];j++)
				{
					jj     = A->L->bindex[j];
					x[jj] -= A->L->value[j] * t0;
				}
			}
			break;
		case 2:
			for(i=0;i<nr;i++)
			{
				t0 = A->WD->value[4*i+0] * x[i*2] + A->WD->value[4*i+1] * x[i*2+1];
				t1 = A->WD->value[4*i+2] * x[i*2] + A->WD->value[4*i+3] * x[i*2+1];
				for(j=A->U->bptr[i];j<A->U->bptr[i+1];j++)
				{
					jj  = A->U->bindex[j];
					x[jj*2+0] -= A->U->value[j*4+0] * t0 + A->U->value[j*4+1] * t1;
					x[jj*2+1] -= A->U->value[j*4+2] * t0 + A->U->value[j*4+3] * t1;
				}
			}
			for(i=nr-1;i>=0;i--)
			{
				t0 = A->WD->value[4*i+0] * x[i*2] + A->WD->value[4*i+1] * x[i*2+1];
				t1 = A->WD->value[4*i+2] * x[i*2] + A->WD->value[4*i+3] * x[i*2+1];
				x[i*2+0] = t0;
				x[i*2+1] = t1;
				for(j=A->L->bptr[i];j<A->L->bptr[i+1];j++)
				{
					jj  = A->L->bindex[j];
					x[jj*2+0] -= A->L->value[j*4+0] * t0 + A->L->value[j*4+1] * t1;
					x[jj*2+1] -= A->L->value[j*4+2] * t0 + A->L->value[j*4+3] * t1;
				}
			}
			break;
		case 3:
			for(i=0;i<nr;i++)
			{
				t0 = A->WD->value[9*i+0] * x[i*3] + A->WD->value[9*i+1] * x[i*3+1] + A->WD->value[9*i+2] * x[i*3+2];
				t1 = A->WD->value[9*i+3] * x[i*3] + A->WD->value[9*i+4] * x[i*3+1] + A->WD->value[9*i+5] * x[i*3+2];
				t2 = A->WD->value[9*i+6] * x[i*3] + A->WD->value[9*i+7] * x[i*3+1] + A->WD->value[9*i+8] * x[i*3+2];
				for(j=A->U->bptr[i];j<A->U->bptr[i+1];j++)
				{
					jj  = A->U->bindex[j];
					x[jj*3+0] -= A->U->value[j*9+0] * t0 + A->U->value[j*9+1] * t1 + A->U->value[j*9+2] * t2;
					x[jj*3+1] -= A->U->value[j*9+3] * t0 + A->U->value[j*9+4] * t1 + A->U->value[j*9+5] * t2;
					x[jj*3+2] -= A->U->value[j*9+6] * t0 + A->U->value[j*9+7] * t1 + A->U->value[j*9+8] * t2;
				}
			}
			for(i=nr-1;i>=0;i--)
			{
				t0 = A->WD->value[9*i+0] * x[i*3] + A->WD->value[9*i+1] * x[i*3+1] + A->WD->value[9*i+2] * x[i*3+2];
				t1 = A->WD->value[9*i+3] * x[i*3] + A->WD->value[9*i+4] * x[i*3+1] + A->WD->value[9*i+5] * x[i*3+2];
				t2 = A->WD->value[9*i+6] * x[i*3] + A->WD->value[9*i+7] * x[i*3+1] + A->WD->value[9*i+8] * x[i*3+2];
				x[i*3]   = t0;
				x[i*3+1] = t1;
				x[i*3+2] = t2;
				for(j=A->L->bptr[i];j<A->L->bptr[i+1];j++)
				{
					jj  = A->L->bindex[j];
					x[jj*3+0] -= A->L->value[j*9+0] * t0 + A->L->value[j*9+1] * t1 + A->L->value[j*9+2] * t2;
					x[jj*3+1] -= A->L->value[j*9+3] * t0 + A->L->value[j*9+4] * t1 + A->L->value[j*9+5] * t2;
					x[jj*3+2] -= A->L->value[j*9+6] * t0 + A->L->value[j*9+7] * t1 + A->L->value[j*9+8] * t2;
				}
			}
			break;
		default:
			w = (LIS_SCALAR *)lis_malloc(bnc*sizeof(LIS_SCALAR),"lis_matrix_solvet_bsc::w");
			for(i=0;i<nr;i++)
			{
				for(jj=0;jj<bnc;jj++)
				{
					t0 = 0.0;
					for(ii=0;ii<bnr;ii++)
					{
						t0 += A->WD->value[i*bs + jj*bnr+ii] * x[i*bnr + ii];
					}
					w[jj] = t0;
				}
				for(j=A->U->bptr[i];j<A->U->bptr[i+1];j++)
				{
					k   = A->U->bindex[j] * bnc;
					for(jj=0;jj<bnc;jj++)
					{
						t0 = 0.0;
						for(ii=0;ii<bnr;ii++)
						{
							t0 += A->U->value[j*bs + jj*bnr+ii] * w[ii];
						}
						x[k + jj] -= t0;
					}
				}
			}
			for(i=nr-1;i>=0;i--)
			{
				for(jj=0;jj<bnc;jj++)
				{
					t0 = 0.0;
					for(ii=0;ii<bnr;ii++)
					{
						t0 += A->WD->value[i*bs + jj*bnr+ii] * x[i*bnr + ii];
					}
					w[jj] = t0;
				}
				memcpy(&x[i*bnr],w,bnr*sizeof(LIS_SCALAR));
				for(j=A->L->bptr[i];j<A->L->bptr[i+1];j++)
				{
					k   = A->L->bindex[j] * bnc;
					for(jj=0;jj<bnc;jj++)
					{
						t0 = 0.0;
						for(ii=0;ii<bnr;ii++)
						{
							t0 += A->L->value[j*bs + jj*bnr+ii] * w[ii];
						}
						x[k + jj] -= t0;
					}
				}
			}
			lis_free(w);
			break;
		}
		break;
	}

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}