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

#include <gmp.h>
#include "test.h"
#include "quadratic/sgp.h"
#include "sample/uniform.h"

#include <big_256_56.h>

MunitResult test_sgp_end_to_end(const MunitParameter *params, void *data) {
    /*
    size_t l = 3;
    size_t modulus_len = 128;

    mpz_t bound, bound_neg, func_key, xy_check, xy;
    mpz_inits(bound, bound_neg, xy_check, NULL);
    mpz_set_ui(bound, 2);
    mpz_pow_ui(bound, bound, 15);
    mpz_neg(bound_neg, bound);

    cfe_ddh s, encryptor, decryptor;
    cfe_error err = cfe_ddh_init(&s, l, modulus_len, bound);
    munit_assert(err == 0);

    cfe_vec msk, mpk, ciphertext, x, y;
    cfe_vec_inits(l, &x, &y, NULL);
    cfe_uniform_sample_range_vec(&x, bound_neg, bound);
    cfe_uniform_sample_range_vec(&y, bound_neg, bound);
    cfe_vec_dot(xy_check, &x, &y);

    cfe_ddh_generate_master_keys(&msk, &mpk, &s);

    err = cfe_ddh_derive_key(func_key, &s, &msk, &y);
    munit_assert(err == 0);

    cfe_ddh_copy(&encryptor, &s);
    err = cfe_ddh_encrypt(&ciphertext, &encryptor, &x, &mpk);
    munit_assert(err == 0);

    cfe_ddh_copy(&decryptor, &s);
    err = cfe_ddh_decrypt(xy, &decryptor, &ciphertext, func_key, &y);
    munit_assert(err == 0);

    munit_assert(mpz_cmp(xy, xy_check) == 0);

    mpz_clears(bound, bound_neg, func_key, xy_check, xy, NULL);
    cfe_vec_frees(&x, &y, &msk, &mpk, &ciphertext, NULL);

    cfe_ddh_free(&s);
    cfe_ddh_free(&encryor);
    cfe_ddh_free(&decryptor);
     */
    size_t n = 2;

    mpz_t b;
    mpz_init_set_ui(b, 10);
    cfe_mat f;
    cfe_mat_init(&f, n, n);
    cfe_uniform_sample_mat(&f, b);
    //cfe_mat_free(&f);

    int err = 1;

    cfe_sgp s;
    err = cfe_sgp_init(&s, n, b);

    cfe_sgp_sec_key msk;


    cfe_sgp_generate_master_key(&msk, &s);

    cfe_vec_print(msk.s);

    cfe_vec x, y;
    cfe_vec_inits(s.n, &x, &y, NULL);

    cfe_uniform_sample_vec(&x, s.bound);
    cfe_uniform_sample_vec(&y, s.bound);

    cfe_sgp_cipher ciphertext;
    cfe_sgp_cipher_init(&ciphertext, &s);
    cfe_sgp_encrypt(&ciphertext, &s, &x, &y, &msk);

    ECP2_BN254 key;
    cfe_sgp_derive_key(&key, &msk, &f);

    cfe_sgp_decrypt(&ciphertext, &key, &f);

    mpz_t(xy);
    mpz_init(xy);
    cfe_vec_dot(xy, &x, &y);

    gmp_printf ("xy %Zd\n", xy);

    ECP_BN254 g1;
    ECP_BN254_generator(&g1);
    ECP2_BN254 g2;
    ECP2_BN254_generator(&g2);

    FP12_BN254 gt, r;
    PAIR_BN254_ate(&gt, &g1, &g2);
    PAIR_BN254_fexp(&gt);

    BIG_256_56 xy_b;
    BIG_256_56_from_mpz(xy_b, xy);

    FP12_BN254_pow(&r, &gt, xy_b);
    FP12_BN254_output(&r);

    munit_assert(err == 0);


    return MUNIT_OK;
}

MunitTest simple_sgp_tests[] = {
        {(char *) "/end-to-end", test_sgp_end_to_end, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
        {NULL, NULL,                                  NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}
};

MunitSuite sgp_suite = {
        (char *) "/quadratic/sgp", simple_sgp_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

