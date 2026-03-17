cmake_minimum_required(VERSION 3.8...4.0.2)

project(boost-redis-app VERSION "1.0.0" LANGUAGES CXX)

add_subdirectory(/home/marcelo/Public/boost tmp/build-boost-redis EXCLUDE_FROM_ALL)

function(add_app name)
   add_executable(${name})

   target_compile_features(${name}
      PRIVATE
         cxx_std_20
   )

   target_sources(${name}
     PRIVATE
       ${name}.cpp
       boost_redis_lib.cpp
   )

   target_link_libraries(${name}
     PRIVATE
        Boost::redis
   )

   install(TARGETS ${name})
endfunction()

add_app(app_boost_redis_co)
add_app(app_boost_redis_cb)
