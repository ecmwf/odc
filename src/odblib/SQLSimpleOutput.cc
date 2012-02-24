#include "SQLSelect.h"
#include "SQLSimpleOutput.h"
#include "Decoder.h"

namespace odb {
namespace sql {

SQLSimpleOutput::SQLSimpleOutput(ostream& out):
	out_(out),
	count_(0)
{
	out_ << fixed;
}

SQLSimpleOutput::~SQLSimpleOutput() {}

void SQLSimpleOutput::print(ostream& s) const
{
	s << "SQLSimpleOutput";
}

void SQLSimpleOutput::size(int) {}
void SQLSimpleOutput::reset() { count_ = 0; }
void SQLSimpleOutput::flush() { out_ << std::flush; }

bool SQLSimpleOutput::output(const expression::Expressions& results)
{
	size_t n = results.size();
    for(size_t i = 0; i < n; i++)
    {
        if(i) out_ << config_.fieldDelimiter;
        results[i]->output(*this);
    }
    out_ << endl;
	count_++;
	return true;
}

void SQLSimpleOutput::outputReal(double x, bool missing) const
{
	if (missing && !config_.doNotWriteNULL)
		out_ << "NULL";
	else
		out_ << x;
}

void SQLSimpleOutput::outputDouble(double x, bool missing) const
{
	if (missing && !config_.doNotWriteNULL)
		out_ << "NULL";
	else
		out_ << x;
}

void SQLSimpleOutput::outputInt(double x, bool missing) const
{
	if (missing && !config_.doNotWriteNULL)
		out_ << "NULL";
	else
		out_ << static_cast<long long>(x);
}

void SQLSimpleOutput::outputUnsignedInt(double x, bool missing) const
{
	if (missing && !config_.doNotWriteNULL)
		out_ << "NULL";
	else
		out_ << static_cast<unsigned long>(x);
}

void SQLSimpleOutput::outputString(double x, bool missing) const
{
	if (missing && !config_.doNotWriteNULL)
		out_ << "NULL";
	else
	{
		out_ << "'";
		char *p = reinterpret_cast<char*>(&x);
		for(size_t i = 0; i < sizeof(x); i++)
			if(p[i] != ' ' && isprint(p[i]))
				out_ << p[i];
		out_ << "'";
	}
}

void SQLSimpleOutput::outputBitfield(double x, bool missing) const
{
	if (missing && !config_.doNotWriteNULL)
		out_ << "NULL";
	else
		Decoder::printBinary(out_, static_cast<unsigned long>(x));
}

void SQLSimpleOutput::prepare(SQLSelect& sql)
{
	if (config_.doNotWriteColumnNames)
		return;

	const expression::Expressions& columns(sql.output());
	for (size_t i = 0; i < columns.size(); i++)
	{
		string name = columns[i]->title();
		//TODO: get types of columns - these can be any expressions, not only selected tables' columns
		//SQLTable *table = sql.findTable(name);
		//string typeName = sql.typeOf(name, table)->name();
		if(i)
			out_ << config_.fieldDelimiter;
		out_ << name; // << ":" << typeName;
		
	}
    out_ << endl;
}

void SQLSimpleOutput::cleanup(SQLSelect& sql) {}
unsigned long long SQLSimpleOutput::count() { return count_; }

} // namespace sql
} // namespace odb
