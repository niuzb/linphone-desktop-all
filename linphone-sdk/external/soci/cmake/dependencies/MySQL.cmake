set(MySQL_FIND_QUIETLY TRUE)

find_package(MySQL REQUIRED)

boost_external_report(MySQL INCLUDE_DIR LIBRARIES)
