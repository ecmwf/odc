#ifndef TRACER_H

#include <ostream>
#include <string>

using namespace std;

class Tracer {
public:
	Tracer(ostream&, const string&);
	~Tracer();
private:
	ostream& out_;
	const string message_;
};

#endif

