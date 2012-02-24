///
/// \file DispatchingWriter.h
///
/// @author Piotr Kuchta, June 2009

#ifndef DispatchingWriter_H
#define DispatchingWriter_H

class PathName;

namespace odb {

class DispatchingWriter
{
public:
	typedef WriterDispatchingIterator<> iterator_class;
	typedef IteratorProxy<iterator_class, DispatchingWriter>  iterator;

	DispatchingWriter(const string &outputFileTemplate, int maxOpenFiles = 0);
	virtual ~DispatchingWriter();

	const string outputFileTemplate() { return outputFileTemplate_; }

	iterator begin();

protected:
	iterator_class* writer();

private:
// No copy allowed
    DispatchingWriter(const DispatchingWriter&);
    DispatchingWriter& operator=(const DispatchingWriter&);

	const string outputFileTemplate_;
	int maxOpenFiles_; 
};

} // namespace odb 

#endif
