#include "Decoder.h"
#include "Log.h"
#include "Exceptions.h"
#include "Endian.h"

namespace odb {

Decoder::W Decoder::makeMask(W size)
{
	W mask = 0;
	while(size--) { mask <<= 1; mask |= 1; }
	return mask;
}

void Decoder::printBinary(ostream& ss, W n)
{
	unsigned char *s = reinterpret_cast<unsigned char *>(&n);

	bool oneSeen = false;

	int endianTest = 1;
	if (*reinterpret_cast<char *>(&endianTest))
	for (int i = sizeof(W) - 1; i >= 0; --i)
	{
		unsigned char c = s[i];
		for(unsigned char mask = 1 << 7; mask; mask >>= 1)
		{
			if (c & mask)
			{
				ss << '1';
				oneSeen = true;
			}
			else
			{
				if (oneSeen) ss << '0';
			}
		}
	}
	else
	for (int i = 0; i < sizeof(W); ++i)
	{
		unsigned char c = s[i];
		for(unsigned char mask = 1 << 7; mask; mask >>= 1)
		{
			if (c & mask)
			{
				ss << '1';
				oneSeen = true;
			}
			else
			{
				if (oneSeen) ss << '0';
			}
		}
	}

	if (! oneSeen)
		ss << '0';
}

} // namespace odb

