/*
 * (C) Copyright 1996-2012 ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */
#ifndef ToolFactory_H
#define ToolFactory_H

// #include "TestCase.h"

namespace odc {
namespace tool {

namespace test {
class TestCase;
}
class Tool;

class AbstractToolFactory {
public:

    static Tool* createTool(const std::string& name, int argc, char** argv);

    static void printToolHelp(const std::string&, std::ostream&);
    static void printToolUsage(const std::string& name, std::ostream&);
    static void printToolsHelp(std::ostream&);
    static std::vector<odc::tool::test::TestCase*>* testCases(const std::vector<std::string>& = matchAll);

    static void listTools(std::ostream&);

    virtual Tool* create(int argc, char** argv) = 0;

    virtual void help(std::ostream&)                      = 0;
    virtual void usage(const std::string&, std::ostream&) = 0;
    virtual bool experimental()                           = 0;

protected:

    AbstractToolFactory(const std::string& name);
    virtual ~AbstractToolFactory();

private:

    static AbstractToolFactory& findTool(const std::string& name);
    static std::map<std::string, AbstractToolFactory*>* toolFactories;
    static const std::vector<std::string> matchAll;
};


template <class T>
class ToolFactory : public AbstractToolFactory {
public:

    ToolFactory(const std::string& name) : AbstractToolFactory(name) {}

    Tool* create(int argc, char** argv) { return new T(argc, argv); }

    void help(std::ostream& o) { T::help(o); }
    void usage(const std::string& name, std::ostream& o) { T::usage(name, o); }
    bool experimental() { return odc::tool::ExperimentalTool<T>::experimental; }
};

}  // namespace tool
}  // namespace odc

#endif
