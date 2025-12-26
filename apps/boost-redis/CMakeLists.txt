cmake_minimum_required(VERSION 3.8...4.0.2)

project(boost-redis-app VERSION "1.0.0" LANGUAGES CXX)

add_subdirectory(/home/marcelo/Public/boost boost-redis EXCLUDE_FROM_ALL)

#==============================================================================
add_library(boost_redis_lib STATIC EXCLUDE_FROM_ALL)

target_sources(boost_redis_lib
  PRIVATE
    boost_redis_lib.cpp
)

target_link_libraries(boost_redis_lib
   PUBLIC
     Boost::redis
)

#==============================================================================
add_executable(app)

target_compile_features(app
   PRIVATE
      cxx_std_20
)

target_sources(app
  PRIVATE
    main.cpp
)

target_link_libraries(app
  PRIVATE
    boost_redis_lib
)

#==============================================================================
add_executable(monitor)

target_compile_features(monitor
   PRIVATE
      cxx_std_20
)

target_sources(monitor
  PRIVATE
    monitor.cpp
)

target_link_libraries(monitor
  PRIVATE
    boost_redis_lib
)
