#include "eckit/runtime/Tool.h"

namespace odb {
namespace tool {
namespace test {

class UnitTest : public eckit::Tool {

    void setUp();
    void test();
    void tearDown();

    virtual void run() {
        setUp();
        test();
        tearDown();
    }
public:
    UnitTest(int argc, char** argv):
        eckit::Tool(argc, argv) {}

};

}
}
}

int main(int argc, char** argv) {
    odb::tool::test::UnitTest u(argc, argv);
    u.start();
}
