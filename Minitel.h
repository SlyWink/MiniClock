#ifndef MINITEL_H
#define MINITEL_H

#include <SoftwareSerial.h>

#define _Agrave(letter) 0x19, 0x41, letter
#define _Aaccute(letter) 0x19, 0x42, letter
#define _Acircum(letter) 0x19, 0x43, letter

#define CURSOR(x,y) 0x1F, (x + 0x40), (y + 0x40)

#define ESC_B1200 0x3A, 0x6B, 0x64
#define ESC_B4800 0x3A, 0x6B, 0x76

#define ESC_SCROLL 0x3A, 0x69, 0x43

#define ESC_BLINK0 0x49
#define ESC_BLINK1 0x48

#define ESC_FG_BLACK   0x40
#define ESC_FG_RED     0x41
#define ESC_FG_GREEN   0x42
#define ESC_FG_YELLOW  0x43
#define ESC_FG_BLUE    0x44
#define ESC_FG_MAGENTA 0x45
#define ESC_FG_CYAN    0x46
#define ESC_FG_WHITE   0x47

#define ESC_BG_BLACK   0x50
#define ESC_BG_RED     0x51
#define ESC_BG_GREEN   0x52
#define ESC_BG_YELLOW  0x53
#define ESC_BG_BLUE    0x54
#define ESC_BG_MAGENTA 0x55
#define ESC_BG_CYAN    0x56
#define ESC_BG_WHITE   0x57


#define ESC_SIZE_1  0x4C
#define ESC_SIZE_W2 0x4D
#define ESC_SIZE_H2 0x4E
#define ESC_SIZE_2  0x4F

#define _SG(l1c1,l1c2,l2c1,l2c2,l3c1,l3c2) (32 + l1c1 + (l1c2<<1) + (l2c1<<2) + (l2c2<<3) + (l3c1<<4) + (l3c2<<6))

#define TEXT_MODE 0x0F
#define GRAPH_MODE 0x0E

#define CLEAR 0x0C

#define ESC_UNDERLINE1 0x5A, 0x20
#define ESC_UNDERLINE0 0x59, 0x20

#define CURSOR_LEFT  0x08
#define CURSOR_RIGHT 0x09
#define CURSOR_DOWN  0x0A
#define CURSOR_UP    0x0B

#define CARRIAGE_RETURN 0x0D

#define CURSOR_0 0x14
#define CURSOR_1 0x11

#define ESC_VID_NORMAL 0x5C
#define ESC_VID_INVERT 0x5D
#define ESC_VID_TRANSP 0x5E

#define ESC_ECHO_OFF 0x3B, 0x60, 0x58, 0x52

#define BELL 0x07

#define ESC_SCREEN_HIDE 0x23, 0x20, 0x58
#define ESC_SCREEN_SHOW 0x23, 0x20, 0x5F

class Minitel: public SoftwareSerial {
  public:

    Minitel(uint8_t receivePin, uint8_t transmitPin) ;

    virtual size_t write(uint8_t b) ;

    virtual int read() ;

    void writeArray(uint8_t *p_bytes, uint8_t p_length) ;

    void write5Bytes(int8_t p_b1, int8_t p_b2=-1, int8_t p_b3=-1, int8_t p_b4=-1, int8_t p_b5=-1) ;

    void writeEscape(int8_t p_b1, int8_t p_b2=-1, int8_t p_b3=-1, int8_t p_b4=-1) ;

  private:

    uint8_t parity7Bits(uint8_t p_byte) ;

} ;

#endif
