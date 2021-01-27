/** Parameter definitions for $(systems[0]["name"]) */
#ifndef __THREEBEARS_PARAMS_H__
#define __THREEBEARS_PARAMS_H__

$set(system=systems[0])

#define VERSION              1
#define MATRIX_SEED_BYTES    $(system["matrix_seed_bytes"])
#define ENC_SEED_BYTES       $(system["es_bytes"])
#define IV_BYTES             $(system["iv"])
#define LGX                  $(system["lgx"])
#define DIGITS               $(system["limbs"])
#define DIM                  $(system["d"])
#define VAR_TIMES_128        $(int(system["variance"]*128))
#define LPR_BITS             4
#define FEC_BITS             $(fec_bits*system["fec"])
#define CCA                  $(system["cca"])
#define SHARED_SECRET_BYTES  $(system["ss_bytes"])
#define PRIVATE_KEY_BYTES    $(system["keygen_seed_bytes"])
#define PRF_KEY_BYTES        PRIVATE_KEY_BYTES

#define BEAR_NAME   "$(system["name"])"
#define encapsulate $(ns[0]+system["name"]+ns[1])_encapsulate
#define decapsulate $(ns[0]+system["name"]+ns[1])_decapsulate
#define keygen      $(ns[0]+system["name"]+ns[1])_keygen
#define get_pubkey  $(ns[0]+system["name"]+ns[1])_get_pubkey

#define GF_BYTES ((LGX*DIGITS+7)/8)
#define PUBLIC_KEY_BYTES (MATRIX_SEED_BYTES + DIM*GF_BYTES)
#define CAPSULE_BYTES \
  (DIM*GF_BYTES + IV_BYTES + ((ENC_SEED_BYTES*8+FEC_BITS)*LPR_BITS+7)/8)
#define Z_BYTES \
  (GF_BYTES + ((ENC_SEED_BYTES*8+FEC_BITS)*LPR_BITS+7)/8)

#endif /*__THREEBEARS_PARAMS_H__*/
