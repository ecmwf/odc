#ifndef ODA2RequestTool_H
#define ODA2RequestTool_H

namespace odb {
namespace tool {

class ODA2RequestTool : public Tool {
	typedef string Value;
	typedef set<Value> Values;

public:
	ODA2RequestTool();
	ODA2RequestTool(int argc, char **argv);
	~ODA2RequestTool();

	static void help(ostream &o);
	static void usage(const string& name, ostream &o);

	virtual void run();

	void readConfig();
	void readConfig(const PathName&);
	void parseConfig(const string&);

	string generateMarsRequest(const PathName& inputFile, bool fast = false);

protected:
	vector<Values>& values() { return values_; }

	void gatherStats(const PathName& inputFile);
	string gatherStatsFast(const PathName& inputFile);

	PathName config();

private:
	map<string, string> columnName2requestKey_;
	vector<Values> values_;
};

} // namespace tool 
} // namespace odb 

#endif

