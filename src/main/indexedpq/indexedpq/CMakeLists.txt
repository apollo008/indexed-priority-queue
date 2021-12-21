add_subdirectory(test)

add_executable(indexedpq_test
        indexed-priority-queue-unittest.cpp
        ${DOTEST_CPP}
        )

target_link_libraries(indexedpq_test
        tulip
        cppunit
        dl
        )

add_test(indexedpq_test indexedpq_test)