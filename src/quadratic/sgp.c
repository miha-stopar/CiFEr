/* *
 * Copyright (C) 2018 XLAB d.o.o.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of either:
 *
 *     * the GNU Lesser General Public License as published by the Free
 *     Software Foundation; either version 3 of the License, or (at your
 *     option) any later version.
 *
 * or
 *
 *     * the GNU General Public License as published by the Free Software
 *     Foundation; either version 2 of the License, or (at your option) any
 *     later version.
 *
 * or both in parallel, as here.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


#include <stdlib.h>
#include <gmp.h>

#include "quadratic/sgp.h"
#include "internal/keygen.h"
#include "internal/dlog.h"
#include "sample/uniform.h"

#include <big_256_56.h>

cfe_error cfe_sgp_init(cfe_sgp *s, size_t n, mpz_t bound) {
    /*
    cfe_elgamal key;
    if (cfe_elgamal_init(&key, modulus_len)) {
        return CFE_ERR_PARAM_GEN_FAILED;
    }
     */

    cfe_error err = CFE_ERR_NONE;

    /*
    mpz_t check;
    mpz_init(check);

    mpz_pow_ui(check, bound, 2);
    mpz_mul_ui(check, check, l);

    if (mpz_cmp(check, key.p) >= 0) {
        err = CFE_ERR_PRECONDITION_FAILED;
        goto cleanup;
    }

    s->l = l;
    mpz_init_set(s->bound, bound);
    mpz_init_set(s->g, key.g);
    mpz_init_set(s->p, key.p);

    cleanup:
    cfe_elgamal_free(&key);
    mpz_clear(check);
     */
    s->n = n;
    mpz_init_set(s->bound, bound);
    BIG_256_56_rcopy(s->modBig, CURVE_Order_BN254);
    mpz_init(s->mod);
    mpz_from_BIG_256_56(s->mod, s->modBig);

    return err;
}

// msk should be uninitialized!
void cfe_sgp_generate_master_key(cfe_sgp_sec_key *msk, cfe_sgp *sgp) {
    cfe_vec s, t;
    cfe_vec_inits(sgp->n, &s, &t, NULL);
    msk->s = &s;
    msk->t = &t;
    cfe_uniform_sample_vec(msk->s, sgp->bound);
    cfe_uniform_sample_vec(msk->t, sgp->bound);
}

// cfe_extract_submatrix returns a matrix obtained from m by removing row i and column j.
// It returns an error if i >= number of rows of m, or if j >= number of
// columns of m.
cfe_error cfe_extract_submatrix(cfe_mat *m, cfe_mat *min, int i, int j) {
    if (i >= m->rows || j >= m->cols) {
        return CFE_ERR_MALFORMED_INPUT;
    }

    int ind1 = 0;
    mpz_t val;
    mpz_init(val);
    for (int k = 0; k < m->rows - 1; k++) {
        if (k == i) {
            ind1++;
        }
        int ind2 = 0;
        for (int l = 0; l < m->cols - 1; l++) {
            if (l == j) {
               ind2++;
            }
            cfe_mat_get(val, m, ind1, ind2);
            cfe_mat_set(min, val, k, l);
            ind2++;
        }
        ind1++;
    }
    mpz_clear(val);

    return 0;
}

void cfe_determinant(cfe_mat *m, mpz_t det) {
    if (m->rows == 1) {
        cfe_mat_get(det, m, 0, 0);
        return;
    }
    mpz_set_si(det, 0);
    mpz_t sign;
    mpz_init_set_ui(sign, 1);
    mpz_t minus;
    mpz_init_set_si(minus, -1);

    mpz_t val;
    mpz_init(val);
    mpz_t minor;
    mpz_init(minor);
    for (int i = 0; i < m->rows; i++) {
        cfe_mat min;
        cfe_mat_init(&min, m->rows - 1, m->cols - 1);
        cfe_extract_submatrix(m, &min, 0, i);

        cfe_mat_get(val, m, 0, i);

        cfe_determinant(&min, minor);

        mpz_mul(minor, minor, val);
        mpz_mul(minor, minor, sign);
        mpz_mul(sign, sign, minus);
        mpz_add(det, det, minor);
        cfe_mat_free(&min);
    }
    mpz_clears(val, minor, NULL);
}

cfe_error cfe_inverse_mod(cfe_mat *m, cfe_mat *inverse_mat, mpz_t mod) {
    mpz_t det;
    mpz_init(det);
    cfe_determinant(m, det);

    mpz_mod(det, det, mod);

    if(mpz_cmp_si(det, 0) == 0) {
        return CFE_ERR_NO_INVERSE;
    }

    mpz_t det_inv;
    mpz_init(det_inv);
    mpz_invert(det_inv, det, mod);

    mpz_t sign;
    mpz_init(sign);
    mpz_t minus;
    mpz_init_set_si(minus, -1);

    cfe_mat min;
    mpz_t minor;
    mpz_t val;
    cfe_mat_init(&min, m->rows - 1, m->cols - 1);
    mpz_inits(minor, val, NULL);

    cfe_mat transposed;
    cfe_mat_init(&transposed, m->rows, m->cols);
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->cols; j++) {
            cfe_extract_submatrix(m, &min, i, j);
            cfe_determinant(&min, minor);

            mpz_mod(minor, minor, mod);
            mpz_pow_ui(sign, minus, i+j);

            mpz_mul(val, minor, det_inv);
            mpz_mul(val, val, sign);
            mpz_mod(val, val, mod);

            cfe_mat_set(&transposed, val, i, j);
        }
    }
    mpz_clears(sign, minus, minor, val, NULL);
    cfe_mat_transpose(inverse_mat, &transposed);
    cfe_mat_frees(&min, &transposed, NULL);

    return 0;
}

cfe_error cfe_sgp_encrypt(cfe_vec *ciphertext, cfe_sgp *s, cfe_vec *x, cfe_vec *y, cfe_sgp_sec_key *msk) {
    mpz_t gamma;
    mpz_init(gamma);
    cfe_uniform_sample(gamma, s->mod);

    cfe_mat W;
    cfe_mat_init(&W, 2, 2);
    cfe_uniform_sample_mat(&W, s->mod);

    cfe_mat E;
    cfe_mat_init(&E, 3, 3);
    mpz_t foo;
    mpz_init_set_ui(foo, 11);
    cfe_uniform_sample_mat(&E, foo);
    cfe_mat_print(&E);

    mpz_t det;
    mpz_init(det);
    cfe_determinant(&E, det);
    gmp_printf ("tra %Zd\n", det);

    cfe_mat inverse_mat;
    cfe_mat_init(&inverse_mat, E.rows, E.cols);
    //cfe_inverse_mod(&E, &inverse_mat, s->mod);

    mpz_t mod;
    mpz_init_set_ui(mod, 7);
    cfe_inverse_mod(&E, &inverse_mat, mod);

    cfe_mat_print(&inverse_mat);

    cfe_mat check;
    cfe_mat_init(&check, E.rows, E.cols);
    cfe_mat_mul(&check, &E, &inverse_mat);

    cfe_mat check_mod;
    cfe_mat_init(&check_mod, E.rows, E.cols);

    cfe_mat_mod(&check_mod, &check, mod);

    cfe_mat_print(&check_mod);


    return 0;
}

