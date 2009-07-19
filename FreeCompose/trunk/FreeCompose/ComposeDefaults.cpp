#include "stdafx.h"
#include "ComposeDefaults.h"

#define CAPITAL 0x80000000 |

#define BACKQUOTE            VK_OEM_3
#define TILDE        CAPITAL VK_OEM_3
#define MINUS                VK_OEM_MINUS
#define UNDERSCORE   CAPITAL VK_OEM_MINUS
#define EQUALS               VK_OEM_PLUS
#define PLUS         CAPITAL VK_OEM_PLUS
#define OPENBRACKET          VK_OEM_4
#define OPENBRACE    CAPITAL VK_OEM_4
#define CLOSEBRACKET         VK_OEM_6
#define CLOSEBRACE   CAPITAL VK_OEM_6
#define BACKSLASH            VK_OEM_5
#define PIPE         CAPITAL VK_OEM_5
#define SEMICOLON            VK_OEM_1
#define COLON        CAPITAL VK_OEM_1
#define SINGLEQUOTE          VK_OEM_7
#define DOUBLEQUOTE  CAPITAL VK_OEM_7
#define COMMA                VK_OEM_COMMA
#define LESSTHAN     CAPITAL VK_OEM_COMMA
#define PERIOD               VK_OEM_PERIOD
#define GREATERTHAN  CAPITAL VK_OEM_PERIOD
#define SLASH                VK_OEM_2
#define QUESTIONMARK CAPITAL VK_OEM_2

#define EXCLAMATIONMARK CAPITAL '1'
#define CIRCUMFLEX      CAPITAL '6'
#define ASTERISK        CAPITAL '8'

#define ACUTE      SINGLEQUOTE
#define GRAVE      BACKQUOTE
#define UMLAUT     DOUBLEQUOTE
#define RINGABOVE  ASTERISK
#define CEDILLA    COMMA
#define OGONEK     COMMA
#define MACRON     MINUS
#define STROKE     MINUS
#define DOTABOVE   PERIOD
#define DOTBELOW   PERIOD
#define MIDDLEDOT  PERIOD

const COMPOSE_KEY_ENTRY DefaultComposeKeyEntries[] = {
	{         'C',                   EQUALS,        L'�', true },
	{ CAPITAL 'C',                   EQUALS,        L'�', true },

	// �������������������������
	{ CAPITAL '1',           CAPITAL '1',           L'�', true },
	{         'C',                   SLASH,         L'�', true },
	{ CAPITAL 'C',                   SLASH,         L'�', true },
	{         'L',                   MINUS,         L'�', true },
	{ CAPITAL 'L',                   MINUS,         L'�', true },
	{         'L',                   EQUALS,        L'�', true },
	{ CAPITAL 'L',                   EQUALS,        L'�', true },
	{         'O',                   'X',           L'�', true },
	{ CAPITAL 'O',                   'X',           L'�', true },
	{         'O',           CAPITAL 'X',           L'�', true },
	{ CAPITAL 'O',           CAPITAL 'X',           L'�', true },
	{         'Y',                   EQUALS,        L'�', true },
	{ CAPITAL 'Y',                   EQUALS,        L'�', true },
	{         PIPE,                  PIPE,          L'�', true },
	{         'S',                   'O',           L'�', true },
	{ CAPITAL 'S',                   'O',           L'�', true },
	{         'S',           CAPITAL 'O',           L'�', true },
	{ CAPITAL 'S',           CAPITAL 'O',           L'�', true },
	{         'O',                   'C',           L'�', true },
	{ CAPITAL 'O',                   'C',           L'�', true },
	{         'O',           CAPITAL 'C',           L'�', true },
	{ CAPITAL 'O',           CAPITAL 'C',           L'�', true },
	{         'A',                   UNDERSCORE,    L'�', true },
	{         LESSTHAN,              LESSTHAN,      L'�', true },
	{         MINUS,                 COMMA,         L'�', true },
	{         'O',                   'R',           L'�', true },
	{ CAPITAL 'O',                   'R',           L'�', true },
	{         'O',           CAPITAL 'R',           L'�', true },
	{ CAPITAL 'O',           CAPITAL 'R',           L'�', true },
	{         '0',                   CIRCUMFLEX,    L'�', true },
	{         PLUS,                  MINUS,         L'�', true },
	{         '2',                   CIRCUMFLEX,    L'�', true },
	{         '3',                   CIRCUMFLEX,    L'�', true },
	{         'M',                   'U',           L'�', true },
	{ CAPITAL 'M',                   'U',           L'�', true },
	{         'M',           CAPITAL 'U',           L'�', true },
	{ CAPITAL 'M',           CAPITAL 'U',           L'�', true },
	{         'P',           CAPITAL '1',           L'�', true },
	{ CAPITAL 'P',           CAPITAL '1',           L'�', true },
	{         '1',                   CIRCUMFLEX,    L'�', true },
	{         'O',                   UNDERSCORE,    L'�', true },
	{         GREATERTHAN,           GREATERTHAN,   L'�', true },
	{         '1',                   '4',           L'�', true },
	{         '1',                   '2',           L'�', true },
	{         '3',                   '4',           L'�', true },
	{         QUESTIONMARK,          QUESTIONMARK,  L'�', true },

	// ��������������������������������
	{ CAPITAL 'A',                   GRAVE,         L'�', true },
	{ CAPITAL 'A',                   ACUTE,         L'�', true },
	{ CAPITAL 'A',                   CIRCUMFLEX,    L'�', true },
	{ CAPITAL 'A',                   TILDE,         L'�', true },
	{ CAPITAL 'A',                   UMLAUT,        L'�', true },
	{ CAPITAL 'A',                   RINGABOVE,     L'�', true },
	{ CAPITAL 'A',                   'E',           L'�', true },
	{ CAPITAL 'A',           CAPITAL 'E',           L'�', true },
	{ CAPITAL 'C',                   CEDILLA,       L'�', true },
	{ CAPITAL 'E',                   GRAVE,         L'�', true },
	{ CAPITAL 'E',                   ACUTE,         L'�', true },
	{ CAPITAL 'E',                   CIRCUMFLEX,    L'�', true },
	{ CAPITAL 'E',                   UMLAUT,        L'�', true },
	{ CAPITAL 'I',                   GRAVE,         L'�', true },
	{ CAPITAL 'I',                   ACUTE,         L'�', true },
	{ CAPITAL 'I',                   CIRCUMFLEX,    L'�', true },
	{ CAPITAL 'I',                   UMLAUT,        L'�', true },
	{ CAPITAL 'D',                   MINUS,         L'�', true },
	{ CAPITAL 'N',                   TILDE,         L'�', true },
	{ CAPITAL 'O',                   GRAVE,         L'�', true },
	{ CAPITAL 'O',                   ACUTE,         L'�', true },
	{ CAPITAL 'O',                   CIRCUMFLEX,    L'�', true },
	{ CAPITAL 'O',                   TILDE,         L'�', true },
	{ CAPITAL 'O',                   UMLAUT,        L'�', true },
	{         'X',                   'X',           L'�', true },
	{ CAPITAL 'X',                   'X',           L'�', true },
	{         'X',           CAPITAL 'X',           L'�', true },
	{ CAPITAL 'X',           CAPITAL 'X',           L'�', true },
	{ CAPITAL 'O',                   SLASH,         L'�', true },
	{ CAPITAL 'U',                   GRAVE,         L'�', true },
	{ CAPITAL 'U',                   ACUTE,         L'�', true },
	{ CAPITAL 'U',                   CIRCUMFLEX,    L'�', true },
	{ CAPITAL 'U',                   UMLAUT,        L'�', true },
	{ CAPITAL 'Y',                   ACUTE,         L'�', true },
	{ CAPITAL 'T',                   'H',           L'�', true },
	{ CAPITAL 'T',           CAPITAL 'H',           L'�', true },
	{         'S',                   'S',           L'�', true },
	{ CAPITAL 'S',                   'S',           L'�', true },
	{         'S',           CAPITAL 'S',           L'�', true },
	{ CAPITAL 'S',           CAPITAL 'S',           L'�', true },

	// ��������������������������������
	{         'A',                   GRAVE,         L'�', true },
	{         'A',                   ACUTE,         L'�', true },
	{         'A',                   CIRCUMFLEX,    L'�', true },
	{         'A',                   TILDE,         L'�', true },
	{         'A',                   UMLAUT,        L'�', true },
	{         'A',                   RINGABOVE,     L'�', true },
	{         'A',                   'E',           L'�', true },
	{         'A',           CAPITAL 'E',           L'�', true },
	{         'C',                   CEDILLA,       L'�', true },
	{         'E',                   GRAVE,         L'�', true },
	{         'E',                   ACUTE,         L'�', true },
	{         'E',                   CIRCUMFLEX,    L'�', true },
	{         'E',                   UMLAUT,        L'�', true },
	{         'I',                   GRAVE,         L'�', true },
	{         'I',                   ACUTE,         L'�', true },
	{         'I',                   CIRCUMFLEX,    L'�', true },
	{         'I',                   UMLAUT,        L'�', true },
	{         'D',                   MINUS,         L'�', true },
	{         'N',                   TILDE,         L'�', true },
	{         'O',                   GRAVE,         L'�', true },
	{         'O',                   ACUTE,         L'�', true },
	{         'O',                   CIRCUMFLEX,    L'�', true },
	{         'O',                   TILDE,         L'�', true },
	{         'O',                   UMLAUT,        L'�', true },
	{         COLON,                 MINUS,         L'�', true },
	{         'O',                   SLASH,         L'�', true },
	{         'U',                   GRAVE,         L'�', true },
	{         'U',                   ACUTE,         L'�', true },
	{         'U',                   CIRCUMFLEX,    L'�', true },
	{         'U',                   UMLAUT,        L'�', true },
	{         'Y',                   ACUTE,         L'�', true },
	{         'T',                   'H',           L'�', true },
	{         'T',           CAPITAL 'H',           L'�', true },
	{         'Y',                   UMLAUT,        L'�', true },

	// Extended Latin: U+0100-U+017F
	{ CAPITAL 'A',                   MACRON,        0x0100, true },
	{         'A',                   MACRON,        0x0101, true },
	// U+0102, U+0103: A with breve
	{ CAPITAL 'A',                   OGONEK,        0x0104, true },
	{         'A',                   OGONEK,        0x0105, true },
	{ CAPITAL 'C',                   ACUTE,         0x0106, true },
	{         'C',                   ACUTE,         0x0107, true },
	{ CAPITAL 'C',                   CIRCUMFLEX,    0x0108, true },
	{         'C',                   CIRCUMFLEX,    0x0109, true },
	{ CAPITAL 'C',                   DOTABOVE,      0x010A, true },
	{         'C',                   DOTABOVE,      0x010B, true },
	// U+010C, U+010D: C with caron
	// U+010E, U+010F: D with caron, d with apostrophe
	// U+0110, U+0111: D with bar
	{ CAPITAL 'E',                   MACRON,        0x0112, true },
	{         'E',                   MACRON,        0x0113, true },
	// U+0114, U+0115: E with breve
	{ CAPITAL 'E',                   DOTABOVE,      0x0116, true },
	{         'E',                   DOTABOVE,      0x0117, true },
	{ CAPITAL 'E',                   OGONEK,        0x0118, true },
	{         'E',                   OGONEK,        0x0119, true },
	// U+011A, U+011B: E with caron
	{ CAPITAL 'G',                   CIRCUMFLEX,    0x011C, true },
	{         'G',                   CIRCUMFLEX,    0x011D, true },
	// U+011E, U+011F: G with breve
	{ CAPITAL 'G',                   DOTABOVE,      0x0120, true },
	{         'G',                   DOTABOVE,      0x0121, true },
	{ CAPITAL 'G',                   CEDILLA,       0x0122, true },
	{         'G',                   CEDILLA,       0x0123, true },
	{ CAPITAL 'H',                   CIRCUMFLEX,    0x0124, true },
	{         'H',                   CIRCUMFLEX,    0x0125, true },
	{ CAPITAL 'H',                   STROKE,        0x0126, true },
	{         'H',                   STROKE,        0x0127, true },
	{ CAPITAL 'I',                   TILDE,         0x0128, true },
	{         'I',                   TILDE,         0x0129, true },
	{ CAPITAL 'I',                   MACRON,        0x012A, true },
	{         'I',                   MACRON,        0x012B, true },
	// U+012C, U+012D: I with breve
	{ CAPITAL 'I',                   OGONEK,        0x012E, true },
	{         'I',                   OGONEK,        0x012F, true },
	{ CAPITAL 'I',                   DOTBELOW,      0x0130, true },
	// U+0131: dotless i
	{ CAPITAL 'I',           CAPITAL 'J',           0x0132, true },
	{ CAPITAL 'I',                   'J',           0x0132, true },
	{         'I',           CAPITAL 'J',           0x0133, true },
	{         'I',                   'J',           0x0133, true },
	{ CAPITAL 'J',                   CIRCUMFLEX,    0x0134, true },
	{         'J',                   CIRCUMFLEX,    0x0135, true },
	{ CAPITAL 'K',                   CEDILLA,       0x0136, true },
	{         'K',                   CEDILLA,       0x0137, true },
	// U+0138: kra
	{ CAPITAL 'L',                   ACUTE,         0x0139, true },
	{         'L',                   ACUTE,         0x013A, true },
	{ CAPITAL 'L',                   CEDILLA,       0x013B, true },
	{         'L',                   CEDILLA,       0x013C, true },
	// U+013D, U+013E: L with caron
	{ CAPITAL 'L',                   MIDDLEDOT,     0x013F, true },
	{         'L',                   MIDDLEDOT,     0x0140, true },
	{ CAPITAL 'L',                   SLASH,         0x0141, true },
	{         'L',                   SLASH,         0x0142, true },
	{ CAPITAL 'N',                   ACUTE,         0x0143, true },
	{         'N',                   ACUTE,         0x0144, true },
	{ CAPITAL 'N',                   CEDILLA,       0x0145, true },
	{         'N',                   CEDILLA,       0x0146, true },
	// U+0147, U+0148: N with caron
	// U+0149: n with apostrophe before
	{ CAPITAL 'N',           CAPITAL 'G',           0x014A, true },
	{ CAPITAL 'N',                   'G',           0x014A, true },
	{         'N',           CAPITAL 'G',           0x014B, true },
	{         'N',                   'G',           0x014B, true },
	{ CAPITAL 'O',                   MACRON,        0x014C, true },
	{         'O',                   MACRON,        0x014D, true },
	// U+014E, U+014F: O with breve
	// U+0150, U+0151: O with double acute
	{ CAPITAL 'O',           CAPITAL 'E',           0x0152, true },
	{ CAPITAL 'O',                   'E',           0x0152, true },
	{         'O',           CAPITAL 'E',           0x0153, true },
	{         'O',                   'E',           0x0153, true },
	{ CAPITAL 'R',                   ACUTE,         0x0154, true },
	{         'R',                   ACUTE,         0x0155, true },
	{ CAPITAL 'R',                   CEDILLA,       0x0156, true },
	{         'R',                   CEDILLA,       0x0157, true },
	// U+0158, U+0159: R with caron
	{ CAPITAL 'S',                   ACUTE,         0x015A, true },
	{         'S',                   ACUTE,         0x015B, true },
	{ CAPITAL 'S',                   CIRCUMFLEX,    0x015C, true },
	{         'S',                   CIRCUMFLEX,    0x015D, true },
	{ CAPITAL 'S',                   CEDILLA,       0x015E, true },
	{         'S',                   CEDILLA,       0x015F, true },
	// U+0160, U+0161: S with caron
	{ CAPITAL 'T',                   CEDILLA,       0x0162, true },
	{         'T',                   CEDILLA,       0x0163, true },
	// U+0164, U+0165: T with caron
	{ CAPITAL 'T',                   STROKE,        0x0166, true },
	{         'T',                   STROKE,        0x0167, true },
	{ CAPITAL 'U',                   TILDE,         0x0168, true },
	{         'U',                   TILDE,         0x0169, true },
	{ CAPITAL 'U',                   MACRON,        0x016A, true },
	{         'U',                   MACRON,        0x016B, true },
	// U+016C, U+016D: U with breve
	{ CAPITAL 'U',                   RINGABOVE,     0x016E, true },
	{         'U',                   RINGABOVE,     0x016F, true },
	// U+0170, U+0171: U with double acute
	{ CAPITAL 'U',                   OGONEK,        0x0172, true },
	{         'U',                   OGONEK,        0x0173, true },
	{ CAPITAL 'W',                   CIRCUMFLEX,    0x0174, true },
	{         'W',                   CIRCUMFLEX,    0x0175, true },
	{ CAPITAL 'Y',                   CIRCUMFLEX,    0x0176, true },
	{         'Y',                   CIRCUMFLEX,    0x0177, true },
	{ CAPITAL 'Y',                   UMLAUT,        0x0178, true },
	{ CAPITAL 'Z',                   ACUTE,         0x0179, true },
	{         'Z',                   ACUTE,         0x017A, true },
	{ CAPITAL 'Z',                   DOTABOVE,      0x017B, true },
	{         'Z',                   DOTABOVE,      0x017C, true },
	// U+017D, U+017E: Z with caron
	// U+017F: long S

	// Special sequences for combining diacritics
	{ CAPITAL '2',                   GRAVE,         0x0300, true },
	{ CAPITAL '2',                   ACUTE,         0x0301, true },
	{ CAPITAL '2',                   CIRCUMFLEX,    0x0302, true },
	{ CAPITAL '2',                   TILDE,         0x0303, true },
	{ CAPITAL '2',                   MACRON,        0x0304, true },
	{ CAPITAL '2',                   DOTABOVE,      0x0307, true },
	{ CAPITAL '2',                   UMLAUT,        0x0308, true },
	{ CAPITAL '2',                   RINGABOVE,     0x030A, true },
	{ CAPITAL '2',                   CEDILLA,       0x0327, true },

	// End of list
	{ 0, 0, 0, false }
};
