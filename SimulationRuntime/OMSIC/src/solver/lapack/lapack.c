/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2014, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF THE BSD NEW LICENSE OR THE
 * GPL VERSION 3 LICENSE OR THE OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
 * ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs: http://www.openmodelica.org or
 * http://www.ida.liu.se/projects/OpenModelica, and in the OpenModelica
 * distribution. GNU version 3 is obtained from:
 * http://www.gnu.org/copyleft/gpl.html. The New BSD License is obtained from:
 * http://www.opensource.org/licenses/BSD-3-Clause.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, EXCEPT AS
 * EXPRESSLY SET FORTH IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE
 * CONDITIONS OF OSMC-PL.
 *
 */

#include "solver/lapack.h"

/* forward global functions */
omsi_callback_allocate_memory   global_allocateMemory;
omsi_callback_free_memory       global_freeMemory;


/*
 * Get an equation system A*x=b in omsi_function_t format.
 * Compute the solution using LAPACK DGESV.
 * If a correct solution is found omsi_ok is returned and equationSystemFunc
 * was updated.
 * If omsi_warning is returned the found solution is not correct. Then
 * equationSysteFunc is not updated.
 * If omsi_error is returned a critical error occurred.
 */
omsi_status solveLapack(omsi_algebraic_system_t* linearSystem, omsi_callback_functions* callback_functions){

    /* variables */
    DATA_LAPACK* lapack_data;
    omsi_status status;

    /* set global functions */
    global_allocateMemory = callback_functions->allocateMemory;
    global_freeMemory = callback_functions->freeMemory;

    /* allocate memory and copy informations into lapack_data */
    lapack_data = set_lapack_data((const omsi_algebraic_system_t*) linearSystem);
    if (lapack_data == NULL) {
        return omsi_error;
    }


    /* solve equation system */
    dgesv_(&lapack_data->n,
           &lapack_data->nrhs,
            lapack_data->A,
           &lapack_data->lda,
            lapack_data->ipiv,
            lapack_data->b,
           &lapack_data->ldb,
           &lapack_data->info);

    if (lapack_data->info < 0) {
        /* ToDO: Log */
        printf("ERROR solving linear system: the %d-th argument had an illegal value\n", (-1)*lapack_data->info);
        freeLapackData(lapack_data);
        return omsi_error;
    }
    else if (lapack_data->info > 0) {
        /* ToDo: Log */
        printf("ERROR solving linear system:  U(%d,%d) is exactly zero.\n", lapack_data->info, lapack_data->info);
        printf("The factorization has been completed, but the factor U is exactly"
                "singular, so the solution could not be computed\n");
        freeLapackData(lapack_data);
        return omsi_error;
    }

    /* check if solution is correct */
    status = eval_residual(lapack_data, linearSystem);
    if (status == omsi_ok) {
        /* copy solution into equationSystemFunc */

    }
    else {
        /* solution is not accuracte enough.
         * ToDo: Still proceed with this solution or or reject solution?
         */
    }

    /* free memory */
    freeLapackData(lapack_data);

    return status;
}


/*
 * Copies necessary informations from equationSystemFunc into a new created
 * lapack_data structure.
 * Input:   equationSystemFunc
 * Output:  pointer to lapack_data, if pointer=NULL an error occurred
 */
DATA_LAPACK* set_lapack_data(const omsi_algebraic_system_t* linear_system) {

    DATA_LAPACK* lapack_data = (DATA_LAPACK*) global_allocateMemory(1, sizeof(DATA_LAPACK));
    if (!lapack_data) {
        /* ToDo: log error out of memory */
        return NULL;
    }

    lapack_data->n = linear_system->functions->n_output_vars;
    lapack_data->nrhs = 1;
    lapack_data->lda = lapack_data->n;
    lapack_data->ldb = lapack_data->n;

    /* allocate memory */
    lapack_data->A = (omsi_real*) global_allocateMemory(lapack_data->lda*lapack_data->n, sizeof(omsi_real));
    lapack_data->ipiv = (omsi_int*) global_allocateMemory(lapack_data->n, sizeof(omsi_int));
    lapack_data->b = (omsi_real*) global_allocateMemory(lapack_data->ldb*lapack_data->nrhs, sizeof(omsi_real));
    if (!lapack_data->A || !lapack_data->ipiv || !lapack_data->b) {
        /* ToDo: log error out of memory */
        return NULL;
    }

    set_lapack_a(lapack_data, linear_system);
    set_lapack_b(lapack_data, linear_system);

    return lapack_data;
}

/*
 * Read data from equationSystemFunc and
 * set matrix A in row-major order.
 */
void set_lapack_a (DATA_LAPACK* lapack_data, const omsi_algebraic_system_t* linear_system) {

    omsi_int i,j;

    for (i=0; i<lapack_data->lda; i++) {
        for (j=0; j<lapack_data->n; j++) {
            /* copy data from column-major to row-major style */
            lapack_data->A[i+j*lapack_data->lda] = linear_system->functions->function_vars->reals[i*lapack_data->n+j];
            /* ToDo: where exactly is this data stored in function ???? */
        }
    }
}

/*
 * Read data from equationSystemFunc and
 * set vector b.
 */
omsi_status set_lapack_b (DATA_LAPACK* lapack_data, const omsi_algebraic_system_t* linearSystem) {

    omsi_unsigned_int i;
    omsi_unsigned_int j=0;

    /* set iteration vars to zero */
    /* ToDo: Do we want to save iteration vars first? */
    if (!omsu_set_omsi_value(linearSystem->functions->function_vars, *(linearSystem->iteration_vars_indices), linearSystem->n_iteration_vars, 0)) {
        return omsi_error;
    }

    /* evaluate residual function A*0-b=0 to get -b */
    linearSystem->functions->evaluate(linearSystem->functions, lapack_data->b);

    /* flip sign */
    for (i=0; i<lapack_data->ldb; i++) {
        lapack_data->b[i] = -lapack_data->b[i];
    }

    return omsi_ok;
}


/*
 * Evaluate residual A*x-b=res and return omsi_ok if it is zero,
 * otherwise omsi_warning.
 */
omsi_status eval_residual(DATA_LAPACK* lapack_data, omsi_algebraic_system_t* linearSystem) {
    /* local variables */
    omsi_int increment = 1;

    omsi_real* res;        /* pointer for residuum vector */
    omsi_real dotProduct;

    /* allocate memory */
    res = (omsi_real*) global_allocateMemory(lapack_data->n, sizeof(omsi_real));

    /* compute residuum A*x-b using generated function and save result in residuum */
    /* ToDo: function call */
    linearSystem->functions->evaluate(linearSystem->functions, res);

    /* compute dot product <residuum, residuum> */
    dotProduct = ddot_(&lapack_data->n, res, &increment, res, &increment);

    /* free memory */
    global_freeMemory(res);

    if (dotProduct < 1e-4) {  /* ToDo: use some accuracy */
        return omsi_ok;
    }
    else {
        /* ToDo: log Solution is not accurate enough */
        return omsi_warning;
    }
}


/*
 * Writes solution generated by solver call into omsi_function_t->function_vars
 * Input:   lapack_data
 * Output:  equationSystemFunc
 */
void get_result(omsi_function_t*    equationSystemFunc,
                DATA_LAPACK*        lapack_data) {

    omsi_unsigned_int i;
    omsi_unsigned_int index;

    for (i=0; i<equationSystemFunc->n_output_vars;i++) {
        index = equationSystemFunc->output_vars_indices[i].index;
        equationSystemFunc->function_vars->reals[index] = lapack_data->b[i];
    }
}


/*
 *  Frees lapack_data
 */
void freeLapackData(DATA_LAPACK* lapack_data) {
    global_freeMemory(lapack_data->A);
    global_freeMemory(lapack_data->ipiv);
    global_freeMemory(lapack_data->b);

    global_freeMemory(lapack_data);
}
