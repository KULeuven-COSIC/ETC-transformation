#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#ifndef __has_builtin
  #define __has_builtin(x) 0
#endif

#if defined(__has_builtin) && __has_builtin(__builtin_readcyclecounter) && USE_CYCLES
#define USING_CYCLE_COUNTER 1
#define TIME_UNIT "cy"
static double now() {
    return __builtin_readcyclecounter() * 0.001;
}
#else
#define USING_CYCLE_COUNTER 0
#if defined TIME_FACTOR
#define TIME_UNIT "kcy"
#else
#define TIME_UNIT "us"
#endif
static double now() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * (uint64_t)1000000 + tv.tv_usec;
}
#endif

#ifndef TIME_FACTOR
#define TIME_FACTOR 1
#endif

static inline int min(int x, int y) { return (x<y) ? x : y; }

#define XSIZE 9
#define XPAD 123
#define XSIZE_MAX 100000
uint8_t *x_alloc(size_t size) {
    uint8_t* ret = (uint8_t *)malloc(size+XSIZE+XPAD);
    for (size_t i=0; i<size+XSIZE+XPAD; i++) ret[i] = (uint8_t)i;
    memcpy(ret,&size,sizeof(size));
    return ret + XSIZE;
}
void x_free(uint8_t *x) {
    free(x-XSIZE);
}
void x_check(uint8_t *x,const char *name) {
    x -= XSIZE;
    size_t size;
    memcpy(&size,x,sizeof(size));
    assert(size<XSIZE_MAX);
    for (size_t i=0; i<size+XSIZE+XPAD; i++) {
        if (i<sizeof(size_t)) continue;
        if (i>=XSIZE && i<XSIZE+size) continue;
        if (x[i] != (uint8_t)i)
            fprintf(stderr,"CHECK FAIL %s %d %d != %d\n",name,(int)i-XSIZE,x[i],(uint8_t)i);
    }
}
    
$for(system in systems) {
$set(name=system["name"])
    
#include "$(name)/threebears.h"
#define ENC_SEED_AND_IV_BYTES $(name.upper()+"_ENC_SEED_AND_IV_BYTES")
#define PUBLIC_KEY_BYTES      $(name.upper()+"_PUBLIC_KEY_BYTES")
#define KEYGEN_SEED_BYTES     $(name.upper()+"_KEYGEN_SEED_BYTES")
#define PRIVATE_KEY_BYTES     $(name.upper()+"_PRIVATE_KEY_BYTES")
#define SHARED_SECRET_BYTES   $(name.upper()+"_SHARED_SECRET_BYTES")
#define CAPSULE_BYTES         $(name.upper()+"_CAPSULE_BYTES")
#define keygen     $(name)_keygen
#define encapsulate $(name)_encapsulate
#define decapsulate $(name)_decapsulate
#define VARIANCE $(system["variance"])
#define CCA      $(system["cca"])
    
int $namespace(test)(int verbose, unsigned int ntrials) {
    uint8_t *seeda=x_alloc(KEYGEN_SEED_BYTES);
    uint8_t *ska=x_alloc(PRIVATE_KEY_BYTES);
    uint8_t *skb=x_alloc(KEYGEN_SEED_BYTES);
    uint8_t *pka=x_alloc(PUBLIC_KEY_BYTES);
    uint8_t *pkb=x_alloc(CAPSULE_BYTES);
    uint8_t *keya=x_alloc(SHARED_SECRET_BYTES);
    uint8_t *keyb=x_alloc(SHARED_SECRET_BYTES);
    unsigned int i,j;

    
    double t;
    int successes = 0, failures=0;
    memset(seeda,0,KEYGEN_SEED_BYTES);
    memset(skb,0,KEYGEN_SEED_BYTES);
    for (i=0; i<ntrials; i++) {
        uint8_t fail = 0;
        skb[0] ^= 1;
    
        keygen(pka,ska,seeda);
        encapsulate(keyb,pkb,pka,skb);
        
        int shouldfail = !!(CCA && (i&1));
        if (shouldfail) {
            pkb[(i/8) % PUBLIC_KEY_BYTES] ^= 1<<(i%8);
        }
        decapsulate(keya,pkb,ska);
        fail = 0;        
        for (j=0; j<SHARED_SECRET_BYTES; j++) {
            fail |= keya[j] ^ keyb[j];
        }
        fail = !!fail;
        if (fail != shouldfail) {
            fprintf(stderr, "Fail %s trial #%d!\n",shouldfail?"negative":"positive",i);
            failures++;
        } else {
            successes++;
        }
        
        memcpy(seeda,keya,min(KEYGEN_SEED_BYTES,SHARED_SECRET_BYTES));
        memcpy(skb,keyb,min(KEYGEN_SEED_BYTES,SHARED_SECRET_BYTES));
    }
    
    if (verbose) {
        unsigned a=0,b=1;
        while (VARIANCE*b != (a=(int)(VARIANCE*b))) { b*=2; }
        printf("%s: q = 2^%d, n = %d*%d, variance = %d/%d = %f\n","$(system["name"])",$(system["lgx"]),$(system["d"]),$(system["limbs"]),a,b,VARIANCE);
        printf("Size: Private key %dB, Public key %dB, Capsule %dB\n", KEYGEN_SEED_BYTES, PUBLIC_KEY_BYTES, CAPSULE_BYTES);
        printf("Pass %d/%d trial pairs (failure rate = %f)\n", successes, ntrials, ((double)(i-successes))/i);
    }


    /* Performance */
    printf("%27s","\\textsc{$(system["name"])}"); fflush(stdout);

    t=now();
    for (i=0; i<ntrials; i++) {
        keygen(pka,ska,seeda);
    }
    t=now()-t;
    if (verbose) {
        printf("Public key:  %"PRId64" %s\n", (uint64_t)(t*TIME_FACTOR+i/2)/i, TIME_UNIT);
    } else {
        printf(" & %4dk",(int)(t*TIME_FACTOR+i/2)/i); fflush(stdout);
    }
    
    t = now();
    for (i=0; i<ntrials; i++) {
        encapsulate(keyb,pkb,pka,skb);
    }
    t = now()-t;
    if (verbose) {
        printf("Encapsulate: %"PRId64" %s\n", (uint64_t)(t*TIME_FACTOR+i/2)/i, TIME_UNIT);
    } else {
        printf(" & %4dk",(int)(t*TIME_FACTOR+i/2)/i); fflush(stdout);
    }
    
    t = now();
    for (i=0; i<ntrials; i++) {
        decapsulate(keya,pkb,ska);
    }
    t = now()-t;
    if (verbose) {
        printf("Decapsulate: %"PRId64" %s\n", (uint64_t)(t*TIME_FACTOR+i/2)/i, TIME_UNIT);
    } else {
        printf(" & %4dk",(int)(t*TIME_FACTOR+i/2)/i); fflush(stdout);
    }

#if CCA
    for (i=0; i<CAPSULE_BYTES; i++) {
        pkb[i] += (uint8_t)i; // make invalid
    }
    t = now();
    for (i=0; i<ntrials; i++) {
        decapsulate(keya,pkb,ska);
    }
    t = now()-t;
    if (verbose) {
        printf("Decaps_fail: %"PRId64" %s\n", (uint64_t)(t*TIME_FACTOR+i/2)/i, TIME_UNIT);
    } else {
        printf(" & %4dk",(int)(t*TIME_FACTOR+i/2)/i); fflush(stdout);
    }
#endif
    
    if (verbose >= 3) {
        memset(seeda,0,KEYGEN_SEED_BYTES);
        memset(skb,0,KEYGEN_SEED_BYTES);
        skb[0] ^= 1;
    
        keygen(pka,ska,seeda);
        encapsulate(keyb,pkb,pka,skb);
        decapsulate(keya,pkb,ska);
        
        printf("Alice's public key:\n");
        for (i=0; i<PUBLIC_KEY_BYTES; i++) printf("%02x",pka[i]);
        
        printf("\n\nAlice's private key:\n");
        for (i=0; i<KEYGEN_SEED_BYTES; i++) printf("%02x",ska[i]);
        
        printf("\n\nBob's capsule:\n");
        for (i=0; i<CAPSULE_BYTES; i++) printf("%02x",pkb[i]);
        
        printf("\n\nShared secret:\n");
        for (i=0; i<SHARED_SECRET_BYTES; i++) printf("%02x",keyb[i]);
        printf("\n");
    } else if (verbose >= 2) {
        printf("   mc=");
        for (i=0; i<KEYGEN_SEED_BYTES; i++) printf("%02x",seeda[i]);
        printf("\n");
    }
    
    if (verbose) {
        printf("\n");
    } else {
        printf(" \\\\\n");
    }
    


    x_check(seeda ,"seed");
    x_check(ska ,"ska ");
    x_check(skb ,"skb ");
    x_check(pka ,"pka ");
    x_check(pkb ,"pkb ");
    x_check(keya,"keya");
    x_check(keyb,"keyb");

    x_free(seeda);
    x_free(ska);
    x_free(skb);
    x_free(pka);
    x_free(pkb);
    x_free(keya);
    x_free(keyb);
    
    return failures;
}

#undef ENC_SEED_AND_IV_BYTES
#undef PUBLIC_KEY_BYTES     
#undef SHARED_SECRET_BYTES  
#undef CAPSULE_BYTES
#undef PRIVATE_KEY_BYTES
#undef KEYGEN_SEED_BYTES
#undef keygen
#undef encapsulate
#undef decapsulate
#undef VARIANCE     
#undef CCA       

}//$for(system)

int main(int argc, char **argv) {
    int failures = 0, do_fail, i, verbose = 1, ntrials = 1000000;
    
    for (i=1; i<argc; i++) {
        if (!strcmp(argv[i],"-q")) {
            verbose--;
        }
        if (!strcmp(argv[i],"-v")) {
            verbose++;
        }
        if (!strcmp(argv[i],"-n") && i<argc-1) {
            ntrials=atoi(argv[++i]);
        }
        if (ntrials < 0) return 1;
    }
    
    if (verbose == 0) {
#if USING_CYCLE_COUNTER
        printf("Timings are in kcycles from cycle counter (multiplied by %5.3f), averaged over %d trials\n",
            (double)TIME_FACTOR,ntrials);
#else
        if (strcmp(TIME_UNIT,"us")) {
            printf("Timings are in kcycles assuming CPU speed is %5.3f GHz, averaged over %d trials\n",
            (double)TIME_FACTOR,ntrials);
        } else {
            printf("Timings are in microseconds (=kilo-nanoseconds), averaged over %d trials\n", ntrials);
        }
#endif
    }
    

    $for(system in systems) {
        $set(name=system["name"])
        do_fail = test(verbose,ntrials);
        failures = failures || do_fail;
    }
    
    return failures;
    
    
}

