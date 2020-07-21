set(ORACLE_FIND_QUIETLY TRUE)

find_package(Oracle REQUIRED)

boost_external_report(Oracle INCLUDE_DIR LIBRARIES)
