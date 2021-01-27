/** Ring arithmetic header */
#ifndef __THREEBEARS_RING_H__
#define __THREEBEARS_RING_H__ 1

#include "common.h"
#include "params.h"

#if defined(WORD_BITS)
#elif (defined(__ILP64__) || defined(__amd64__) || defined(__x86_64__) || defined(__aarch64__))
#define WORD_BITS 64
#else
#define WORD_BITS 32
#endif

$set(bits=systems[0]["limbs"]*systems[0]["lgx"])
$for(wbits in [16,32,64]){
#$("el"if wbits>16 else "")if WORD_BITS == $(wbits)
typedef uint$(wbits)_t limb_t;
typedef int$(wbits)_t slimb_t;
typedef $("__" if wbits>=64 else "")uint$(2*wbits)_t dlimb_t;
typedef $("__" if wbits>=64 else "")int$(2*wbits)_t dslimb_t;
#define LBITS $([x for x in range(1,wbits)
    if bits%(4*x)==0
    and 4*(bits/x) < 2**(2*(wbits-x))
][-1])}
#endif

#define NLIMBS (LGX*DIGITS/LBITS)
#define LMASK  (((limb_t)1<<LBITS)-1)

typedef limb_t gf_t[NLIMBS];

#define GF_BYTES ((LGX*DIGITS+7)/8)
#define contract contract_$(bits)
#define expand   expand_$(bits)
#define mac      mac_$(bits)
#define canon    canon_$(bits)
#define modulus  modulus_$(bits)

/* Serialize a gf_t */
void WEAK contract(uint8_t ch[GF_BYTES], gf_t ll);

/* Deserialize a gf_t */
void WEAK expand(gf_t ll, const uint8_t ch[GF_BYTES]);

/* Multiply and accumulate c = c + a*b */
void WEAK mac(gf_t c, const gf_t a, const gf_t b);

/* Reduce ring element to canonical form */
void WEAK canon(gf_t c);

/** Return the i'th limb of the modulus */
static inline limb_t modulus(int i) {
    return (i==NLIMBS/2) ? LMASK-1 : LMASK;
}


#endif /*__THREEBEARS_RING_H__*/
