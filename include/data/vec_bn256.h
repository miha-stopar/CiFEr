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

#ifndef CIFER_VECTOR_BN256_H
#define CIFER_VECTOR_BN256_H

#include <stdbool.h>
#include <gmp.h>

/**
 * \file
 * \ingroup data
 * \brief Vector BN256 struct and operations.
 */

/**
 * Vector of AMCL integers.
 */
typedef struct cfe_vec_bn256 {
    BIG_256_56 *vec; /** A pointer to the first integer */
    size_t size; /** The size of the vector */
} cfe_vec_bn256;

/**
 * Initializes a vector.
 *
 * @param v A pointer to an uninitialized vector
 * @param size The size of the vector
 */
void cfe_vec_bn256_init(cfe_vec_bn256 *v, size_t size);

/**
 * Variadic version of cfe_vec_bn256_init.
 * Initializes a NULL-terminated list of vectors.
 */
void cfe_vec_bn256_inits(size_t size, cfe_vec *v, ...);

/**
 * Frees the memory occupied by the contents of the vector.
 */
void cfe_vec_bn256_free(cfe_vec_bn256 *v);

/**
 * Variadic version of cfe_vec_bn256_free.
 * Frees a NULL-terminated list of vectors.
 */
void cfe_vec_bn256_frees(cfe_vec *v, ...);

#endif
