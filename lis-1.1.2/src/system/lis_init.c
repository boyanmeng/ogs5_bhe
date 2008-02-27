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
#ifdef USE_SSE2
	#include <emmintrin.h>
#endif
#ifdef _OPENMP
	#include <omp.h>
#endif
#ifdef USE_MPI
	#include <mpi.h>
#endif
#include "lislib.h"

LIS_ARGS	cmd_args = NULL;
int			lis_mpi_initialized = LIS_FALSE;
LIS_SCALAR	*lis_vec_tmp = NULL;

#ifdef USE_QUAD_PRECISION
	#define LIS_QUAD_SCALAR_SIZE 128
	double *lis_quad_scalar_tmp = NULL;
	unsigned int lis_x87_fpu_cw = 0;
#endif

#ifdef USE_MPI
#define LIS_MPI_MSCALAR_LEN		2
	MPI_Op			LIS_MPI_MSUM;
	MPI_Datatype	LIS_MPI_MSCALAR;
	extern void lis_mpi_msum(LIS_QUAD *invec, LIS_QUAD *inoutvec, int *len, MPI_Datatype *datatype);
#endif

#define LIS_INIT_OPTIONS_LEN	1
#define LIS_INIT_OPTIONS_OMPNUMTHREADS		1

char *LIS_INIT_OPTNAME[] = {
	"-omp_num_threads"
};
int LIS_INIT_OPTACT[] = {
	LIS_INIT_OPTIONS_OMPNUMTHREADS
};


/************************************************
 * lis_initialize
 * lis_finalize
 ************************************************/

#undef __FUNC__
#define __FUNC__ "lis_version"
void lis_version(void)
{
	LIS_DEBUG_FUNC_IN;

	lis_printf(LIS_COMM_WORLD,"Lis Version %s\n",LIS_VERSION);

	LIS_DEBUG_FUNC_OUT;
}

#undef __FUNC__
#define __FUNC__ "lis_initialize"
int lis_initialize(int* argc, char** argv[])
{
	LIS_ARGS	p;
	int		i,nprocs;

	LIS_DEBUG_FUNC_IN;

/*	lis_memory_init();*/

	#ifdef USE_MPI
		MPI_Initialized(&lis_mpi_initialized);
		if (!lis_mpi_initialized) MPI_Init(argc, argv);

		#ifdef USE_QUAD_PRECISION
			MPI_Type_contiguous( LIS_MPI_MSCALAR_LEN, MPI_DOUBLE, &LIS_MPI_MSCALAR );
			MPI_Type_commit( &LIS_MPI_MSCALAR );
			MPI_Op_create((MPI_User_function *)lis_mpi_msum, LIS_TRUE, &LIS_MPI_MSUM);
		#endif
	#endif

	#ifdef _OPENMP
		nprocs = omp_get_max_threads();
	#endif

	lis_arg2args(*argc,*argv,&cmd_args);
	p = cmd_args->next;
	while( p!=cmd_args )
	{
		for(i=0;i<LIS_INIT_OPTIONS_LEN;i++)
		{
			if( strcmp(p->arg1, LIS_INIT_OPTNAME[i])==0 )
			{
				switch( LIS_INIT_OPTACT[i] )
				{
				case LIS_INIT_OPTIONS_OMPNUMTHREADS:
					sscanf(p->arg2, "%d", &nprocs);
					break;
				}
			}
		}
		p = p->next;
	}

	#ifdef _OPENMP
		omp_set_num_threads(nprocs);
		lis_vec_tmp = (LIS_SCALAR *)lis_malloc( nprocs*LIS_VEC_TMP_PADD*sizeof(LIS_QUAD),"lis_initialize::lis_vec_tmp" );
		if( lis_vec_tmp==NULL )
		{
			LIS_SETERR_MEM(nprocs*LIS_VEC_TMP_PADD*sizeof(LIS_QUAD));
			return LIS_ERR_OUT_OF_MEMORY;
		}
	#endif

	#ifdef USE_QUAD_PRECISION
		lis_quad_scalar_tmp = (LIS_SCALAR *)lis_malloc( LIS_QUAD_SCALAR_SIZE*sizeof(LIS_SCALAR),"lis_initialize::lis_quad_scalar_tmp" );
		if( lis_quad_scalar_tmp==NULL )
		{
			LIS_SETERR_MEM(LIS_QUAD_SCALAR_SIZE*sizeof(LIS_SCALAR));
			return LIS_OUT_OF_MEMORY;
		}
		lis_quad_x87_fpu_init(&lis_x87_fpu_cw);
	#endif

	for(i=1;i<*argc;i++)
	{
		if( strncmp(argv[0][i], "-help", 5)==0 )
		{
/*			lis_display();*/
			CHKERR(1);
		}
		else if( strncmp(argv[0][i], "-ver", 4)==0 )
		{
			lis_version();
			CHKERR(1);
		}
	}

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_finalize"
int lis_finalize(void)
{
	LIS_DEBUG_FUNC_IN;

	lis_precon_register_free();
	if( cmd_args ) lis_args_free(cmd_args);
	#ifdef _OPENMP
		lis_free(lis_vec_tmp);
	#endif
	#ifdef USE_QUAD_PRECISION
		lis_free(lis_quad_scalar_tmp);
		lis_quad_x87_fpu_finalize(lis_x87_fpu_cw);
	#endif

	lis_free_all();
	
	#ifdef USE_MPI
		if (!lis_mpi_initialized) MPI_Finalize();
	#endif

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_text2args"
int lis_text2args(char *text, LIS_ARGS *args)
{
	char buf[1024];
	char *p,*s1,*s2;
	int  k1,k2,f;
	LIS_ARGS arg_top,arg;

	LIS_DEBUG_FUNC_IN;

	arg_top = (LIS_ARGS)lis_malloc(sizeof(struct LIS_ARGS_STRUCT),"lis_text2args::arg_top");
	arg_top->next = arg_top;
	arg_top->prev = arg_top;
	arg_top->arg1 = NULL;
	arg_top->arg2 = NULL;

	strcpy(buf,text);
	p   = buf;
	f   = LIS_TRUE;
	while( *p!='\0' && f )
	{
		while( *p==' ' && *p=='\t' && *p=='\n' && *p=='\r' )
		{
			p++;
		}
		s1 = p;
		while( *p!=' ' && *p!='\t' && *p!='\n' && *p!='\r' && *p!='\0' )
		{
			*p = (char)tolower(*p);
			p++;
		}
		if( *p=='\0' ) break;
		*p++ = '\0';
		while( *p==' ' && *p=='\t' && *p=='\n' && *p=='\r' )
		{
			p++;
		}
		s2 = p;
		while( *p!=' ' && *p!='\t' && *p!='\n' && *p!='\r' && *p!='\0' )
		{
			*p = (char)tolower(*p);
			p++;
		}
		if( *p=='\0' ) f = LIS_FALSE;
		*p++ = '\0';
		k1 = (int)strlen(s1);
		k2 = (int)strlen(s2);
		if( k1>0 && k2>0 )
		{
			arg             = (LIS_ARGS)lis_malloc(sizeof(struct LIS_ARGS_STRUCT),"lis_text2args::arg");
			arg->arg1       = (char *)lis_malloc((k1+1)*sizeof(char),"lis_text2args::arg->arg1");
			arg->arg2       = (char *)lis_malloc((k2+1)*sizeof(char),"lis_text2args::arg->arg2");
			arg->next       = arg_top;
			arg->prev       = arg_top->prev;
			arg->prev->next = arg;
			arg->next->prev = arg;
			strcpy(arg->arg1,s1);
			strcpy(arg->arg2,s2);
		}
	}

	*args = arg_top;

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}

#undef __FUNC__
#define __FUNC__ "lis_arg2args"
int lis_arg2args(int argc, char *argv[], LIS_ARGS *args)
{
	char *p;
	int  i,k1,k2;
	LIS_ARGS arg_top,arg;

	LIS_DEBUG_FUNC_IN;

	arg_top = (LIS_ARGS)lis_malloc(sizeof(struct LIS_ARGS_STRUCT),"lis_arg2args::arg_top");
	arg_top->next = arg_top;
	arg_top->prev = arg_top;
	arg_top->arg1 = NULL;
	arg_top->arg2 = NULL;

	i = 1;
	while( i<argc )
	{
		if( argv[i][0]=='-' && (i+1)<argc )
		{
			k1              = (int)strlen(argv[i]);
			k2              = (int)strlen(argv[i+1]);
			arg             = (LIS_ARGS)lis_malloc(sizeof(struct LIS_ARGS_STRUCT),"lis_arg2args::arg");
			arg->arg1       = (char *)lis_malloc((k1+1)*sizeof(char),"lis_arg2args::arg->arg1");
			arg->arg2       = (char *)lis_malloc((k2+1)*sizeof(char),"lis_arg2args::arg->arg2");
			arg->next       = arg_top;
			arg->prev       = arg_top->prev;
			arg->prev->next = arg;
			arg->next->prev = arg;
			strcpy(arg->arg1,argv[i]);
			strcpy(arg->arg2,argv[i+1]);
			p = arg->arg1;
			while( *p!='\0' )
			{
				*p = (char)tolower(*p);
				p++;
			}
			p = arg->arg2;
			while( *p!='\0' )
			{
				*p = (char)tolower(*p);
				p++;
			}
			i += 2;
		}
		else
		{
			i++;
		}
	}

	*args = arg_top;

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}


#undef __FUNC__
#define __FUNC__ "lis_args_free"
int lis_args_free(LIS_ARGS args)
{
	LIS_ARGS arg,t;

	LIS_DEBUG_FUNC_IN;

	arg = args->next;
	while( arg!=args )
	{
		t             = arg;
		arg           = arg->next;
		lis_free2(2,t->arg1,t->arg2);
		t->next->prev = t->prev;
		t->prev->next = t->next;
		lis_free(t);
	}
	lis_free(args);

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}


#undef __FUNC__
#define __FUNC__ "lis_ranges_create"
int lis_ranges_create(LIS_Comm comm, int *local_n, int *global_n, int **ranges, int *is, int *ie, int *nprocs, int *my_rank)
{
	#ifdef USE_MPI
		int		i;
	#endif
	int		*tranges;

	LIS_DEBUG_FUNC_IN;

	#ifdef USE_MPI
		MPI_Comm_size(comm,nprocs);
		MPI_Comm_rank(comm,my_rank);
		tranges = (int *)lis_malloc( (*nprocs+1)*sizeof(int),"lis_ranges_create::tranges" );
		if( tranges==NULL )
		{
			LIS_SETERR_MEM((*nprocs+1)*sizeof(int));
			return LIS_OUT_OF_MEMORY;
		}
	#else
		*nprocs  = 1;
		*my_rank = 0;
		tranges  = NULL;
	#endif

	#ifdef USE_MPI
		MPI_Allreduce(local_n,&i,1,MPI_INT,MPI_SUM,comm);
		if( i==0 )
	#else
		if( *local_n==0 )
	#endif
	{
		#ifdef USE_MPI
			LIS_GET_ISIE(*my_rank,*nprocs,*global_n,*is,*ie);
			*local_n = *ie-*is;
			MPI_Allgather(ie,1,MPI_INT,&tranges[1],1,MPI_INT,comm);
			tranges[0] = 0;
		#else
			*local_n = *global_n;
			*is      = 0;
			*ie      = *global_n;
		#endif
	}
	else
	{
		#ifdef USE_MPI
			MPI_Allgather(local_n,1,MPI_INT,&tranges[1],1,MPI_INT,comm);
			tranges[0] = 0;
			for(i=0;i<*nprocs;i++)
			{
				tranges[i+1] += tranges[i];
			}
			*global_n = tranges[*nprocs];
			*is       = tranges[*my_rank];
			*ie       = tranges[*my_rank+1];
		#else
			*global_n = *local_n;
			*is       = 0;
			*ie       = *local_n;
		#endif
	}
	*ranges = tranges;

	LIS_DEBUG_FUNC_OUT;
	return LIS_SUCCESS;
}
