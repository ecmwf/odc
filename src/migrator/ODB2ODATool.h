#ifndef ODB2ODATool_H
#define ODB2ODATool_H

#define SRC __FILE__, __LINE__

namespace odb {
namespace tool {

class ODB2ODATool : public Tool {
public:
	ODB2ODATool (int argc, char *argv[]); 
	ODB2ODATool (const Application *); 
    ODB2ODATool (const CommandLineParser &);

	void run(); 

private:
// No copy allowed
    ODB2ODATool(const ODB2ODATool&);
    ODB2ODATool& operator=(const ODB2ODATool&);
};

} // namespace tool 
} //namespace odb 

#endif 
