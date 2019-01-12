#include <Minitel.h>
#include <TinyDS1307.h>
#include "minitel-matrix.h"

#define KEY_ENVOI      -1
#define KEY_RETOUR     -2
#define KEY_ANNULATION -3
#define KEY_SOMMAIRE   -4
#define KEY_SUITE      -5

#define IDX_YEAR    2
#define IDX_MONTH   (IDX_YEAR + 3)
#define IDX_DAY     (IDX_MONTH + 3)
#define IDX_HOUR    (IDX_DAY + 3)
#define IDX_MINUTE  (IDX_HOUR + 3)
#define IDX_SECOND  (IDX_MINUTE + 3)
#define IDX_WEEKDAY (IDX_SECOND + 3)
#define IDX_LAST    IDX_WEEKDAY

#define isDigit(c) ((c >= '0') && (c <= '9'))

#define DATE_START CARRIAGE_RETURN, CURSOR_RIGHT, CURSOR_RIGHT

#define WEEKDAY_LENGTH 3

const uint8_t WEEKDAYS[][WEEKDAY_LENGTH] PROGMEM = {
  { '?', '?', '?' }, { 'L', 'U', 'N' }, { 'M', 'A', 'R' }, { 'M', 'E', 'R' },
  { 'J', 'E', 'U' }, { 'V', 'E', 'N' }, { 'S', 'A', 'M' }, { 'D', 'I', 'M' }
} ;

const uint8_t MSG_CANCELED[] PROGMEM = { '*', 'A','n', 'n', 'u', 'l', _Aaccute('e'), ' ', '*' } ;
const uint8_t MSG_MODIFIED[] PROGMEM = { '*', 'M','o', 'd', 'i', 'f', 'i', _Aaccute('e'), ' ', '*' } ;

typedef struct {
  uint8_t year2k ;
  uint8_t month ;
  uint8_t day ;
  uint8_t weekday ;
  uint8_t hour ;
  uint8_t minute ;
  uint8_t second ;
} DATE_T ;

DATE_T g_date ;

Minitel myMinitel(4,3) ;


void int2str1(uint8_t p_num, char *p_str) {
  *p_str = '0' + p_num % 10 ;
}


void int2str2(uint8_t p_num, char *p_str) {
  *p_str = '0' + p_num / 10 ;
  *(p_str + 1) = '0' + p_num % 10 ;
}


uint8_t str2int1(char *p_str) {
  if (isDigit(*p_str)) return (*p_str - '0') ;
  return -1 ;
}


uint8_t str2int2(char *p_str) {
  if ( isDigit(p_str[0]) && isDigit(p_str[1]) )
    return (p_str[0] - '0') * 10 + (p_str[1] - '0') ;
  return -1 ;
}


void writeProgmem(const uint8_t *p_pm, uint8_t p_length) {
  uint8_t l_idx, l_byte ;

  for (l_idx=0 ; l_idx<p_length ; l_idx++) {
    l_byte = pgm_read_byte_near(&p_pm[l_idx]) ;
    if (l_byte != 0xFF) myMinitel.write(l_byte) ;
  }
}


int8_t readFigure(void) {
  int8_t l_byte ;

  for(;;) {
    while ((l_byte = myMinitel.read()) == -1) ;
    if (l_byte == 0x13) {
      while ((l_byte = myMinitel.read()) == -1) ;
      switch(l_byte) {
        case 0x41:
          return KEY_ENVOI ;
        case 0x42:
          return KEY_RETOUR ;
        case 0x45:
          return KEY_ANNULATION ;
        case 0x46:
          return KEY_SOMMAIRE ;
//        case 0x47:
//          return KEY_CORRECTION ;
        case 0x48:
          return KEY_SUITE ;
      }
    } else
     if (isDigit(l_byte)) return l_byte ;
    myMinitel.write(BELL) ;
  }
}


bool valiDate(char *p_strdate, DATE_T *p_date) {
  p_date->year2k = str2int2(&p_strdate[IDX_YEAR]) ;
  p_date->month = str2int2(&p_strdate[IDX_MONTH]) ;
  if ( (p_date->month == 0) || (p_date->month > 12) ) return false ;
//myMinitel.write(BELL) ; delay(500) ;
  p_date->day = str2int2(&p_strdate[IDX_DAY]) ;
  if ( (p_date->day == 0) || (p_date->day > 31) ) return false ;
//myMinitel.write(BELL) ; delay(500) ;
  switch(p_date->month) {
    case 2:
      if ( (p_date->year2k & 1) && (p_date->day > 28) ) return false ;
//myMinitel.write(BELL) ; delay(500) ;
      if (p_date->day > 29) return false ;
//myMinitel.write(BELL) ; delay(500) ;
      break ;
    case 4:
    case 6:
    case 9:
    case 11:
      if (p_date->day > 30) return false ;
//myMinitel.write(BELL) ; delay(500) ;
      break ;
  }
  p_date->weekday = str2int1(&p_strdate[IDX_WEEKDAY]) ;
  if ( (p_date->weekday == 0) || (p_date->weekday > 7) ) return false ;
  p_date->hour = str2int2(&p_strdate[IDX_HOUR]) ;
  if (p_date->hour > 23) return false ;
//myMinitel.write(BELL) ; delay(500) ;
  p_date->minute = str2int2(&p_strdate[IDX_MINUTE]) ;
  if (p_date->minute > 59) return false ;
//myMinitel.write(BELL) ; delay(500) ;
  p_date->second = str2int2(&p_strdate[IDX_SECOND]) ;
  if (p_date->second > 59) return false ;
//myMinitel.write(BELL) ; delay(500) ;
  return true ;
}


bool readDigits(char *p_strdate) {
  uint8_t l_curpos = IDX_YEAR ;
  int8_t l_key ;

  for(;;) {
    l_key=readFigure() ;
    switch(l_key) {
      case KEY_SUITE:
        if (l_curpos < IDX_LAST) {
          myMinitel.write(CURSOR_RIGHT) ; l_curpos++ ;
          if (!isDigit(p_strdate[l_curpos])) {
            myMinitel.write(CURSOR_RIGHT) ; l_curpos++ ;
          }
        }
        break ;
      case KEY_RETOUR:
        if (l_curpos > IDX_YEAR) {
          myMinitel.write(CURSOR_LEFT) ; l_curpos-- ;
          if (!isDigit(p_strdate[l_curpos])) {
            myMinitel.write(CURSOR_LEFT) ; l_curpos-- ;
          }
        }
        break ;
      case KEY_ANNULATION:
        myMinitel.write5Bytes(DATE_START) ; l_curpos = IDX_YEAR ;
        break ;
      case KEY_ENVOI:
        return true ;
      case KEY_SOMMAIRE:
        return false ;
      default:
        if (isDigit(l_key)) {
          myMinitel.write(l_key) ;
          p_strdate[l_curpos] = l_key ;
          if (l_curpos < IDX_LAST) {
            l_curpos++ ;
            if (!isDigit(p_strdate[l_curpos])) {
              myMinitel.write(CURSOR_RIGHT) ; l_curpos++ ;
            }
          } else myMinitel.write(CURSOR_LEFT) ;
        }
      }
  }
}


bool _readDate(DATE_T *p_date) {
  char *l_strdate = "2000/00/00-00:00:00#0" ;

  int2str2(p_date->year2k,&(l_strdate[IDX_YEAR])) ;
  int2str2(p_date->month,&(l_strdate[IDX_MONTH])) ;
  int2str2(p_date->day,&(l_strdate[IDX_DAY])) ;
  int2str1(p_date->weekday,&(l_strdate[IDX_WEEKDAY])) ;
  int2str2(p_date->hour,&(l_strdate[IDX_HOUR])) ;
  int2str2(p_date->minute,&(l_strdate[IDX_MINUTE])) ;
  int2str2(p_date->second,&(l_strdate[IDX_SECOND])) ;
  for(;;) {
    myMinitel.write(CARRIAGE_RETURN) ;
    myMinitel.print(l_strdate) ;
    myMinitel.write5Bytes(DATE_START) ;
    if (readDigits(l_strdate)) {
      if (valiDate(l_strdate,p_date)) return true ;
    } else return false ;
  }
}


bool readDate(DATE_T *p_date) {
  myMinitel.write(CURSOR_1) ;
  bool l_status = _readDate(p_date) ;
  myMinitel.write(CURSOR_0) ;
  return l_status ;
}


void readRtc(DATE_T *p_date) {
  for (uint8_t l_times=0 ; l_times<5 ; l_times++) {
    tinyrtc.read() ;
    if (tinyrtc.day()) break ;
    delay(5) ;
  }
  p_date->year2k = (tinyrtc.year() >= 2000) ? tinyrtc.year()-2000 : 0 ;
  p_date->month = tinyrtc.month() ;
  p_date->day = tinyrtc.day() ;
  p_date->weekday = tinyrtc.dow() ;
  p_date->hour = tinyrtc.hour() ;
  p_date->minute = tinyrtc.minute() ;
  p_date->second = tinyrtc.second() ;
}


uint8_t countOne(DATE_T *p_date) {
  uint8_t l_one = 0 ;

  if (p_date->hour / 10 == 1) l_one++ ;
  if (p_date->hour % 10 == 1) l_one++ ;
  if (p_date->minute / 10 == 1) l_one++ ;
  if (p_date->minute % 10 == 1) l_one++ ;
  if (p_date->second / 10 == 1) l_one++ ;
  if (p_date->second % 10 == 1) l_one++ ;
  return l_one ;
}


void writeTime(uint8_t p_column, uint8_t p_line, DATE_T *p_date) {
  uint8_t l_time[6] ;

  l_time[0] = p_date->hour / 10 ; l_time[1] = p_date->hour % 10 ;
  l_time[2] = p_date->minute /10 ; l_time[3] = p_date->minute % 10 ;
  l_time[4] = p_date->second / 10 ; l_time[5] = p_date->second % 10 ;
  for (uint8_t l_height=0 ; l_height<BIG_DIGIT_HEIGHT ; l_height++) {
    myMinitel.write5Bytes(CURSOR(p_line+l_height,p_column)) ;
    myMinitel.write(GRAPH_MODE) ;
    myMinitel.writeEscape(ESC_UNDERLINE1) ;
    for (uint8_t l_timeidx=0 ; l_timeidx<6 ; l_timeidx++) {
      writeProgmem(BIG_DIGITS[l_time[l_timeidx]][l_height],BIG_DIGIT_WIDTH) ;
      if ((l_timeidx == 1) || (l_timeidx == 3)) {
        writeProgmem(BIG_COLUMN[l_height],2) ;
      } else
        myMinitel.write(_SG(0,0,0,0,0,0)) ; // Space
    }
  }
}


void writeDate(uint8_t p_column, uint8_t p_line, DATE_T *p_date) {
  static char l_temp[] = { '0', '0' } ;
  
  myMinitel.write5Bytes(CURSOR(p_line,p_column)) ;
  myMinitel.writeEscape(ESC_SIZE_2) ;
  myMinitel.writeEscape(ESC_FG_GREEN) ;
  writeProgmem(WEEKDAYS[p_date->weekday],WEEKDAY_LENGTH) ;
  myMinitel.print(" ") ;
  int2str2(p_date->day,l_temp) ; myMinitel.writeArray((uint8_t *)l_temp,2) ;
  myMinitel.print("/") ;
  int2str2(p_date->month,l_temp) ; myMinitel.writeArray((uint8_t *)l_temp,2) ;
  myMinitel.print("/20") ;
  int2str2(p_date->year2k,l_temp) ; myMinitel.writeArray((uint8_t *)l_temp,2) ;
}


void setup() {
  
  delay(4000) ;
  myMinitel.begin(1200) ;
  myMinitel.println("1200 bps ") ;
  myMinitel.writeEscape(ESC_B4800) ;
  myMinitel.begin(4800) ; delay(1000) ;
  myMinitel.println("4800 bps ") ;
  myMinitel.writeEscape(ESC_SCROLL) ;
  myMinitel.flush() ;

  TinyWireM.begin() ;
  tinyrtc.begin() ;
  //if (tinyrtc.isrunning()) readRtc(&g_date) ;
  myMinitel.writeEscape(ESC_ECHO_OFF) ;

  while (myMinitel.read() != -1) ;
  randomSeed(analogRead(1)) ;
}


void loop() {
  static uint8_t l_second = 0 ;
  static uint8_t l_line, l_column ;
  static uint8_t l_leftone ;  

  readRtc(&g_date) ;
  if ( !(g_date.second % 5) && (g_date.second != l_second)  ) {
    l_second = g_date.second ;
    myMinitel.write(CLEAR) ;
//    myMinitel.writeEscape(ESC_SCREEN_HIDE) ;
    l_leftone = countOne(&g_date) ;
    l_line = random(19-3)+1 ; l_column = random(3+l_leftone)+1 ;
    writeTime(l_column,l_line,&g_date) ;
    writeDate(l_column+6-l_leftone/2,l_line+6,&g_date) ;
//    myMinitel.writeEscape(ESC_SCREEN_SHOW) ;
  } else
    if ( (myMinitel.read() == 0x13) && (myMinitel.read() == 0x46) ) {
      myMinitel.write(CLEAR) ;
      myMinitel.println("Saisir date, heure et jour de semaine :") ;
      myMinitel.println() ;
      if (readDate(&g_date)) {
        tinyrtc.adjust(g_date.year2k+2000,g_date.month,g_date.day,g_date.weekday,g_date.hour,g_date.minute,g_date.second) ;
        myMinitel.println() ;
        writeProgmem(MSG_MODIFIED,sizeof(MSG_MODIFIED)) ;
      } else {
        myMinitel.println() ;
        writeProgmem(MSG_CANCELED,sizeof(MSG_CANCELED)) ;
      }
    }
}
