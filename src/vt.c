// Ters Virtual Terminal
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

#include "ters.h"
#include "vt.h"

int vt_read_escape_sequences(u_char *buffer, int count) {
	return 0;
}

void vt_print_buffer(u_char *buffer, int count) {
	for (int i = 0; i < count; i++) {
		switch (buffer[i]) {
			// C0
			case ESC: {
				int count = vt_read_escape_sequences(buffer + i + 1, count - i - 1);
				if (count > 0) {
					i += count;
					break;
				}
			}
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
			//case ESC:
			case FS:
			case GS:
			case RS:
			case US:
			//case SP:
			//case DEL:
				screen_printf("^%c", buffer[i] + 64);
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
			case SP:
				screen_vt_sp();
				break;
			case DEL:
				screen_vt_del();
				break;
			// C1
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
				screen_printf("0x%X", buffer[i]);
				break;
			default:
				screen_addch(buffer[i]);
				break;
		}
	}

	screen_refresh();
}
