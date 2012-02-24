#include "Tracer.h"

Tracer::Tracer(ostream& o, const string& m)
: out_(o), message_(m)
{
	//out_ << message_ << " BEGIN" <<  endl;
	out_ << "BEGIN " <<  message_ << endl;
}

Tracer::~Tracer()
{
	//out_ << message_ << " END" <<  endl;
	out_ << "END " <<  message_ << endl;
}
