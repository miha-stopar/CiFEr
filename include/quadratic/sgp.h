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

#ifndef CIFER_SGP_H
#define CIFER_SGP_H

#include "data/vec.h"
#include "internal/errors.h"

/**
 * \file
 * \ingroup simple
 * \brief SGP scheme.
 */

/**
 * cfe_sgp represents a scheme for quadratic multi-variate polynomials
// based on  Sans, Gay and Pointcheval:
// "Reading in the Dark: Classifying Encrypted Digits with
// Functional Encryption".
 */
typedef struct cfe_sgp {
    size_t n;
    mpz_t bound;
} cfe_sgp;

cfe_error cfe_sgp_init(cfe_sgp *s, size_t n, mpz_t bound);

/**
 * cfe_sgp_sec_key represents a secret key for SGP scheme.
 */
typedef struct cfe_sgp_sec_key {
    cfe_vec *s;
    cfe_vec *t;
} cfe_sgp_sec_key;

void cfe_sgp_generate_master_key(cfe_sgp_sec_key *msk, cfe_sgp *s);

#endif
