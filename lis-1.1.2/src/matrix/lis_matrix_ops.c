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
#include <math.h>
#ifdef _OPENMP
	#include <omp.h>
#endif
#ifdef USE_MPI
	#include <mpi.h>
#endif
#include "lislib.h"

#undef __FUNC__
#define __FUNC__ "lis_matrix_set_blocksize"
int lis_matrix_set_blocksize(LIS_MATRIX A, int bnr, int bnc, int row[], int col[])
{
	int i,n;
	int err;
	int *conv_row,*conv_col;

	LIS_DEBUG_FUNC_IN;

	err = lis_matrix_check(A,LIS_MATRIX_CHECK_NULL);
	if( err ) return err;

	if( bnr<=0 || bnc<=0 )
	{
		LIS_SETERR2(LIS_ERR_ILL_ARG,"bnr=%d <= 0 or bnc=%d <= 0\n",bnr,bnc);
		return LIS_ERR_ILL_ARG;
	}
	if( (row==NULL && col!=NULL) || (row!=NULL && col==NULL) )
	{
		LIS_SETERR2(LIS_ERR_ILL_ARG,"either row[]=%x or col[]=%x is NULL\n",row,col);
		return LIS_ERR_ILL_ARG;
	}
	if( row==NULL )
	{
		A->conv_bnr = bnr;
		A->conv_bnc = bnc;
	}
	else
	{
		n = A->n;
		conv_row = (int *)lis_malloc(n*sizeof(int),"lis_matrix_set_blocksize::conv_row");
		if( conv_row==NULL )
		{
			LIS_SETERR_MEM(n*sizeof(int));
			return LIS_OUT_OF_MEMORY;
		}
		conv_col = (int *)lis_malloc(n*sizeof(int),"lis_matrix_set_blocksize::conv_col");
		if( conv_col==NULL )
		{
			LIS_SETERR_MEM(n*sizeof(int));
			lis_free(conv_row);
			return LIS_OUT_OF_MEMORY;
		}
		for(i=0;i<n;i++)
		{
			conv_row[i] = row[i];
			conv_col[i] = col[i];
		}
		A->conv_row = conv_row;
		A->conv_col = conv_col;
	}

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_convert"
int lis_matrix_convert(LIS_MATRIX Ain, LIS_MATRIX Aout)
{
	int err;
	int istmp;
	int convert_matrix_type;
	LIS_MATRIX Atmp,Atmp2;

	LIS_DEBUG_FUNC_IN;

	err = lis_matrix_check(Ain,LIS_MATRIX_CHECK_ALL);
	if( err ) return err;
	err = lis_matrix_check(Aout,LIS_MATRIX_CHECK_NULL);
	if( err ) return err;

	err = lis_matrix_merge(Ain);
	if( err ) return err;

	convert_matrix_type = Aout->matrix_type;

	if( Ain->matrix_type==convert_matrix_type && !Ain->is_block )
	{
		err = lis_matrix_copy(Ain,Aout);
		return err;
	}
	if( Ain->matrix_type!=LIS_MATRIX_CRS )
	{
		istmp = LIS_TRUE;
		switch( Ain->matrix_type )
		{
		case LIS_MATRIX_RCO:
			switch( convert_matrix_type )
			{
			case LIS_MATRIX_CRS:
				err = lis_matrix_convert_rco2crs(Ain,Aout);
				LIS_DEBUG_FUNC_OUT;
				return err;
				break;
			case LIS_MATRIX_BSR:
				err = lis_matrix_convert_rco2bsr(Ain,Aout);
				LIS_DEBUG_FUNC_OUT;
				return err;
			case LIS_MATRIX_CCS:
				err = lis_matrix_convert_rco2ccs(Ain,Aout);
				LIS_DEBUG_FUNC_OUT;
				return err;
				break;
			default:
				err = lis_matrix_duplicate(Ain,&Atmp);
				if( err ) return err;
				err = lis_matrix_convert_rco2crs(Ain,Atmp);
				break;
			}
			break;
		case LIS_MATRIX_CCS:
			switch( convert_matrix_type )
			{
			case LIS_MATRIX_BSC:
				err = lis_matrix_convert_ccs2bsc(Ain,Aout);
				LIS_DEBUG_FUNC_OUT;
				return err;
			default:
				err = lis_matrix_duplicate(Ain,&Atmp);
				if( err ) return err;
				err = lis_matrix_convert_ccs2crs(Ain,Atmp);
				break;
			}
			break;
		case LIS_MATRIX_MSR:
			err = lis_matrix_duplicate(Ain,&Atmp);
			if( err ) return err;
			err = lis_matrix_convert_msr2crs(Ain,Atmp);
			break;
		case LIS_MATRIX_DIA:
			err = lis_matrix_duplicate(Ain,&Atmp);
			if( err ) return err;
			err = lis_matrix_convert_dia2crs(Ain,Atmp);
			break;
		case LIS_MATRIX_ELL:
			err = lis_matrix_duplicate(Ain,&Atmp);
			if( err ) return err;
			err = lis_matrix_convert_ell2crs(Ain,Atmp);
			break;
		case LIS_MATRIX_JDS:
			err = lis_matrix_duplicate(Ain,&Atmp);
			if( err ) return err;
			err = lis_matrix_convert_jds2crs(Ain,Atmp);
			break;
		case LIS_MATRIX_BSR:
			err = lis_matrix_duplicate(Ain,&Atmp);
			if( err ) return err;
			err = lis_matrix_convert_bsr2crs(Ain,Atmp);
			break;
		case LIS_MATRIX_BSC:
			err = lis_matrix_duplicate(Ain,&Atmp);
			if( err ) return err;
			err = lis_matrix_convert_bsc2crs(Ain,Atmp);
			break;
		case LIS_MATRIX_VBR:
			err = lis_matrix_duplicate(Ain,&Atmp);
			if( err ) return err;
			err = lis_matrix_convert_vbr2crs(Ain,Atmp);
			break;
		case LIS_MATRIX_DNS:
			err = lis_matrix_duplicate(Ain,&Atmp);
			if( err ) return err;
			err = lis_matrix_convert_dns2crs(Ain,Atmp);
			break;
		case LIS_MATRIX_COO:
			err = lis_matrix_duplicate(Ain,&Atmp);
			if( err ) return err;
			err = lis_matrix_convert_coo2crs(Ain,Atmp);
			break;
		default:
			LIS_SETERR_IMP;
			err = LIS_ERR_NOT_IMPLEMENTED;
		}
		if( err )
		{
			return err;
		}
		if( convert_matrix_type== LIS_MATRIX_CRS )
		{
			lis_matrix_storage_destroy(Aout);
			lis_matrix_DLU_destroy(Aout);
			lis_matrix_diag_destroy(Aout->WD);
			if( Aout->l2g_map ) lis_free( Aout->l2g_map );
			if( Aout->commtable ) lis_commtable_destroy( Aout->commtable );
			if( Aout->ranges ) lis_free( Aout->ranges );
			lis_matrix_copy_struct(Atmp,Aout);
			lis_free(Atmp);
			return LIS_SUCCESS;
		}
	}
	else
	{
		istmp = LIS_FALSE;
		Atmp = Ain;
	}

	switch( convert_matrix_type )
	{
	case LIS_MATRIX_BSR:
		err = lis_matrix_convert_crs2bsr(Atmp,Aout);
		break;
	case LIS_MATRIX_CCS:
		err = lis_matrix_convert_crs2ccs(Atmp,Aout); 
		break;
	case LIS_MATRIX_MSR:
		err = lis_matrix_convert_crs2msr(Atmp,Aout);
		break;
	case LIS_MATRIX_ELL:
		err = lis_matrix_convert_crs2ell(Atmp,Aout);
		break;
	case LIS_MATRIX_DIA:
		err = lis_matrix_convert_crs2dia(Atmp,Aout);
		break;
	case LIS_MATRIX_JDS:
		err = lis_matrix_convert_crs2jds(Atmp,Aout);
		break;
	case LIS_MATRIX_BSC:
		err = lis_matrix_duplicate(Atmp,&Atmp2);
		if( err ) return err;
		err = lis_matrix_convert_crs2ccs(Atmp,Atmp2);
		if( err ) return err;
		if( Atmp!=Ain )
		{
			lis_matrix_destroy(Atmp);
		}
		Atmp = Atmp2;
		istmp = LIS_TRUE;
		err = lis_matrix_convert_ccs2bsc(Atmp,Aout);
		break;
	case LIS_MATRIX_VBR:
		err = lis_matrix_convert_crs2vbr(Atmp,Aout);
		break;
	case LIS_MATRIX_DNS:
		err = lis_matrix_convert_crs2dns(Atmp,Aout);
		break;
	case LIS_MATRIX_COO:
		err = lis_matrix_convert_crs2coo(Atmp,Aout);
		break;
	default:
		LIS_SETERR_IMP;
		err = LIS_ERR_NOT_IMPLEMENTED;
	}

	if( istmp ) lis_matrix_destroy(Atmp);
	if( err )
	{
		return err;
	}

	LIS_DEBUG_FUNC_OUT;
	return err;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_convert_self"
int lis_matrix_convert_self(LIS_SOLVER solver)
{
	int			err;
	int			storage,block;
	LIS_MATRIX	A,B;

	LIS_DEBUG_FUNC_IN;

	A = solver->A;
	storage     = solver->options[LIS_OPTIONS_STORAGE];
	block       = solver->options[LIS_OPTIONS_STORAGE_BLOCK];

	if( storage>0 && A->matrix_type!=storage )
	{
		err = lis_matrix_duplicate(A,&B);
		if( err ) return err;
		lis_matrix_set_blocksize(B,block,block,NULL,NULL);
		lis_matrix_set_type(B,storage);
		err = lis_matrix_convert(A,B);
		if( err ) return err;
		lis_matrix_storage_destroy(A);
		lis_matrix_DLU_destroy(A);
		lis_matrix_diag_destroy(A->WD);
		if( A->l2g_map ) lis_free( A->l2g_map );
		if( A->commtable ) lis_commtable_destroy( A->commtable );
		if( A->ranges ) lis_free( A->ranges );
		err = lis_matrix_copy_struct(B,A);
		if( err ) return err;
		lis_free(B);
		if( A->matrix_type==LIS_MATRIX_JDS )
		{
			A->work = (LIS_SCALAR *)lis_malloc(A->n*sizeof(LIS_SCALAR),"lis_precon_create_bjacobi::A->work");
			if( A->work==NULL )
			{
				LIS_SETERR_MEM(A->n*sizeof(LIS_SCALAR));
				return LIS_OUT_OF_MEMORY;
			}
		}
	}

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_get_diagonal"
int lis_matrix_get_diagonal(LIS_MATRIX A, LIS_VECTOR D)
{
	LIS_SCALAR *d;

	LIS_DEBUG_FUNC_IN;

	d = D->value;
	switch( A->matrix_type )
	{
	case LIS_MATRIX_CRS:
		lis_matrix_get_diagonal_crs(A, d);
		break;
	case LIS_MATRIX_CCS:
		lis_matrix_get_diagonal_ccs(A, d);
		break;
	case LIS_MATRIX_MSR:
		lis_matrix_get_diagonal_msr(A, d);
		break;
	case LIS_MATRIX_DIA:
		lis_matrix_get_diagonal_dia(A, d);
		break;
	case LIS_MATRIX_ELL:
		lis_matrix_get_diagonal_ell(A, d);
		break;
	case LIS_MATRIX_JDS:
		lis_matrix_get_diagonal_jds(A, d);
		break;
	case LIS_MATRIX_BSR:
		lis_matrix_get_diagonal_bsr(A, d);
		break;
	case LIS_MATRIX_BSC:
		lis_matrix_get_diagonal_bsc(A, d);
		break;
	case LIS_MATRIX_DNS:
		lis_matrix_get_diagonal_dns(A, d);
		break;
	case LIS_MATRIX_COO:
		lis_matrix_get_diagonal_coo(A, d);
		break;
	case LIS_MATRIX_VBR:
		lis_matrix_get_diagonal_vbr(A, d);
		break;
	default:
		LIS_SETERR_IMP;
		return LIS_ERR_NOT_IMPLEMENTED;
		break;
	}
	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_scaling"
int lis_matrix_scaling(LIS_MATRIX A, LIS_VECTOR B, LIS_VECTOR D, int action)
{
	int			i,n,np;
	LIS_SCALAR  *b,*d;

	n  = A->n;
	np = A->np;
	b  = B->value;
	d  = D->value;

	lis_matrix_get_diagonal(A,D);
	if( action==LIS_SCALE_SYMM_DIAG )
	{
#ifdef USE_MPI
		if( A->np>D->np )
		{
			D->value = (LIS_SCALAR *)lis_realloc(D->value,A->np*sizeof(LIS_SCALAR));
			if( D->value==NULL )
			{
				LIS_SETERR_MEM(A->np*sizeof(LIS_SCALAR));
				return LIS_OUT_OF_MEMORY;
			}
			d = D->value;
		}
		lis_send_recv(A->commtable,d);
#endif
		#ifdef _OPENMP
		#pragma omp parallel for private(i)
		#endif
		for(i=0; i<np; i++)
		{
			d[i] = 1.0 / sqrt(fabs(d[i]));
		}

		switch( A->matrix_type )
		{
		case LIS_MATRIX_CRS:
			lis_matrix_scaling_symm_crs(A, d);
			break;
		case LIS_MATRIX_CCS:
			lis_matrix_scaling_symm_ccs(A, d);
			break;
		case LIS_MATRIX_MSR:
			lis_matrix_scaling_symm_msr(A, d);
			break;
		case LIS_MATRIX_DIA:
			lis_matrix_scaling_symm_dia(A, d);
			break;
		case LIS_MATRIX_ELL:
			lis_matrix_scaling_symm_ell(A, d);
			break;
		case LIS_MATRIX_JDS:
			lis_matrix_scaling_symm_jds(A, d);
			break;
		case LIS_MATRIX_BSR:
			lis_matrix_scaling_symm_bsr(A, d);
			break;
		case LIS_MATRIX_BSC:
			lis_matrix_scaling_symm_bsc(A, d);
			break;
		case LIS_MATRIX_DNS:
			lis_matrix_scaling_symm_dns(A, d);
			break;
		case LIS_MATRIX_COO:
			lis_matrix_scaling_symm_coo(A, d);
			break;
		case LIS_MATRIX_VBR:
			lis_matrix_scaling_symm_vbr(A, d);
			break;
		default:
			LIS_SETERR_IMP;
			return LIS_ERR_NOT_IMPLEMENTED;
			break;
		}
	}
	else
	{
		#ifdef _OPENMP
		#pragma omp parallel for private(i)
		#endif
		for(i=0; i<n; i++)
		{
			d[i] = 1.0 / d[i];
		}
		switch( A->matrix_type )
		{
		case LIS_MATRIX_CRS:
			lis_matrix_scaling_crs(A, d);
			break;
		case LIS_MATRIX_CCS:
			lis_matrix_scaling_ccs(A, d);
			break;
		case LIS_MATRIX_MSR:
			lis_matrix_scaling_msr(A, d);
			break;
		case LIS_MATRIX_DIA:
			lis_matrix_scaling_dia(A, d);
			break;
		case LIS_MATRIX_ELL:
			lis_matrix_scaling_ell(A, d);
			break;
		case LIS_MATRIX_JDS:
			lis_matrix_scaling_jds(A, d);
			break;
		case LIS_MATRIX_BSR:
			lis_matrix_scaling_bsr(A, d);
			break;
		case LIS_MATRIX_BSC:
			lis_matrix_scaling_bsc(A, d);
			break;
		case LIS_MATRIX_DNS:
			lis_matrix_scaling_dns(A, d);
			break;
		case LIS_MATRIX_COO:
			lis_matrix_scaling_coo(A, d);
			break;
		case LIS_MATRIX_VBR:
			lis_matrix_scaling_vbr(A, d);
			break;
		default:
			LIS_SETERR_IMP;
			return LIS_ERR_NOT_IMPLEMENTED;
			break;
		}
	}

	#ifdef _OPENMP
	#pragma omp parallel for private(i)
	#endif
	for(i=0; i<n; i++)
	{
		b[i] = b[i]*d[i];
	}
	A->is_scaled = LIS_TRUE;
	B->is_scaled = LIS_TRUE;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_split"
int lis_matrix_split(LIS_MATRIX A)
{
	int err;

	LIS_DEBUG_FUNC_IN;

	if( A->is_splited )
	{
		LIS_DEBUG_FUNC_OUT;
		return LIS_SUCCESS;
	}
	switch( A->matrix_type )
	{
	case LIS_MATRIX_CRS:
		err = lis_matrix_split_crs(A);
		break;
	case LIS_MATRIX_CCS:
		err = lis_matrix_split_ccs(A);
		break;
	case LIS_MATRIX_BSR:
		err = lis_matrix_split_bsr(A);
		break;
	case LIS_MATRIX_MSR:
		err = lis_matrix_split_msr(A);
		break;
	case LIS_MATRIX_ELL:
		err = lis_matrix_split_ell(A);
		break;
	case LIS_MATRIX_DIA:
		err = lis_matrix_split_dia(A);
		break;
	case LIS_MATRIX_JDS:
		err = lis_matrix_split_jds(A);
		break;
	case LIS_MATRIX_BSC:
		err = lis_matrix_split_bsc(A);
		break;
	case LIS_MATRIX_DNS:
		err = lis_matrix_split_dns(A);
		break;
	case LIS_MATRIX_COO:
		err = lis_matrix_split_coo(A);
		break;
	case LIS_MATRIX_VBR:
		err = lis_matrix_split_vbr(A);
		break;
	default:
		LIS_SETERR_IMP;
		return LIS_ERR_NOT_IMPLEMENTED;
		break;
	}

	if( err ) return err;
	if( !A->is_save )
	{
		lis_matrix_storage_destroy(A);
	}
	A->is_splited = LIS_TRUE;

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_merge"
int lis_matrix_merge(LIS_MATRIX A)
{
	int err;

	LIS_DEBUG_FUNC_IN;

	if( !A->is_splited || (A->is_save && A->is_splited) )
	{
		LIS_DEBUG_FUNC_OUT;
		return LIS_SUCCESS;
	}
	switch( A->matrix_type )
	{
	case LIS_MATRIX_CRS:
		err = lis_matrix_merge_crs(A);
		break;
	case LIS_MATRIX_CCS:
		err = lis_matrix_merge_ccs(A);
		break;
	case LIS_MATRIX_MSR:
		err = lis_matrix_merge_msr(A);
		break;
	case LIS_MATRIX_BSR:
		err = lis_matrix_merge_bsr(A);
		break;
	case LIS_MATRIX_ELL:
		err = lis_matrix_merge_ell(A);
		break;
	case LIS_MATRIX_JDS:
		err = lis_matrix_merge_jds(A);
		break;
	case LIS_MATRIX_DIA:
		err = lis_matrix_merge_dia(A);
		break;
	case LIS_MATRIX_BSC:
		err = lis_matrix_merge_bsc(A);
		break;
	case LIS_MATRIX_DNS:
		err = lis_matrix_merge_dns(A);
		break;
	case LIS_MATRIX_COO:
		err = lis_matrix_merge_coo(A);
		break;
	case LIS_MATRIX_VBR:
		err = lis_matrix_merge_vbr(A);
		break;
	default:
		LIS_SETERR_IMP;
		return LIS_ERR_NOT_IMPLEMENTED;
		break;
	}

	if( err ) return err;
	if( !A->is_save )
	{
		lis_matrix_DLU_destroy(A);
		A->is_splited = LIS_FALSE;
	}


	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_copy"
int lis_matrix_copy(LIS_MATRIX Ain, LIS_MATRIX Aout)
{
	int err;

	LIS_DEBUG_FUNC_IN;

	err = lis_matrix_check(Ain,LIS_MATRIX_CHECK_ALL);
	if( err ) return err;
	err = lis_matrix_check(Ain,LIS_MATRIX_CHECK_NULL);
	if( err ) return err;

	switch( Ain->matrix_type )
	{
	case LIS_MATRIX_CRS:
		err = lis_matrix_copy_crs(Ain,Aout);
		break;
	case LIS_MATRIX_CCS:
		err = lis_matrix_copy_ccs(Ain,Aout);
		break;
	case LIS_MATRIX_MSR:
		err = lis_matrix_copy_msr(Ain,Aout);
		break;
	case LIS_MATRIX_DIA:
		err = lis_matrix_copy_dia(Ain,Aout);
		break;
	case LIS_MATRIX_ELL:
		err = lis_matrix_copy_ell(Ain,Aout);
		break;
	case LIS_MATRIX_JDS:
		err = lis_matrix_copy_jds(Ain,Aout);
		break;
	case LIS_MATRIX_BSR:
		err = lis_matrix_copy_bsr(Ain,Aout);
		break;
	case LIS_MATRIX_VBR:
		err = lis_matrix_copy_vbr(Ain,Aout);
		break;
	case LIS_MATRIX_DNS:
		err = lis_matrix_copy_dns(Ain,Aout);
		break;
	case LIS_MATRIX_COO:
		err = lis_matrix_copy_coo(Ain,Aout);
		break;
	case LIS_MATRIX_BSC:
		err = lis_matrix_copy_bsc(Ain,Aout);
		break;
		default:
			LIS_SETERR_IMP;
			return LIS_ERR_NOT_IMPLEMENTED;
			break;
	}
	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_copyDLU"
int lis_matrix_copyDLU(LIS_MATRIX Ain, LIS_MATRIX_DIAG *D, LIS_MATRIX *L, LIS_MATRIX *U)
{
	int err;

	LIS_DEBUG_FUNC_IN;

	err = lis_matrix_check(Ain,LIS_MATRIX_CHECK_ALL);
	if( err ) return err;

	switch( Ain->matrix_type )
	{
	case LIS_MATRIX_CRS:
		err = lis_matrix_copyDLU_crs(Ain,D,L,U);
		break;
/*
	case LIS_MATRIX_CCS:
		err = lis_matrix_copy_ccs(Ain,Aout);
		break;
	case LIS_MATRIX_MSR:
		err = lis_matrix_copy_msr(Ain,Aout);
		break;
	case LIS_MATRIX_DIA:
		err = lis_matrix_copy_dia(Ain,Aout);
		break;
	case LIS_MATRIX_ELL:
		err = lis_matrix_copy_ell(Ain,Aout);
		break;
	case LIS_MATRIX_JDS:
		err = lis_matrix_copy_jds(Ain,Aout);
		break;
	case LIS_MATRIX_BSR:
		err = lis_matrix_copy_bsr(Ain,Aout);
		break;
	case LIS_MATRIX_BSC:
		err = lis_matrix_copy_bsc(Ain,Aout);
		break;
	case LIS_MATRIX_VBR:
		err = lis_matrix_copy_vbr(Ain,Aout);
		break;
	case LIS_MATRIX_DNS:
		err = lis_matrix_copy_dns(Ain,Aout);
		break;
	case LIS_MATRIX_COO:
		err = lis_matrix_copy_coo(Ain,Aout);
		break;
*/
		default:
			LIS_SETERR_IMP;
			*D = NULL;
			*L = NULL;
			*U = NULL;
			return LIS_ERR_NOT_IMPLEMENTED;
			break;
	}
	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_solve"
int lis_matrix_solve(LIS_MATRIX A, LIS_VECTOR b, LIS_VECTOR x, int flag)
{
	LIS_DEBUG_FUNC_IN;

	if( !A->is_splited ) lis_matrix_split(A);

	switch( A->matrix_type )
	{
	case LIS_MATRIX_CRS:
		lis_matrix_solve_crs(A,b,x,flag);
		break;
	case LIS_MATRIX_BSR:
		lis_matrix_solve_bsr(A,b,x,flag);
		break;
	case LIS_MATRIX_CCS:
		lis_matrix_solve_ccs(A,b,x,flag);
		break;
	case LIS_MATRIX_MSR:
		lis_matrix_solve_msr(A,b,x,flag);
		break;
	case LIS_MATRIX_ELL:
		lis_matrix_solve_ell(A,b,x,flag);
		break;
	case LIS_MATRIX_JDS:
		lis_matrix_solve_jds(A,b,x,flag);
		break;
	case LIS_MATRIX_DIA:
		lis_matrix_solve_dia(A,b,x,flag);
		break;
	case LIS_MATRIX_DNS:
		lis_matrix_solve_dns(A,b,x,flag);
		break;
	case LIS_MATRIX_BSC:
		lis_matrix_solve_bsc(A,b,x,flag);
		break;
	case LIS_MATRIX_VBR:
		lis_matrix_solve_vbr(A,b,x,flag);
		break;
	default:
		LIS_SETERR_IMP;
		return LIS_ERR_NOT_IMPLEMENTED;
		break;
	}

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_matrix_solvet"
int lis_matrix_solvet(LIS_MATRIX A, LIS_VECTOR b, LIS_VECTOR x, int flag)
{
	LIS_DEBUG_FUNC_IN;

	if( !A->is_splited ) lis_matrix_split(A);

	switch( A->matrix_type )
	{
	case LIS_MATRIX_CRS:
		lis_matrix_solvet_crs(A,b,x,flag);
		break;
	case LIS_MATRIX_BSR:
		lis_matrix_solvet_bsr(A,b,x,flag);
		break;
	case LIS_MATRIX_CCS:
		lis_matrix_solvet_ccs(A,b,x,flag);
		break;
	case LIS_MATRIX_MSR:
		lis_matrix_solvet_msr(A,b,x,flag);
		break;
	case LIS_MATRIX_ELL:
		lis_matrix_solvet_ell(A,b,x,flag);
		break;
	case LIS_MATRIX_JDS:
		lis_matrix_solvet_jds(A,b,x,flag);
		break;
	case LIS_MATRIX_DIA:
		lis_matrix_solvet_dia(A,b,x,flag);
		break;
	case LIS_MATRIX_DNS:
		lis_matrix_solvet_dns(A,b,x,flag);
		break;
	case LIS_MATRIX_BSC:
		lis_matrix_solvet_bsc(A,b,x,flag);
		break;
	case LIS_MATRIX_VBR:
		lis_matrix_solvet_vbr(A,b,x,flag);
		break;
	default:
		LIS_SETERR_IMP;
		return LIS_ERR_NOT_IMPLEMENTED;
		break;
	}

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}


void lis_array_LUdecomp(int n, LIS_SCALAR *a)
{
	int i,j,k;
	LIS_SCALAR t;

	for(k=0;k<n;k++)
	{
		a[k*n+k] = 1.0 / a[k*n+k];
		for(i=k+1;i<n;i++)
		{
			t = a[k*n+i] * a[k*n+k];
			for(j=k+1;j<n;j++)
			{
				a[j*n+i] -= t * a[j*n+k];
			}
			a[k*n+i] = t;
		}
	}
}

void lis_array_invGauss(int n, LIS_SCALAR *a)
{
	int i,j,k;
	LIS_SCALAR t,*lu;

	lu = malloc(n*n*sizeof(LIS_SCALAR));
	memcpy(lu,a,n*n*sizeof(LIS_SCALAR));
	for(k=0;k<n;k++)
	{
		lu[k*n+k] = 1.0 / lu[k*n+k];
		for(i=k+1;i<n;i++)
		{
			t = lu[k*n+i] * lu[k*n+k];
			for(j=k+1;j<n;j++)
			{
				lu[j*n+i] -= t * lu[j*n+k];
			}
			lu[k*n+i] = t;
		}
	}
	for(k=0;k<n;k++)
	{
		for(i=0;i<n;i++)
		{
			 t = (i==k);
			 for(j=0;j<i;j++)
			 {
				 t -= lu[j*n+i] * a[k*n+j];
			 }
			 a[k*n+i] = t;
		}
		for(i=n-1;i>=0;i--)
		{
			t = a[k*n+i];
			for(j=i+1;j<n;j++)
			{
				t -= lu[j*n+i] * a[k*n+j];
			}
			a[k*n+i] = t * lu[i*n+i];
		}
	}
	free(lu);
}

void lis_array_matvec(int n, LIS_SCALAR *a, LIS_SCALAR *b, LIS_SCALAR *c, int op)
{
	int			i,j;
	LIS_SCALAR	t;
	/* c = A*b */

	if( op==LIS_INS_VALUE )
	{
		switch(n)
		{
		case 1:
			c[0] = a[0]*b[0];
			break;
		case 2:
			c[0] = a[0]*b[0] + a[2]*b[1];
			c[1] = a[1]*b[0] + a[3]*b[1];
			break;
		case 3:
			c[0] = a[0]*b[0] + a[3]*b[1] + a[6]*b[2];
			c[1] = a[1]*b[0] + a[4]*b[1] + a[7]*b[2];
			c[2] = a[2]*b[0] + a[5]*b[1] + a[8]*b[2];
			break;
		default:
			for(i=0;i<n;i++)
			{
				t = 0.0;
				for(j=0;j<n;j++)
				{
					t += a[j*n+i] * b[j];
				}
				c[i] = t;
			}
			break;
		}
	}
	else if( op==LIS_SUB_VALUE )
	{
		switch(n)
		{
		case 1:
			c[0] -= a[0]*b[0];
			break;
		case 2:
			c[0] -= a[0]*b[0] + a[2]*b[1];
			c[1] -= a[1]*b[0] + a[3]*b[1];
			break;
		case 3:
			c[0] -= a[0]*b[0] + a[3]*b[1] + a[6]*b[2];
			c[1] -= a[1]*b[0] + a[4]*b[1] + a[7]*b[2];
			c[2] -= a[2]*b[0] + a[5]*b[1] + a[8]*b[2];
			break;
		default:
			for(i=0;i<n;i++)
			{
				t = 0.0;
				for(j=0;j<n;j++)
				{
					t += a[j*n+i] * b[j];
				}
				c[i] -= t;
			}
			break;
		}
	}
	else
	{
		switch(n)
		{
		case 1:
			c[0] += a[0]*b[0];
			break;
		case 2:
			c[0] += a[0]*b[0] + a[2]*b[1];
			c[1] += a[1]*b[0] + a[3]*b[1];
			break;
		case 3:
			c[0] += a[0]*b[0] + a[3]*b[1] + a[6]*b[2];
			c[1] += a[1]*b[0] + a[4]*b[1] + a[7]*b[2];
			c[2] += a[2]*b[0] + a[5]*b[1] + a[8]*b[2];
			break;
		default:
			for(i=0;i<n;i++)
			{
				t = 0.0;
				for(j=0;j<n;j++)
				{
					t += a[j*n+i] * b[j];
				}
				c[i] += t;
			}
			break;
		}
	}
}

void lis_array_matvect(int n, LIS_SCALAR *a, LIS_SCALAR *b, LIS_SCALAR *c, int op)
{
	int			i,j;
	LIS_SCALAR	t;
	/* c = A*b */

	if( op==LIS_INS_VALUE )
	{
		switch(n)
		{
		case 1:
			c[0] = a[0]*b[0];
			break;
		case 2:
			c[0] = a[0]*b[0] + a[1]*b[1];
			c[1] = a[2]*b[0] + a[3]*b[1];
			break;
		case 3:
			c[0] = a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
			c[1] = a[3]*b[0] + a[4]*b[1] + a[5]*b[2];
			c[2] = a[6]*b[0] + a[7]*b[1] + a[8]*b[2];
			break;
		default:
			for(i=0;i<n;i++)
			{
				t = 0.0;
				for(j=0;j<n;j++)
				{
					t += a[i*n+j] * b[j];
				}
				c[i] = t;
			}
			break;
		}
	}
	else if( op==LIS_SUB_VALUE )
	{
		switch(n)
		{
		case 1:
			c[0] -= a[0]*b[0];
			break;
		case 2:
			c[0] -= a[0]*b[0] + a[1]*b[1];
			c[1] -= a[2]*b[0] + a[3]*b[1];
			break;
		case 3:
			c[0] -= a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
			c[1] -= a[3]*b[0] + a[4]*b[1] + a[5]*b[2];
			c[2] -= a[6]*b[0] + a[7]*b[1] + a[8]*b[2];
			break;
		default:
			for(i=0;i<n;i++)
			{
				t = 0.0;
				for(j=0;j<n;j++)
				{
					t += a[i*n+j] * b[j];
				}
				c[i] -= t;
			}
			break;
		}
	}
	else
	{
		switch(n)
		{
		case 1:
			c[0] += a[0]*b[0];
			break;
		case 2:
			c[0] += a[0]*b[0] + a[1]*b[1];
			c[1] += a[2]*b[0] + a[3]*b[1];
			break;
		case 3:
			c[0] += a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
			c[1] += a[3]*b[0] + a[4]*b[1] + a[5]*b[2];
			c[2] += a[6]*b[0] + a[7]*b[1] + a[8]*b[2];
			break;
		default:
			for(i=0;i<n;i++)
			{
				t = 0.0;
				for(j=0;j<n;j++)
				{
					t += a[i*n+j] * b[j];
				}
				c[i] += t;
			}
			break;
		}
	}
}

void lis_array_matmat(int n, LIS_SCALAR *a, LIS_SCALAR *b, LIS_SCALAR *c, int op)
{
	/* C = A*B */

	if( op==LIS_INS_VALUE )
	{
		switch(n)
		{
		case 1:
			c[0] = a[0]*b[0];
			break;
		case 2:
			c[0] = a[0]*b[0] + a[2]*b[1];
			c[1] = a[1]*b[0] + a[3]*b[1];
			c[2] = a[0]*b[2] + a[2]*b[3];
			c[3] = a[1]*b[2] + a[3]*b[3];
			break;
		case 3:
			c[0] = a[0]*b[0] + a[3]*b[1] + a[6]*b[2];
			c[1] = a[1]*b[0] + a[4]*b[1] + a[7]*b[2];
			c[2] = a[2]*b[0] + a[5]*b[1] + a[8]*b[2];
			c[3] = a[0]*b[3] + a[3]*b[4] + a[6]*b[5];
			c[4] = a[1]*b[3] + a[4]*b[4] + a[7]*b[5];
			c[5] = a[2]*b[3] + a[5]*b[4] + a[8]*b[5];
			c[6] = a[0]*b[6] + a[3]*b[7] + a[6]*b[8];
			c[7] = a[1]*b[6] + a[4]*b[7] + a[7]*b[8];
			c[8] = a[2]*b[6] + a[5]*b[7] + a[8]*b[8];
			break;
		}
	}
	else if( op==LIS_SUB_VALUE )
	{
		switch(n)
		{
		case 1:
			c[0] -= a[0]*b[0];
			break;
		case 2:
			c[0] -= a[0]*b[0] + a[2]*b[1];
			c[1] -= a[1]*b[0] + a[3]*b[1];
			c[2] -= a[0]*b[2] + a[2]*b[3];
			c[3] -= a[1]*b[2] + a[3]*b[3];
			break;
		case 3:
			c[0] -= a[0]*b[0] + a[3]*b[1] + a[6]*b[2];
			c[1] -= a[1]*b[0] + a[4]*b[1] + a[7]*b[2];
			c[2] -= a[2]*b[0] + a[5]*b[1] + a[8]*b[2];
			c[3] -= a[0]*b[3] + a[3]*b[4] + a[6]*b[5];
			c[4] -= a[1]*b[3] + a[4]*b[4] + a[7]*b[5];
			c[5] -= a[2]*b[3] + a[5]*b[4] + a[8]*b[5];
			c[6] -= a[0]*b[6] + a[3]*b[7] + a[6]*b[8];
			c[7] -= a[1]*b[6] + a[4]*b[7] + a[7]*b[8];
			c[8] -= a[2]*b[6] + a[5]*b[7] + a[8]*b[8];
			break;
		}
	}
	else
	{
		switch(n)
		{
		case 1:
			c[0] += a[0]*b[0];
			break;
		case 2:
			c[0] += a[0]*b[0] + a[2]*b[1];
			c[1] += a[1]*b[0] + a[3]*b[1];
			c[2] += a[0]*b[2] + a[2]*b[3];
			c[3] += a[1]*b[2] + a[3]*b[3];
			break;
		case 3:
			c[0] += a[0]*b[0] + a[3]*b[1] + a[6]*b[2];
			c[1] += a[1]*b[0] + a[4]*b[1] + a[7]*b[2];
			c[2] += a[2]*b[0] + a[5]*b[1] + a[8]*b[2];
			c[3] += a[0]*b[3] + a[3]*b[4] + a[6]*b[5];
			c[4] += a[1]*b[3] + a[4]*b[4] + a[7]*b[5];
			c[5] += a[2]*b[3] + a[5]*b[4] + a[8]*b[5];
			c[6] += a[0]*b[6] + a[3]*b[7] + a[6]*b[8];
			c[7] += a[1]*b[6] + a[4]*b[7] + a[7]*b[8];
			c[8] += a[2]*b[6] + a[5]*b[7] + a[8]*b[8];
			break;
		}
	}
}

void lis_array_matmat2(int m, int n, int k, LIS_SCALAR *a, int lda, LIS_SCALAR *b, int ldb, LIS_SCALAR *c, int ldc, int op)
{
	int			i,j,l;
	/* C = A*B */

	if( op==LIS_INS_VALUE )
	{
		for(j=0;j<n;j++)
		{
			for(i=0;i<m;i++)
			{
				c[j*ldc+i] = 0.0;
			}
			for(l=0;l<k;l++)
			{
				for(i=0;i<m;i++)
				{
					c[j*ldc+i] += b[j*ldb+l] * a[l*lda+i];
				}
			}
		}
	}
	else if( op==LIS_SUB_VALUE )
	{
		for(j=0;j<n;j++)
		{
			for(l=0;l<k;l++)
			{
				for(i=0;i<m;i++)
				{
					c[j*ldc+i] -= b[j*ldb+l] * a[l*lda+i];
				}
			}
		}
	}
	else
	{
		switch(n)
		{
		case 1:
			c[0] += a[0]*b[0];
			break;
		case 2:
			c[0] += a[0]*b[0] + a[2]*b[1];
			c[1] += a[1]*b[0] + a[3]*b[1];
			c[2] += a[0]*b[2] + a[2]*b[3];
			c[3] += a[1]*b[2] + a[3]*b[3];
			break;
		case 3:
			c[0] += a[0]*b[0] + a[3]*b[1] + a[6]*b[2];
			c[1] += a[1]*b[0] + a[4]*b[1] + a[7]*b[2];
			c[2] += a[2]*b[0] + a[5]*b[1] + a[8]*b[2];
			c[3] += a[0]*b[3] + a[3]*b[4] + a[6]*b[5];
			c[4] += a[1]*b[3] + a[4]*b[4] + a[7]*b[5];
			c[5] += a[2]*b[3] + a[5]*b[4] + a[8]*b[5];
			c[6] += a[0]*b[6] + a[3]*b[7] + a[6]*b[8];
			c[7] += a[1]*b[6] + a[4]*b[7] + a[7]*b[8];
			c[8] += a[2]*b[6] + a[5]*b[7] + a[8]*b[8];
			break;
		}
	}
}

void lis_array_nrm2(int n, LIS_SCALAR *v, LIS_SCALAR *nrm2)
{
	int i;
	LIS_SCALAR t;

	t = 0.0;
	for(i=0;i<n;i++)
	{
		t += v[i]*v[i];
	}
	*nrm2 = sqrt(t);
}

void lis_array_nrm1(int n, LIS_SCALAR *v, LIS_SCALAR *nrm1)
{
	int i;
	LIS_SCALAR t;

	t = 0.0;
	for(i=0;i<n;i++)
	{
		t += fabs(v[i]);
	}
	*nrm1 = t;
}

void lis_array_dot(int n, LIS_SCALAR *v, LIS_SCALAR *dot)
{
	int i;
	LIS_SCALAR t;

	t = 0.0;
	for(i=0;i<n;i++)
	{
		t += v[i]*v[i];
	}
	*dot = t;
}

void lis_array_matinv(int n, LIS_SCALAR *a, LIS_SCALAR *b, LIS_SCALAR *c)
{
	int i,j,k;
	LIS_SCALAR t;

	for(i=0;i<n;i++)
	{
		c[i] = -b[i] * a[0];
		for(j=1;j<n;j++)
		{
			t = -b[j*n+i];
			for(k=0;k<j-1;k++)
			{
				t -= c[k*n+i] * a[j*n+k];
			}
			c[j*n+i] = t * a[j*n+j];
		}
	}
	for(i=0;i<n;i++)
	{
		for(j=n-1;j>=0;j--)
		{
			t = c[j*n+i];
			for(k=j+1;k<n;k++)
			{
				t -= c[k*n+i] * a[j*n+k];
			}
			c[j*n+i] = t;
		}
	}
}

void lis_array_invvec(int n, LIS_SCALAR *a, LIS_SCALAR *x, LIS_SCALAR *y)
{
	/* y = inv(a) * x */
	int i,j;
	LIS_SCALAR t;

	for(i=0;i<n;i++)
	{
		t = x[i];
		for(j=0;j<i;j++)
		{
			t -= a[j*n+i] * y[j];
		}
		y[i] = t;
	}
	for(i=n-1;i>=0;i--)
	{
		t = y[i];
		for(j=i+1;j<n;j++)
		{
			t -= a[j*n+i] * y[j];
		}
		y[i] = t * a[i*n+i];
	}
}

void lis_array_invvect(int n, LIS_SCALAR *a, LIS_SCALAR *x, LIS_SCALAR *y)
{
	/* y = inv(a) * x */
	int i,j;

	for(i=0;i<n;i++) y[i] = x[i];
	for(i=0;i<n;i++)
	{
		y[i] = a[i*n+i] * y[i];
		for(j=i+1;j<n;j++)
		{
			y[j] -= a[j*n+i] * y[i];
		}
	}
	for(i=n-1;i>=0;i--)
	{
		for(j=0;j<i;j++)
		{
			y[j] -= a[j*n+i] * y[i];
		}
	}
}

void lis_array_matvec2(int m, int n, LIS_SCALAR *a, int lda, LIS_SCALAR *b, LIS_SCALAR *c, int op)
{
	int			i,j;
	LIS_SCALAR	t;
	/* c = A*b */

	if( op==LIS_INS_VALUE )
	{
		for(i=0;i<m;i++)
		{
			t = 0.0;
			for(j=0;j<n;j++)
			{
				t += a[j*lda+i] * b[j];
			}
			c[i] = t;
		}
	}
	else if( op==LIS_SUB_VALUE )
	{
		for(i=0;i<m;i++)
		{
			t = 0.0;
			for(j=0;j<n;j++)
			{
				t += a[j*lda+i] * b[j];
			}
			c[i] -= t;
		}
	}
	else if( op==LIS_ADD_VALUE )
	{
		for(i=0;i<m;i++)
		{
			t = 0.0;
			for(j=0;j<n;j++)
			{
				t += a[j*lda+i] * b[j];
			}
			c[i] += t;
		}
	}
	else
	{
		switch(n)
		{
		case 1:
			c[0] += a[0]*b[0];
			break;
		case 2:
			c[0] += a[0]*b[0] + a[2]*b[1];
			c[1] += a[1]*b[0] + a[3]*b[1];
			break;
		case 3:
			c[0] += a[0]*b[0] + a[3]*b[1] + a[6]*b[2];
			c[1] += a[1]*b[0] + a[4]*b[1] + a[7]*b[2];
			c[2] += a[2]*b[0] + a[5]*b[1] + a[8]*b[2];
			break;
		default:
			for(i=0;i<n;i++)
			{
				t = 0.0;
				for(j=0;j<n;j++)
				{
					t += a[j*n+i] * b[j];
				}
				c[i] += t;
			}
			break;
		}
	}
}

void lis_array_solve(int n, LIS_SCALAR *aa, LIS_SCALAR *b, LIS_SCALAR *x,
LIS_SCALAR *a)
{
	int i,j,k,imax,swap;
	LIS_SCALAR t,al;

	for(i=0;i<n*n;i++) a[i] = aa[i];

	switch( n )
	{
	case 1:
		x[0] = b[0] / a[0];
		break;
	case 2:
		a[0]  = 1.0 / a[0];
		a[1] *= a[0];
		a[3] -= a[1] * a[2];
		a[3]  = 1.0 / a[3];
		/* forward sub */
		x[0] = b[0];
		x[1] = b[1] - a[1] * x[0];
		/* backward sub */
		x[1] *= a[3];
		x[0] -= a[2] * x[1];
		x[0] *= a[0];
		break;
	default:
		for(k=0;k<n;k++)
		{
			a[k*n+k] = 1.0 / a[k*n+k];
			for(i=k+1;i<n;i++)
			{
				t = a[k*n+i] * a[k*n+k];
				for(j=k+1;j<n;j++)
				{
					a[j*n+i] -= t * a[j*n+k];
				}
				a[k*n+i] = t;
			}
		}

		/* forward sub */
		for(i=0;i<n;i++)
		{
			x[i] = b[i];
			for(j=0;j<i;j++)
			{
				x[i] -= a[j*n+i] * x[j];
			}
		}
		/* backward sub */
		for(i=n-1;i>=0;i--)
		{
			for(j=i+1;j<n;j++)
			{
				x[i] -= a[j*n+i] * x[j];
			}
			x[i] *= a[i*n+i];
		}
		break;
	}
}
