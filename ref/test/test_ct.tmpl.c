#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <inttypes.h>
#include <valgrind/memcheck.h>

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
    
int $namespace(test)() {
    uint8_t seeda[KEYGEN_SEED_BYTES];
    uint8_t ska[PRIVATE_KEY_BYTES], skb[ENC_SEED_AND_IV_BYTES];
    uint8_t pka[PUBLIC_KEY_BYTES], pkb[CAPSULE_BYTES];
    uint8_t keya[SHARED_SECRET_BYTES], keyb[SHARED_SECRET_BYTES];
    unsigned int j;
    
    int failures=0;
    memset(ska,0,sizeof(ska));
    memset(skb,0,sizeof(skb));
    
    VALGRIND_MAKE_MEM_UNDEFINED(seeda,sizeof(seeda));
    VALGRIND_MAKE_MEM_UNDEFINED(ska,sizeof(ska));
    VALGRIND_MAKE_MEM_UNDEFINED(skb,sizeof(skb));
    
    uint8_t fail = 0;
    skb[0] ^= 1;

    keygen(pka,ska,seeda);
    encapsulate(keyb,pkb,pka,skb);
    decapsulate(keya,pkb,ska);

    VALGRIND_MAKE_MEM_DEFINED(&fail,sizeof(fail));
    VALGRIND_MAKE_MEM_DEFINED(&keya,sizeof(keya));
    VALGRIND_MAKE_MEM_DEFINED(&keyb,sizeof(keyb));
    
    for (j=0; j<SHARED_SECRET_BYTES; j++) {
        fail |= keya[j] ^ keyb[j];
    }
    if (fail) {
        fprintf(stderr, "$(system['name']): Fail trial #%d!\n",0);
        failures++;
        for (j=0; j<sizeof(keya); j++) printf("%02x",keya[j]);
        printf("\n");
        for (j=0; j<sizeof(keyb); j++) printf("%02x",keyb[j]);
        printf("\n");
    }
    return failures;
}

#undef ENC_SEED_AND_IV_BYTES
#undef PUBLIC_KEY_BYTES     
#undef SHARED_SECRET_BYTES  
#undef CAPSULE_BYTES
#undef KEYGEN_SEED_BYTES
#undef PRIVATE_KEY_BYTES
#undef keygen
#undef encapsulate
#undef decapsulate
#undef VARIANCE       

}//$for(system)

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    int failures = 0, do_fail;

    $for(system in systems) {
        $set(name=system["name"])
        do_fail = test();
        if (!do_fail) printf("$(system['name']): pass.\n");
        failures = failures || do_fail;
    }
    
    return failures;
    
    
}

