set(PostgreSQL_FIND_QUIETLY TRUE)

find_package(PostgreSQL REQUIRED)

boost_external_report(PostgreSQL INCLUDE_DIRS LIBRARIES VERSION)
