#include "eckit/runtime/Tool.h"

class UnitTest : public eckit::Tool {

   virtual void setUp() {}
   virtual void test() {}
   virtual void tearDown() {}

    virtual void run() {
        setUp();
        test();
        tearDown();
    }

public:
    UnitTest(int argc, char** argv):
        eckit::Tool(argc, argv) {}

};

//=========================================

static void noop() {}

class JustTest : public UnitTest {
    void (*t_)();
    void (*u_)();
    void (*d_)();

    virtual void test()     { t_(); }
    virtual void setUp()    { u_(); }
    virtual void tearDown() { d_(); }

public:
    JustTest(int argc, char** argv, 
            void (*t)(), void (*u)() = noop,void (*d)() = noop): UnitTest(argc, argv), t_(t), u_(u), d_(d) {}

};

class OneTest;

static OneTest* one_tests = 0;

class OneTest {
    OneTest *next_;
    const char* n_;
    void (*t_)();
public:
    OneTest(const char* n, void (*t)()): n_(n), t_(t), next_(one_tests) { one_tests =  this; }
};


#define TEST(name) static void name(); static OneTest test##name(#name, &name); static void name() 

//=========================================




#define _JUST_ONE_TEST(t,u,d) int main(int c,char** v) { JustTest x(c,v,&t,&u,&d); x.start(); return 0; }
#define TEST_MAIN _JUST_ONE_TEST(test,setUp,tearDown)
#define MANY_TESTS_MAIN _JUST_ONE_TEST(test,setUp,tearDown)
