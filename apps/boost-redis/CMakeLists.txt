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
add_executable(app_boost_redis)

target_compile_features(app_boost_redis
   PRIVATE
      cxx_std_20
)

target_sources(app_boost_redis
  PRIVATE
    main.cpp
)

target_link_libraries(app_boost_redis
  PRIVATE
    boost_redis_lib
)

install(TARGETS app_boost_redis)

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
