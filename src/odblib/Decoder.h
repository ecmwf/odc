// File Decoder.h
// Baudouin Raoult - ECMWF Dec 03

#ifndef Decoder_H
#define Decoder_H

#ifndef machine_H
#include "machine.h"
#endif

namespace odb {

class Decoder {
public:
	typedef long long W;

	static W makeMask(W);
	static void printBinary(ostream&, W);
};

} // namespace odb 

#endif
