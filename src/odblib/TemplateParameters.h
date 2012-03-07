/*
 * © Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

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
