/**************************************************************************************************
*                                                                                                 *
* This file is part of HPIPM.                                                                     *
*                                                                                                 *
* HPIPM -- High Performance Interior Point Method.                                                *
* Copyright (C) 2017 by Gianluca Frison.                                                          *
* Developed at IMTEK (University of Freiburg) under the supervision of Moritz Diehl.              *
* All rights reserved.                                                                            *
*                                                                                                 *
* HPMPC is free software; you can redistribute it and/or                                          *
* modify it under the terms of the GNU Lesser General Public                                      *
* License as published by the Free Software Foundation; either                                    *
* version 2.1 of the License, or (at your option) any later version.                              *
*                                                                                                 *
* HPMPC is distributed in the hope that it will be useful,                                        *
* but WITHOUT ANY WARRANTY; without even the implied warranty of                                  *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                                            *
* See the GNU Lesser General Public License for more details.                                     *
*                                                                                                 *
* You should have received a copy of the GNU Lesser General Public                                *
* License along with HPMPC; if not, write to the Free Software                                    *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA                  *
*                                                                                                 *
* Author: Gianluca Frison, gianluca.frison (at) imtek.uni-freiburg.de                             *
*                                                                                                 *
**************************************************************************************************/



#include <stdlib.h>
#include <stdio.h>
#ifdef USE_C99_MATH
#include <math.h>
#endif

#include <blasfeo_target.h>
#include <blasfeo_common.h>
#include <blasfeo_d_aux.h>
#include <blasfeo_d_blas.h>

#include <hpipm_d_dense_qp_dim.h>
#include <hpipm_d_dense_qp.h>
#include <hpipm_d_dense_qp_sol.h>
#include <hpipm_d_dense_qp_res.h>
#include <hpipm_d_dense_qp_ipm.h>
#include <hpipm_d_core_qp_ipm.h>
#include <hpipm_d_core_qp_ipm_aux.h>
#include <hpipm_d_dense_qp_kkt.h>



#define AXPY blasfeo_daxpy
#define BACKUP_RES_M d_backup_res_m
#define COMPUTE_ALPHA_QP d_compute_alpha_qp
#define COMPUTE_CENTERING_CORRECTION_QP d_compute_centering_correction_qp
#define COMPUTE_CENTERING_QP d_compute_centering_qp
#define COMPUTE_LIN_RES_DENSE_QP d_compute_lin_res_dense_qp
#define COMPUTE_MU_AFF_QP d_compute_mu_aff_qp
#define COMPUTE_RES_DENSE_QP d_compute_res_dense_qp
#define CORE_QP_IPM_WORKSPACE d_core_qp_ipm_workspace
#define CREATE_CORE_QP_IPM d_create_core_qp_ipm
#define CREATE_DENSE_QP_RES d_create_dense_qp_res
#define CREATE_DENSE_QP_SOL d_create_dense_qp_sol
#define CREATE_STRMAT blasfeo_create_dmat
#define CREATE_STRVEC blasfeo_create_dvec
#define DENSE_QP d_dense_qp
#define DENSE_QP_IPM_ARG d_dense_qp_ipm_arg
#define HPIPM_MODE hpipm_mode
#define DENSE_QP_IPM_WORKSPACE d_dense_qp_ipm_workspace
#define DENSE_QP_DIM d_dense_qp_dim
#define DENSE_QP_RES d_dense_qp_res
#define DENSE_QP_RES_WORKSPACE d_dense_qp_res_workspace
#define DENSE_QP_SOL d_dense_qp_sol
#define DOT blasfeo_ddot
#define FACT_LQ_SOLVE_KKT_STEP_DENSE_QP d_fact_lq_solve_kkt_step_dense_qp
#define FACT_SOLVE_LU_KKT_STEP_DENSE_QP d_fact_solve_lu_kkt_step_dense_qp
#define FACT_SOLVE_KKT_STEP_DENSE_QP d_fact_solve_kkt_step_dense_qp
#define FACT_SOLVE_KKT_UNCONSTR_DENSE_QP d_fact_solve_kkt_unconstr_dense_qp
#define GELQF_WORKSIZE blasfeo_dgelqf_worksize
#define INIT_VAR_DENSE_QP d_init_var_dense_qp
#define MEMSIZE_CORE_QP_IPM d_memsize_core_qp_ipm
#define MEMSIZE_DENSE_QP_RES d_memsize_dense_qp_res
#define MEMSIZE_DENSE_QP_SOL d_memsize_dense_qp_sol
#define REAL double
#define SIZE_STRMAT blasfeo_memsize_dmat
#define SIZE_STRVEC blasfeo_memsize_dvec
#define SOLVE_KKT_STEP_DENSE_QP d_solve_kkt_step_dense_qp
#define STRMAT blasfeo_dmat
#define STRVEC blasfeo_dvec
#define UPDATE_VAR_QP d_update_var_qp
#define VECMUL blasfeo_dvecmul
#define VECMULDOT blasfeo_dvecmuldot
#define VECNRM_INF blasfeo_dvecnrm_inf
#define VECSC blasfeo_dvecsc



// arg
#define MEMSIZE_DENSE_QP_IPM_ARG d_memsize_dense_qp_ipm_arg
#define CREATE_DENSE_QP_IPM_ARG d_create_dense_qp_ipm_arg
#define SET_DEFAULT_DENSE_QP_IPM_ARG d_set_default_dense_qp_ipm_arg
#define SET_DENSE_QP_IPM_ARG_ITER_MAX d_set_dense_qp_ipm_arg_iter_max
#define SET_DENSE_QP_IPM_ARG_MU0 d_set_dense_qp_ipm_arg_mu0
#define SET_DENSE_QP_IPM_ARG_TOL_STAT d_set_dense_qp_ipm_arg_tol_stat
#define SET_DENSE_QP_IPM_ARG_TOL_EQ d_set_dense_qp_ipm_arg_tol_eq
#define SET_DENSE_QP_IPM_ARG_TOL_INEQ d_set_dense_qp_ipm_arg_tol_ineq
#define SET_DENSE_QP_IPM_ARG_TOL_COMP d_set_dense_qp_ipm_arg_tol_comp
#define SET_DENSE_QP_IPM_ARG_REG_PRIM d_set_dense_qp_ipm_arg_reg_prim
#define SET_DENSE_QP_IPM_ARG_REG_DUAL d_set_dense_qp_ipm_arg_reg_dual
// ipm
#define MEMSIZE_DENSE_QP_IPM d_memsize_dense_qp_ipm
#define CREATE_DENSE_QP_IPM d_create_dense_qp_ipm
#define GET_DENSE_QP_IPM_ITER d_get_dense_qp_ipm_iter
#define GET_DENSE_QP_IPM_RES_STAT d_get_dense_qp_ipm_res_stat
#define GET_DENSE_QP_IPM_RES_EQ d_get_dense_qp_ipm_res_eq
#define GET_DENSE_QP_IPM_RES_INEQ d_get_dense_qp_ipm_res_ineq
#define GET_DENSE_QP_IPM_RES_COMP d_get_dense_qp_ipm_res_comp
#define GET_DENSE_QP_IPM_STAT d_get_dense_qp_ipm_stat
#define SOLVE_DENSE_QP_IPM d_solve_dense_qp_ipm
#define SOLVE_DENSE_QP_IPM2 d_solve_dense_qp_ipm2



#include "x_dense_qp_ipm.c"
