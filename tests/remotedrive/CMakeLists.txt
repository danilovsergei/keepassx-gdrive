add_subdirectory(commands)
add_unit_test(NAME testcommand SOURCES TestCommand.cpp MOCS TestCommand.h LIBS ${TEST_LIBRARIES})
add_unit_test(NAME testdatabasesync SOURCES TestDatabaseSync.cpp MOCS TestDatabaseSync.h
              LIBS ${TEST_LIBRARIES})
add_unit_test(NAME testremotedatabasesync SOURCES TestDatabaseRemoteSync.cpp MOCS TestDatabaseRemoteSync.h
              LIBS ${TEST_LIBRARIES})
 
