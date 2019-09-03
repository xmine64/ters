// Ters Virtual Terminal
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

#define TAB_SIZE 8

// From ANSI standard, Joe Smith, 18-May-84
// C0 set of 7-bit control characters
#define NUL     0x00
#define SOH     0x01
#define STX     0x02
#define ETX     0x03
#define EOT     0x04
#define ENQ     0x05
#define ACK     0x06
#define BEL     0x07
#define BS      0x08
#define HT      0x09
#define LF      0x0A
#define VT      0x0B
#define FF      0x0C
#define CR      0x0D
#define SO      0x0E
#define SI      0x0F
#define DLE     0x10
#define XON     0x11
#define DC2     0x12
#define XOFF    0x13
#define DC4     0x14
#define NAK     0x15
#define SYN     0x16
#define ETB     0x17
#define CAN     0x18
#define EM      0x19
#define SUB     0x1A
#define ESC     0x1B
#define FS      0x1C
#define GS      0x1D
#define RS      0x1E
#define US      0x1F
// C1 set of 8-bit control characters
#define IND     0x84
#define NEL     0x85
#define SSA     0x86
#define ESA     0x87
#define HTS     0x88
#define HTJ     0x89
#define VTS     0x8A
#define PLD     0x8B
#define PLU     0x8C
#define RI      0x8D
#define SS2     0x8E
#define SS3     0x8F
#define DCS     0x90
#define PU1     0x91
#define PU2     0x92
#define STS     0x93
#define CCH     0x94
#define MW      0x95
#define SPA     0x96
#define EPA     0x97
#define CSI     0x9B
#define ST      0x9C
#define OSC     0x9D
#define PM      0x9E
#define APC     0x9F

/* handlers */
void screen_addch(u_char c);    // print character

void screen_vt_bs();            // do backspace
void screen_vt_htab();          // Horizontal Tab
void screen_vt_lf();            // go to new line
void screen_vt_vtab();          // Vertical Tab
void screen_vt_cr();            // do caridge return