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

const COMPOSE_SEQUENCE DefaultComposeKeyEntries[229] = {
	{         'C',                   EQUALS,        L'€', },
	{ CAPITAL 'C',                   EQUALS,        L'€', },
	{         'T',                   'M',           L'™', },
	{ CAPITAL 'T',                   'M',           L'™', },
	{         'T',           CAPITAL 'M',           L'™', },
	{ CAPITAL 'T',           CAPITAL 'M',           L'™', },

	// ¡¢£¤¥¦§©ª«¬®°±²³µ¶¹º»¼½¾¿
	{ CAPITAL '1',           CAPITAL '1',           L'¡', },
	{         'C',                   SLASH,         L'¢', },
	{ CAPITAL 'C',                   SLASH,         L'¢', },
	{         'L',                   MINUS,         L'£', },
	{ CAPITAL 'L',                   MINUS,         L'£', },
	{         'L',                   EQUALS,        L'£', },
	{ CAPITAL 'L',                   EQUALS,        L'£', },
	{         'O',                   'X',           L'¤', },
	{ CAPITAL 'O',                   'X',           L'¤', },
	{         'O',           CAPITAL 'X',           L'¤', },
	{ CAPITAL 'O',           CAPITAL 'X',           L'¤', },
	{         'Y',                   EQUALS,        L'¥', },
	{ CAPITAL 'Y',                   EQUALS,        L'¥', },
	{         PIPE,                  PIPE,          L'¦', },
	{         'S',                   'O',           L'§', },
	{ CAPITAL 'S',                   'O',           L'§', },
	{         'S',           CAPITAL 'O',           L'§', },
	{ CAPITAL 'S',           CAPITAL 'O',           L'§', },
	{         'O',                   'C',           L'©', },
	{ CAPITAL 'O',                   'C',           L'©', },
	{         'O',           CAPITAL 'C',           L'©', },
	{ CAPITAL 'O',           CAPITAL 'C',           L'©', },
	{         'A',                   UNDERSCORE,    L'ª', },
	{         LESSTHAN,              LESSTHAN,      L'«', },
	{         MINUS,                 COMMA,         L'¬', },
	{         'O',                   'R',           L'®', },
	{ CAPITAL 'O',                   'R',           L'®', },
	{         'O',           CAPITAL 'R',           L'®', },
	{ CAPITAL 'O',           CAPITAL 'R',           L'®', },
	{         '0',                   CIRCUMFLEX,    L'°', },
	{         PLUS,                  MINUS,         L'±', },
	{         '2',                   CIRCUMFLEX,    L'²', },
	{         '3',                   CIRCUMFLEX,    L'³', },
	{         'M',                   'U',           L'µ', },
	{ CAPITAL 'M',                   'U',           L'µ', },
	{         'M',           CAPITAL 'U',           L'µ', },
	{ CAPITAL 'M',           CAPITAL 'U',           L'µ', },
	{         'P',           CAPITAL '1',           L'¶', },
	{ CAPITAL 'P',           CAPITAL '1',           L'¶', },
	{         '1',                   CIRCUMFLEX,    L'¹', },
	{         'O',                   UNDERSCORE,    L'º', },
	{         GREATERTHAN,           GREATERTHAN,   L'»', },
	{         '1',                   '4',           L'¼', },
	{         '1',                   '2',           L'½', },
	{         '3',                   '4',           L'¾', },
	{         QUESTIONMARK,          QUESTIONMARK,  L'¿', },

	// ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß
	{ CAPITAL 'A',                   GRAVE,         L'À', },
	{ CAPITAL 'A',                   ACUTE,         L'Á', },
	{ CAPITAL 'A',                   CIRCUMFLEX,    L'Â', },
	{ CAPITAL 'A',                   TILDE,         L'Ã', },
	{ CAPITAL 'A',                   UMLAUT,        L'Ä', },
	{ CAPITAL 'A',                   RINGABOVE,     L'Å', },
	{ CAPITAL 'A',                   'E',           L'Æ', },
	{ CAPITAL 'A',           CAPITAL 'E',           L'Æ', },
	{ CAPITAL 'C',                   CEDILLA,       L'Ç', },
	{ CAPITAL 'E',                   GRAVE,         L'È', },
	{ CAPITAL 'E',                   ACUTE,         L'É', },
	{ CAPITAL 'E',                   CIRCUMFLEX,    L'Ê', },
	{ CAPITAL 'E',                   UMLAUT,        L'Ë', },
	{ CAPITAL 'I',                   GRAVE,         L'Ì', },
	{ CAPITAL 'I',                   ACUTE,         L'Í', },
	{ CAPITAL 'I',                   CIRCUMFLEX,    L'Î', },
	{ CAPITAL 'I',                   UMLAUT,        L'Ï', },
	{ CAPITAL 'D',                   MINUS,         L'Ð', },
	{ CAPITAL 'N',                   TILDE,         L'Ñ', },
	{ CAPITAL 'O',                   GRAVE,         L'Ò', },
	{ CAPITAL 'O',                   ACUTE,         L'Ó', },
	{ CAPITAL 'O',                   CIRCUMFLEX,    L'Ô', },
	{ CAPITAL 'O',                   TILDE,         L'Õ', },
	{ CAPITAL 'O',                   UMLAUT,        L'Ö', },
	{         'X',                   'X',           L'×', },
	{ CAPITAL 'X',                   'X',           L'×', },
	{         'X',           CAPITAL 'X',           L'×', },
	{ CAPITAL 'X',           CAPITAL 'X',           L'×', },
	{ CAPITAL 'O',                   SLASH,         L'Ø', },
	{ CAPITAL 'U',                   GRAVE,         L'Ù', },
	{ CAPITAL 'U',                   ACUTE,         L'Ú', },
	{ CAPITAL 'U',                   CIRCUMFLEX,    L'Û', },
	{ CAPITAL 'U',                   UMLAUT,        L'Ü', },
	{ CAPITAL 'Y',                   ACUTE,         L'Ý', },
	{ CAPITAL 'T',                   'H',           L'Þ', },
	{ CAPITAL 'T',           CAPITAL 'H',           L'Þ', },
	{         'S',                   'S',           L'ß', },
	{ CAPITAL 'S',                   'S',           L'ß', },
	{         'S',           CAPITAL 'S',           L'ß', },
	{ CAPITAL 'S',           CAPITAL 'S',           L'ß', },

	// àáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ
	{         'A',                   GRAVE,         L'à', },
	{         'A',                   ACUTE,         L'á', },
	{         'A',                   CIRCUMFLEX,    L'â', },
	{         'A',                   TILDE,         L'ã', },
	{         'A',                   UMLAUT,        L'ä', },
	{         'A',                   RINGABOVE,     L'å', },
	{         'A',                   'E',           L'æ', },
	{         'A',           CAPITAL 'E',           L'æ', },
	{         'C',                   CEDILLA,       L'ç', },
	{         'E',                   GRAVE,         L'è', },
	{         'E',                   ACUTE,         L'é', },
	{         'E',                   CIRCUMFLEX,    L'ê', },
	{         'E',                   UMLAUT,        L'ë', },
	{         'I',                   GRAVE,         L'ì', },
	{         'I',                   ACUTE,         L'í', },
	{         'I',                   CIRCUMFLEX,    L'î', },
	{         'I',                   UMLAUT,        L'ï', },
	{         'D',                   MINUS,         L'ð', },
	{         'N',                   TILDE,         L'ñ', },
	{         'O',                   GRAVE,         L'ò', },
	{         'O',                   ACUTE,         L'ó', },
	{         'O',                   CIRCUMFLEX,    L'ô', },
	{         'O',                   TILDE,         L'õ', },
	{         'O',                   UMLAUT,        L'ö', },
	{         COLON,                 MINUS,         L'÷', },
	{         'O',                   SLASH,         L'ø', },
	{         'U',                   GRAVE,         L'ù', },
	{         'U',                   ACUTE,         L'ú', },
	{         'U',                   CIRCUMFLEX,    L'û', },
	{         'U',                   UMLAUT,        L'ü', },
	{         'Y',                   ACUTE,         L'ý', },
	{         'T',                   'H',           L'þ', },
	{         'T',           CAPITAL 'H',           L'þ', },
	{         'Y',                   UMLAUT,        L'ÿ', },

	// Extended Latin: U+0100-U+017F
	{ CAPITAL 'A',                   MACRON,        0x0100, },
	{         'A',                   MACRON,        0x0101, },
	// U+0102, U+0103: A with breve
	{ CAPITAL 'A',                   OGONEK,        0x0104, },
	{         'A',                   OGONEK,        0x0105, },
	{ CAPITAL 'C',                   ACUTE,         0x0106, },
	{         'C',                   ACUTE,         0x0107, },
	{ CAPITAL 'C',                   CIRCUMFLEX,    0x0108, },
	{         'C',                   CIRCUMFLEX,    0x0109, },
	{ CAPITAL 'C',                   DOTABOVE,      0x010A, },
	{         'C',                   DOTABOVE,      0x010B, },
	// U+010C, U+010D: C with caron
	// U+010E, U+010F: D with caron, d with apostrophe
	// U+0110, U+0111: D with bar
	{ CAPITAL 'E',                   MACRON,        0x0112, },
	{         'E',                   MACRON,        0x0113, },
	// U+0114, U+0115: E with breve
	{ CAPITAL 'E',                   DOTABOVE,      0x0116, },
	{         'E',                   DOTABOVE,      0x0117, },
	{ CAPITAL 'E',                   OGONEK,        0x0118, },
	{         'E',                   OGONEK,        0x0119, },
	// U+011A, U+011B: E with caron
	{ CAPITAL 'G',                   CIRCUMFLEX,    0x011C, },
	{         'G',                   CIRCUMFLEX,    0x011D, },
	// U+011E, U+011F: G with breve
	{ CAPITAL 'G',                   DOTABOVE,      0x0120, },
	{         'G',                   DOTABOVE,      0x0121, },
	{ CAPITAL 'G',                   CEDILLA,       0x0122, },
	{         'G',                   CEDILLA,       0x0123, },
	{ CAPITAL 'H',                   CIRCUMFLEX,    0x0124, },
	{         'H',                   CIRCUMFLEX,    0x0125, },
	{ CAPITAL 'H',                   STROKE,        0x0126, },
	{         'H',                   STROKE,        0x0127, },
	{ CAPITAL 'I',                   TILDE,         0x0128, },
	{         'I',                   TILDE,         0x0129, },
	{ CAPITAL 'I',                   MACRON,        0x012A, },
	{         'I',                   MACRON,        0x012B, },
	// U+012C, U+012D: I with breve
	{ CAPITAL 'I',                   OGONEK,        0x012E, },
	{         'I',                   OGONEK,        0x012F, },
	{ CAPITAL 'I',                   DOTBELOW,      0x0130, },
	// U+0131: dotless i
	{ CAPITAL 'I',           CAPITAL 'J',           0x0132, },
	{ CAPITAL 'I',                   'J',           0x0132, },
	{         'I',           CAPITAL 'J',           0x0133, },
	{         'I',                   'J',           0x0133, },
	{ CAPITAL 'J',                   CIRCUMFLEX,    0x0134, },
	{         'J',                   CIRCUMFLEX,    0x0135, },
	{ CAPITAL 'K',                   CEDILLA,       0x0136, },
	{         'K',                   CEDILLA,       0x0137, },
	// U+0138: kra
	{ CAPITAL 'L',                   ACUTE,         0x0139, },
	{         'L',                   ACUTE,         0x013A, },
	{ CAPITAL 'L',                   CEDILLA,       0x013B, },
	{         'L',                   CEDILLA,       0x013C, },
	// U+013D, U+013E: L with caron
	{ CAPITAL 'L',                   MIDDLEDOT,     0x013F, },
	{         'L',                   MIDDLEDOT,     0x0140, },
	{ CAPITAL 'L',                   SLASH,         0x0141, },
	{         'L',                   SLASH,         0x0142, },
	{ CAPITAL 'N',                   ACUTE,         0x0143, },
	{         'N',                   ACUTE,         0x0144, },
	{ CAPITAL 'N',                   CEDILLA,       0x0145, },
	{         'N',                   CEDILLA,       0x0146, },
	// U+0147, U+0148: N with caron
	// U+0149: n with apostrophe before
	{ CAPITAL 'N',           CAPITAL 'G',           0x014A, },
	{ CAPITAL 'N',                   'G',           0x014A, },
	{         'N',           CAPITAL 'G',           0x014B, },
	{         'N',                   'G',           0x014B, },
	{ CAPITAL 'O',                   MACRON,        0x014C, },
	{         'O',                   MACRON,        0x014D, },
	// U+014E, U+014F: O with breve
	// U+0150, U+0151: O with double acute
	{ CAPITAL 'O',           CAPITAL 'E',           0x0152, },
	{ CAPITAL 'O',                   'E',           0x0152, },
	{         'O',           CAPITAL 'E',           0x0153, },
	{         'O',                   'E',           0x0153, },
	{ CAPITAL 'R',                   ACUTE,         0x0154, },
	{         'R',                   ACUTE,         0x0155, },
	{ CAPITAL 'R',                   CEDILLA,       0x0156, },
	{         'R',                   CEDILLA,       0x0157, },
	// U+0158, U+0159: R with caron
	{ CAPITAL 'S',                   ACUTE,         0x015A, },
	{         'S',                   ACUTE,         0x015B, },
	{ CAPITAL 'S',                   CIRCUMFLEX,    0x015C, },
	{         'S',                   CIRCUMFLEX,    0x015D, },
	{ CAPITAL 'S',                   CEDILLA,       0x015E, },
	{         'S',                   CEDILLA,       0x015F, },
	// U+0160, U+0161: S with caron
	{ CAPITAL 'T',                   CEDILLA,       0x0162, },
	{         'T',                   CEDILLA,       0x0163, },
	// U+0164, U+0165: T with caron
	{ CAPITAL 'T',                   STROKE,        0x0166, },
	{         'T',                   STROKE,        0x0167, },
	{ CAPITAL 'U',                   TILDE,         0x0168, },
	{         'U',                   TILDE,         0x0169, },
	{ CAPITAL 'U',                   MACRON,        0x016A, },
	{         'U',                   MACRON,        0x016B, },
	// U+016C, U+016D: U with breve
	{ CAPITAL 'U',                   RINGABOVE,     0x016E, },
	{         'U',                   RINGABOVE,     0x016F, },
	// U+0170, U+0171: U with double acute
	{ CAPITAL 'U',                   OGONEK,        0x0172, },
	{         'U',                   OGONEK,        0x0173, },
	{ CAPITAL 'W',                   CIRCUMFLEX,    0x0174, },
	{         'W',                   CIRCUMFLEX,    0x0175, },
	{ CAPITAL 'Y',                   CIRCUMFLEX,    0x0176, },
	{         'Y',                   CIRCUMFLEX,    0x0177, },
	{ CAPITAL 'Y',                   UMLAUT,        0x0178, },
	{ CAPITAL 'Z',                   ACUTE,         0x0179, },
	{         'Z',                   ACUTE,         0x017A, },
	{ CAPITAL 'Z',                   DOTABOVE,      0x017B, },
	{         'Z',                   DOTABOVE,      0x017C, },
	// U+017D, U+017E: Z with caron
	// U+017F: long S

	// Special sequences for combining diacritics
	{ CAPITAL '2',                   GRAVE,         0x0300, },
	{ CAPITAL '2',                   ACUTE,         0x0301, },
	{ CAPITAL '2',                   CIRCUMFLEX,    0x0302, },
	{ CAPITAL '2',                   TILDE,         0x0303, },
	{ CAPITAL '2',                   MACRON,        0x0304, },
	{ CAPITAL '2',                   DOTABOVE,      0x0307, },
	{ CAPITAL '2',                   UMLAUT,        0x0308, },
	{ CAPITAL '2',                   RINGABOVE,     0x030A, },
	{ CAPITAL '2',                   CEDILLA,       0x0327, },
};
