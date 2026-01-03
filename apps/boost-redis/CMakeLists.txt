cmake_minimum_required(VERSION 3.8...4.0.2)

project(boost-redis-app VERSION "1.0.0" LANGUAGES CXX)

add_subdirectory(/home/marcelo/Public/boost tmp/boost-redis-bin  EXCLUDE_FROM_ALL)

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
function(add_app name)
   add_executable(${name})

   target_compile_features(${name}
      PRIVATE
         cxx_std_20
   )

   target_sources(${name}
     PRIVATE
       ${name}.cpp
   )

   target_link_libraries(${name}
     PRIVATE
       boost_redis_lib
   )

   install(TARGETS ${name})
endfunction()

add_app(app_boost_redis)
add_app(app_boost_redis_cb)

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
