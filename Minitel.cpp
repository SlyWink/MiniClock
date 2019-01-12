#include <Minitel.h>


Minitel::Minitel(uint8_t receivePin, uint8_t transmitPin) :
  SoftwareSerial(receivePin,transmitPin,false) {
}
 

size_t Minitel::write(uint8_t b) {
  return SoftwareSerial::write(parity7Bits(b)) ;
}


int Minitel::read() {
  int b = SoftwareSerial::read() ;
  if ( (b == -1) || (b != parity7Bits(b)) ) return -1 ;
  return (b & 0x7F) ;
}
 

void Minitel::writeArray(uint8_t *p_bytes, uint8_t p_length) {
  do {
    write(*p_bytes) ;
    p_bytes++ ;
  } while (--p_length) ;
}


void Minitel::write5Bytes(int8_t p_b1, int8_t p_b2=-1, int8_t p_b3=-1, int8_t p_b4=-1, int8_t p_b5=-1) {
  write(p_b1) ;
  if (p_b2 > -1) {
    write(p_b2) ;
    if (p_b3 > -1) {
      write(p_b3) ;
      if (p_b4 > -1) {
        write(p_b4) ;
        if (p_b5 > -1) write(p_b5) ;
      }
    }
  }
}


void Minitel::writeEscape(int8_t p_b1, int8_t p_b2=-1, int8_t p_b3=-1, int8_t p_b4=-1) {
  write5Bytes(0x1B,p_b1,p_b2,p_b3,p_b4) ;
}


uint8_t Minitel::parity7Bits(uint8_t p_byte) {
  uint8_t l_bitsum = 0 ;

  p_byte &= 0x7F ;
  for (uint8_t l_bitnum=0 ; l_bitnum<7 ; l_bitnum++)
    if (p_byte & _BV(l_bitnum)) l_bitsum++ ;
  if (l_bitsum & 1) p_byte |= 0x80 ;
  return p_byte ;
}
