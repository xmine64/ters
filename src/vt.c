// Ters Virtual Terminal
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

#include "ters.h"
#include "vt.h"

static int SavedX = 0;
static int SavedY = 0;

int vt_read_escape_sequences(u_char *buffer, int count) {
	if (count <= 1) return 0;
	if (buffer[0] != '[') return 0;

	int parameters[3] = {0, 0, 0};
	int parameters_count = 0;
	int parameter = 0;

	int i=1;
	for (; i < count; i++) {
		if (buffer[i] >= '0' && buffer[i] <= '9') {
			parameter = (parameter*10) + (buffer[i] - '0');
			continue;
		}
		if (buffer[i] == ';') {
			parameters[parameters_count] = parameter;
			parameter = 0;
			parameters_count++;
			continue;
		}
		if (parameter > 0) {
			parameters[parameters_count] = parameter;
			parameter = 0;
			parameters_count++;
		}
		switch (buffer[i]) {
			case 'H': // set cursor position
			case 'f':
				screen_vt_H(parameters[0], parameters[1]);
				break;
			case 'A': // cursor up
				screen_vt_A(parameters[0] == 0? 1 : parameters[0]);
				break;
			case 'B': // cursor down
				screen_vt_B(parameters[0] == 0? 1 : parameters[0]);
				break;
			case 'C': // cursor forward
				screen_vt_C(parameters[0] == 0? 1 : parameters[0]);
				break;
			case 'D': // cursor backward
				screen_vt_D(parameters[0] == 0? 1 : parameters[0]);
				break;
			case 's': // save cursor state
				SavedX = screen_get_x();
				SavedY = screen_get_y();
				break;
			case 'u': // restore saved state
				screen_set_x(SavedX);
				screen_set_y(SavedY);
				break;
			case 'm': // set graphic mode
				// TODO
				break;
			case 'K':
				screen_vt_K();
				break;
			case 'J':
				if (parameters[0] == 2) {
					screen_vt_J();
					break;
				}
			default:
				break;
		}
		break;
	}

	return i;
}

void vt_print_buffer(u_char *buffer, int count) {
	for (int i = 0; i < count; i++) {
		switch (buffer[i]) {
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
			case ESC: {
				int count = vt_read_escape_sequences(buffer + i + 1, count - i - 1);
				if (count > 0) {
					i += count;
					break;
				}
			} // !!falls down!!
			// C0
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
			// C1
			case HOP:
			case BPH:
			case NBH:
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
			case SOS:
			case SGCI:
			case SCI:
			case CSI:
			case ST:
			case OSC:
			case PM:
			case APC:
			default:
				screen_addch(buffer[i]);
				break;
		}
	}

	screen_refresh();
}
