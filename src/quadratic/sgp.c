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

