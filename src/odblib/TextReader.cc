///
/// \file ODA.cc
///
/// @author Piotr Kuchta, Oct 2010


#include <algorithm>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <math.h>

using namespace std;

#include "LimitValues.h"

#include "Exceptions.h"
#include "PathName.h"
#include "MemoryBlock.h"
#include "DataHandle.h"
#include "FileHandle.h"

#include "SQLBitfield.h"
#include "DataStream.h"
#include "HashTable.h"
#include "Codec.h"
#include "Column.h"
#include "HashTable.h"
#include "SQLBitfield.h"
#include "SQLIteratorSession.h"
#include "MetaData.h"
#include "RowsIterator.h"
#include "Header.h"
#include "IteratorProxy.h"
#include "Writer.h"
#include "TextReader.h"
#include "Reader.h"
#include "TextReaderIterator.h"
#include "WriterBufferingIterator.h"
#include "FixedSizeWriterIterator.h"
#include "RowsIterator.h"
#include "SelectIterator.h"


#include "SQLType.h"
#include "SQLInteractiveSession.h"
#include "SQLIteratorSession.h"
#include "SQLTable.h"
#include "SQLSelect.h"
#include "SQLParser.h"
#include "SQLExpression.h"


using namespace std;

namespace odb {
#define MEGA(x) (x*1024*1024)

TextReader::TextReader(std::istream& input)
: in_(&input),
  deleteDataHandle_(false)
{}

TextReader::TextReader()
: in_(0),
  deleteDataHandle_(true),
  path_("")
{}

TextReader::TextReader(const string& path)
: in_(new std::ifstream(path.c_str())),
  deleteDataHandle_(true),
  path_(path)
{
        //dataHandle_->openForRead();
}

TextReader::~TextReader()
{
        ///if (dataHandle_ && deleteDataHandle_)
        if (in_ && deleteDataHandle_)
        {
                //dataHandle_->close();
                //delete dataHandle_;
                delete in_;
        }
}

TextReaderIterator* TextReader::createReadIterator(const PathName& pathName)
{
        return new TextReaderIterator(*this, pathName);
}

TextReader::iterator TextReader::begin()
{
        TextReaderIterator * it = new TextReaderIterator(*this);
        it->next();
        return iterator(it);
}

const TextReader::iterator TextReader::end() { return iterator(0); }

} // namespace odb
