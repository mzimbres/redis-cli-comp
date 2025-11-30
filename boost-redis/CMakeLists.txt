cmake_minimum_required(VERSION 3.8...4.0.2)

project(boost-redis-app VERSION "1.0.0" LANGUAGES CXX)

add_subdirectory(/home/marcelo/Public/boost boost-redis EXCLUDE_FROM_ALL)

#==============================================================================
add_executable(boost_redis_app)

target_compile_features(boost_redis_app
   PRIVATE
      cxx_std_20
)

target_sources(boost_redis_app
  PRIVATE
    main.cpp
)

target_link_libraries(boost_redis_app
  PRIVATE
    Boost::redis
)

#==============================================================================
add_executable(counter)

target_compile_features(counter
   PRIVATE
      cxx_std_20
)

target_sources(counter
  PRIVATE
    main.cpp
)

target_link_libraries(counter
  PRIVATE
    Boost::redis
)
