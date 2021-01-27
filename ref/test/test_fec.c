#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "melas_fec.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    unsigned i,j,k,fail,fl;
    uint8_t supported_fec_lens[] = {24,32,40};
    
    for (fl=0; fl<sizeof(supported_fec_lens); fl++) {
        unsigned len = supported_fec_lens[fl],len2=len+MELAS_FEC_BYTES,nfails=0,ntrials=0;
        uint8_t data[len2], data0[len2];
        
        for (i=0; i<len2*8; i++) {
            for (j=0; j<=i; j++) {
                for (k=0; k<len2; k++) {
                    data[k] = data0[k] = rand();
                }
                melas_fec_set(&data[len],data,len);
                ntrials++;
                data[i/8] ^= 1<<(i%8);
                if (j<i) data[j/8] ^= 1<<(j%8);
                melas_fec_correct(data,len,&data[len]);
            
                fail = 0;
                for (k=0; k<len; k++) {
                    if (data[k] != data0[k]) {
                        fail=1;
                    }
                }
                nfails += fail;
                if (fail) {
                    fprintf(stderr,"Fail %d %d!\n  data =", i,j);
                    for (k=0; k<len; k++) {
                        fprintf(stderr," %02x",data[k]^data0[k]);
                        data[k] = 0;
                    }
                    fprintf(stderr,"\n");
                }
            }
        }
        fprintf(stderr,"Length %d: pass %d/%d trials\n", len,ntrials-nfails,ntrials);
    }
    
    return 0;
}
