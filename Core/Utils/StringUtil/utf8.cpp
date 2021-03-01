#pragma once
#include "utf8.h"

#pragma warning(push)
#pragma warning(disable:4244)

/**
UTF-8 Conversion functions, 32-bit codes are written as multi word into a 16-bit character strings.
as we are using 16-bit unicode strings which are not unicode encoded.

@date 2005.4.19
@author juhnu

*/

namespace Lunia {
	namespace StringUtil {
		namespace UTF8 {

			uint32 DecodeSingleCharacter(const char** utf8_buffer)
			{
				uint32	uc;
				char	c;

				// Security considerations:
				//
				// If we hit a zero byte, we want to return 0 without stepping
				// the buffer pointer past the 0.
				//
				// If we hit an "overlong sequence"; i.e. a character encoded
				// in a longer multibyte string than is necessary, then we
				// need to discard the character.  This is so attackers can't
				// disguise dangerous characters or character sequences --
				// there is only one valid encoding for each character.
				//
				// If we decode characters { 0xD800 .. 0xDFFF } or { 0xFFFE,
				// 0xFFFF } then we ignore them; they are not valid in UTF-8.

				// This isn't actually an invalid character; it's a valid char that
				// looks like an inverted question mark.
#define INVALID 0x0FFFD

#define FIRST_BYTE(mask, shift)		\
	uc = (c & (mask)) << (shift);

#define NEXT_BYTE(shift)						\
	c = **utf8_buffer;						\
	if (c == 0) return 0; /* end of buffer, do not advance */	\
	if ((c & 0xC0) != 0x80) return INVALID; /* standard check */	\
	(*utf8_buffer)++;						\
	uc |= (c & 0x3F) << shift;

				c = **utf8_buffer;
				if (c == 0) return 0;	// End of buffer.  Do not advance.

				(*utf8_buffer)++;
				if ((c & 0x80) == 0) return (uint32)c;	// Conventional 7-bit ASCII.

				// Multi-byte sequences.
				if ((c & 0xE0) == 0xC0)
				{
					// Two-byte sequence.
					FIRST_BYTE(0x1F, 6);
					NEXT_BYTE(0);
					if (uc < 0x80) return INVALID;	// overlong
					return uc;
				}
				else if ((c & 0xF0) == 0xE0)
				{
					// Three-byte sequence.
					FIRST_BYTE(0x0F, 12);
					NEXT_BYTE(6);
					NEXT_BYTE(0);
					if (uc < 0x800) return INVALID;	// overlong
					if (uc >= 0x0D800 && uc <= 0x0DFFF) return INVALID;	// not valid ISO 10646
					if (uc == 0x0FFFE || uc == 0x0FFFF) return INVALID;	// not valid ISO 10646
					return uc;
				}
				else if ((c & 0xF8) == 0xF0)
				{
					// Four-byte sequence.
					FIRST_BYTE(0x07, 18);
					NEXT_BYTE(12);
					NEXT_BYTE(6);
					NEXT_BYTE(0);
					if (uc < 0x010000) return INVALID;	// overlong
					return uc;
				}
				else if ((c & 0xFC) == 0xF8)
				{
					// Five-byte sequence.
					FIRST_BYTE(0x03, 24);
					NEXT_BYTE(18);
					NEXT_BYTE(12);
					NEXT_BYTE(6);
					NEXT_BYTE(0);
					if (uc < 0x0200000) return INVALID;	// overlong
					return uc;
				}
				else if ((c & 0xFE) == 0xFC)
				{
					// Six-byte sequence.
					FIRST_BYTE(0x01, 30);
					NEXT_BYTE(24);
					NEXT_BYTE(18);
					NEXT_BYTE(12);
					NEXT_BYTE(6);
					NEXT_BYTE(0);
					if (uc < 0x04000000) return INVALID;	// overlong
					return uc;
				}
				else
				{
					// Invalid.
					return INVALID;
				}
			}


			std::wstring Decode(const std::string& str) { return Decode(str.c_str()); }
			std::wstring Decode(const char* buffer) {
				const char** utf8_buffer = &buffer;
				std::wstring result;

				uint32 value[2];
				value[1] = 0;
				do {
					value[0] = DecodeSingleCharacter(utf8_buffer);
					if (value[0] != 0) {
						result += reinterpret_cast<wchar_t*>(value);
					}
				} while (value[0] != 0);
				return result;
			}



			void	EncodeSingleCharacter(char* buffer, int* index, uint32 ucs_character)
			{
				if (ucs_character <= 0x7F)
				{
					// Plain single-byte ASCII.
					buffer[(*index)++] = char(ucs_character);
				}
				else if (ucs_character <= 0x7FF)
				{
					// Two bytes.
					buffer[(*index)++] = char(0xC0 | (ucs_character >> 6));
					buffer[(*index)++] = char(0x80 | ((ucs_character >> 0) & 0x3F));
				}
				else if (ucs_character <= 0xFFFF)
				{
					// Three bytes.
					buffer[(*index)++] = char(0xE0 | (ucs_character >> 12));
					buffer[(*index)++] = char(0x80 | ((ucs_character >> 6) & 0x3F));
					buffer[(*index)++] = char(0x80 | ((ucs_character >> 0) & 0x3F));
				}
				else if (ucs_character <= 0x1FFFFF)
				{
					// Four bytes.
					buffer[(*index)++] = char(0xF0 | (ucs_character >> 18));
					buffer[(*index)++] = char(0x80 | ((ucs_character >> 12) & 0x3F));
					buffer[(*index)++] = char(0x80 | ((ucs_character >> 6) & 0x3F));
					buffer[(*index)++] = char(0x80 | ((ucs_character >> 0) & 0x3F));
				}
				else if (ucs_character <= 0x3FFFFFF)
				{
					// Five bytes.
					buffer[(*index)++] = char(0xF8 | (ucs_character >> 24));
					buffer[(*index)++] = char(0x80 | ((ucs_character >> 18) & 0x3F));
					buffer[(*index)++] = char(0x80 | ((ucs_character >> 12) & 0x3F));
					buffer[(*index)++] = char(0x80 | ((ucs_character >> 6) & 0x3F));
					buffer[(*index)++] = char(0x80 | ((ucs_character >> 0) & 0x3F));
				}
				else if (ucs_character <= 0x7FFFFFFF)
				{
					// Six bytes.
					buffer[(*index)++] = char(0xFC | (ucs_character >> 30));
					buffer[(*index)++] = char(0x80 | ((ucs_character >> 24) & 0x3F));
					buffer[(*index)++] = char(0x80 | ((ucs_character >> 18) & 0x3F));
					buffer[(*index)++] = char(0x80 | ((ucs_character >> 12) & 0x3F));
					buffer[(*index)++] = char(0x80 | ((ucs_character >> 6) & 0x3F));
					buffer[(*index)++] = char(0x80 | ((ucs_character >> 0) & 0x3F));
				}
				else
				{
					// Invalid char; don't encode anything.
				}
			}


			std::string  Encode(const std::wstring& str) { return Encode(str.c_str()); }
			std::string  Encode(const wchar_t* wstr) {

				const wchar_t* in = wstr;
				std::string result;
				// First pass: compute the necessary string length.
				int	bytes_needed = 0;
				char	dummy[10];
				int	offset;
				for (;;)
				{
					uint32	uc = *in++;
					offset = 0;
					EncodeSingleCharacter(dummy, &offset, uc);
					bytes_needed += offset;

					assert(offset <= 6);

					if (uc == 0)
					{
						break;
					}
				}

				// Second pass: transfer the data.
				result.resize(bytes_needed - 1);	// resize() adds 1 for the \0 terminator
				in = wstr;
				char* out = &((result)[0]);
				offset = 0;
				for (;;)
				{
					assert(offset < bytes_needed);

					uint32	uc = *in++;
					EncodeSingleCharacter(out, &offset, uc);

					assert(offset <= bytes_needed);

					if (uc == 0)
					{
						break;
					}
				}

				assert(offset == bytes_needed);
				assert((result)[offset - 1] == 0);
				assert(result.length() == (int)strlen(result.c_str()));

				return result;






			}

		}
	}
}

#pragma warning(pop)

