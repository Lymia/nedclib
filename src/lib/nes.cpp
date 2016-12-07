#include "nedclib2.h"

static char DMCA_data[0x19] = "\0\0DMCA NINTENDO E-READER";

static uint16_t encode_nmi(uint16_t nmi) {
  for(int i=0; i<0x18; i++) {
    for(int j=0; j<8; j++) {
      if(nmi & 0x0001) {
        nmi >>= 1;
        nmi ^= 0x8646;
      } else {
        nmi >>= 1;
      }
    }
    nmi ^= (DMCA_data[0x17-i] << 8);
  }

  return nmi;
}

NEDCLIB_API int nedc_encode_nes(unsigned char *nesdata) {
  if(!is_nes(nesdata)) return 1;
  if(nesdata[4] != 1 || nesdata[5] != 1 || (nesdata[6] & 0xFE) != 0 || nesdata[7] != 0)
    return 2;

  uint16_t nmi = encode_nmi((nesdata[0x3FFB+16] << 8) + (nesdata[0x3FFA+16]));
  nesdata[0x3FFB+16] = (nmi >> 8) & 0xFF;
  nesdata[0x3FFA+16] = nmi & 0xFF;

  if(nesdata[6] & 1) nesdata[0x3FFD + 16] &= 0x7F;
  else               nesdata[0x3FFD + 16] |= 0x80;

  return 0;
}
