///
/// \file TemplateParameter.h
///
/// @author Piotr Kuchta, June 2009

#ifndef TemplateParameters_H
#define TemplateParameters_H

class PathName;
class DataHandle;

namespace odb {

class DispatchingWriter;
class HashTable;
class SQLIteratorSession;

struct TemplateParameter {
	TemplateParameter(size_t startPos, size_t endPos, size_t columnIndex, string name)
	: startPos(startPos), endPos(endPos), columnIndex(columnIndex), name(name)
	{}

	size_t startPos;
	size_t endPos;
	size_t columnIndex;
	string name;

private:
// No copy allowed.
	TemplateParameter(const TemplateParameter&);
	TemplateParameter& operator=(const TemplateParameter&);
};

class TemplateParameters : public vector<TemplateParameter*> {
public:
	TemplateParameters();
	~TemplateParameters();
	void release();
	static TemplateParameters& parse(const string& fileNameTemplate, TemplateParameters&, const MetaData& = nullMD);

private:
// No copy allowed.
	TemplateParameters(const TemplateParameters&);
	TemplateParameters& operator=(const TemplateParameters&);

	static MetaData nullMD;
};

} // namespace odb

#endif
