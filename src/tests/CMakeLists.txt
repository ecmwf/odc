ecbuild_add_test(   TARGET      test_odb
					BOOST
					SOURCES     test_odb_example.cc
					ARGS        "-integer 100 -listlong 88,99,11,22"
                    LIBS        Odb )
