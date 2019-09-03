// Ters Virtual Terminal
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

#include "ters.h"
#include "vt.h"

void vt_print_buffer(u_char *buffer, int count) {
	for (int i = 0; i < count; i++) {
		switch (buffer[i]) {
			case NUL:
			case SOH:
			case STX:
			case ETX:
			case EOT:
			case ENQ:
			case ACK:
			//case BEL:
			case FF:
			//case CR:
			case SO:
			case SI:
			case DLE:
			case XON:
			case DC2:
			case XOFF:
			case DC4:
			case NAK:
			case SYN:
			case ETB:
			case CAN:
			case EM:
			case SUB:
			case ESC:
			case FS:
			case GS:
			case RS:
			case US:
				screen_printf("\nVT: Unimplemented C0 Control Char `%d`\n\n", buffer[i]);
				break;
			case BS:
				screen_vt_bs();
				break;
			case HT:
				screen_vt_htab();
				break;
			case VT:
				screen_vt_vtab();
				break;
			case BEL:
				screen_beep();
				break;
			case CR:
				screen_vt_cr();
				break;
			case LF:
				screen_vt_lf();
				break;
			case IND:
			case NEL:
			case SSA:
			case ESA:
			case HTS:
			case HTJ:
			case VTS:
			case PLD:
			case PLU:
			case RI:
			case SS2:
			case SS3:
			case DCS:
			case PU1:
			case PU2:
			case STS:
			case CCH:
			case MW:
			case SPA:
			case EPA:
			case CSI:
			case ST:
			case OSC:
			case PM:
			case APC:
				screen_printf("\nVT: Unimplemented C1 Control Char `%d`\n\n", buffer[i]);
				break;
			default:
				screen_addch(buffer[i]);
				break;
		}
	}

	screen_refresh();
}
