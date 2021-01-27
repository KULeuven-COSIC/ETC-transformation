/** Public header */
#ifndef __THREE_BEARS_$(systems[0]["name"].upper())_H__
#define __THREE_BEARS_$(systems[0]["name"].upper())_H__
$set(system=systems[0])
$set(name=system["name"])

#include <stdint.h>
#include <stddef.h> /* for size_t */
#include <string.h> /* for memcpy */

#define $namespace(KEYGEN_SEED_BYTES) $(system["keygen_seed_bytes"])
#define $namespace(PRIVATE_KEY_BYTES) KEYGEN_SEED_BYTES
#define $namespace(SHARED_SECRET_BYTES) $(system["ss_bytes"])
#define $namespace(ENC_SEED_AND_IV_BYTES) $(system["es_bytes"]+system["iv"])
#define $namespace(PUBLIC_KEY_BYTES) $(system["pk_bytes"])
#define $namespace(CAPSULE_BYTES) $(system["capsule_bytes"])

/**
 * Expand a secret seed to a public key.
 *
 * @param[out] pk The public key.
 * @param[in] sk The private seed, which must be uniformly random.
 */
void $namespace(get_pubkey) (
    uint8_t pk[PUBLIC_KEY_BYTES],
    const uint8_t seed[PRIVATE_KEY_BYTES]
);

/**
 * Expand a secret seed to a public/private keypair.
 *
 * @param[out] pk The public key.
 * @param[out] sk The private key.
 * @param[in] seed The private seed, which must be uniformly random.
 */
static inline void $namespace(keygen) (
    uint8_t pk[PUBLIC_KEY_BYTES],
    uint8_t sk[PRIVATE_KEY_BYTES],
    const uint8_t seed[KEYGEN_SEED_BYTES]
) {
    get_pubkey(pk,seed);
    memcpy(sk,seed,KEYGEN_SEED_BYTES);
}
    
/**
 * Create a shared secret using a random seed and another party's public key.
 *
 * Input and output parameters may not alias.
 *
 * @param[out] shared_secret The shared secret key.
 * @param[out] capsule A ciphertext to send to the other party.
 * @param[in] pk The other party's public key.
 * @param[in] seed A random seed.
 */
void $namespace(encapsulate) (
    uint8_t shared_secret[SHARED_SECRET_BYTES],
    uint8_t capsule[CAPSULE_BYTES],
    const uint8_t pk[PUBLIC_KEY_BYTES],
    const uint8_t seed[ENC_SEED_AND_IV_BYTES]
);

/**
 * Extract the shared secret from a capsule using the private key.
 * Has a negligible but nonzero probability of failure.
 *
 * Input and output parameters may not alias.
 *
 * @param[out] shared_secret The shared secret.
 * @param[in] capsule The capsule produced by encapsulate_cca2.
 * @param[in] sk The private key.
 */
void $namespace(decapsulate) ( 
    uint8_t shared_secret[SHARED_SECRET_BYTES],
    const uint8_t capsule[CAPSULE_BYTES],
    const uint8_t sk[PRIVATE_KEY_BYTES]
);

void secure_bzero (void *s,size_t size);

#endif /*__THREE_BEARS_$(systems[0]["name"].upper())_H__*/
