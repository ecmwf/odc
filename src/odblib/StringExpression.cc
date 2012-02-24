#include "StringExpression.h"
#include "Exceptions.h"
#include "StringTool.h"
#include "SQLTable.h"
#include "StringExpression.h"
#include "ColumnExpression.h"


namespace odb {
namespace sql {
namespace expression {

StringExpression::StringExpression(const string& name)
: name_(name)
{
	ASSERT(name.length() <= sizeof(value_));

	char* buf = (char*)&value_;
	memset(buf,' ',sizeof(value_));

	int  off = sizeof(value_) - name.length();
	for(size_t i = 0; i < name.length(); i++)
		buf[off+i] = name[i];

	string s(buf, sizeof(double));
	//Log::info() << "StringExpression::StringExpression: '" << s << "'" << endl;
}

StringExpression::StringExpression(const StringExpression& o)
: name_(o.name_), value_(o.value_)
{}

void StringExpression::expandStars(const std::vector<SQLTable*>& tables, expression::Expressions& e)
{
	ostream& L(Log::info());

	Log::info() << "StringExpression::expandStars: name_: '"  << name_ << "', value_: '" << value_ << "'" << endl;

	if (! StringTool::isColumnRegex(name_))
	{
		e.push_back(this);
		return;
	}

	unsigned int matched = 0;
	for(std::vector<SQLTable*>::const_iterator j = tables.begin();  j != tables.end(); ++j)
	{
		SQLTable* table = (*j);
		std::vector<string> names = table->columnNames();

		for(size_t i = 0; i < names.size(); i++)
		{
			const string& name = names[i];
			if (! StringTool::matchEx(name_, name))
			{
				L << "StringExpression::expandStars: skip '" << name << "'" << endl;
				continue;
			}
			
			L << "StringExpression::expandStars: adding '" << name << "'" << endl;
			++matched;
			e.push_back(new ColumnExpression(name, table));
		}
	}
	if (! matched)
		throw UserError(string("No columns matching regex '") + name_ + "' found.");
	delete this;
}

SQLExpression* StringExpression::clone() const { return new StringExpression(name_); }

StringExpression::~StringExpression() {}

double StringExpression::eval(bool& missing) const { return value_; }

void StringExpression::prepare(SQLSelect& sql) {}

void StringExpression::cleanup(SQLSelect& sql) {}

void StringExpression::output(ostream& s) const { s << name_; }

void StringExpression::print(ostream& s) const { s << "'" << name_ << "'"; }

} // namespace expression
} // namespace sql
} // namespace odb 
