cmake_minimum_required(VERSION 3.8...4.0.2)

project(boost-redis-app VERSION "1.0.0" LANGUAGES CXX)

add_subdirectory($ENV{HOME}/Public/boost tmp/build-boost-redis EXCLUDE_FROM_ALL)

add_library(Boost::redis_asio ALIAS boost_redis)

function(add_app name flavor)
   set(app app_boost_redis_${name})
   add_executable(${app})

   target_compile_features(${app}
      PRIVATE
         cxx_std_20
   )

   target_sources(${app}
     PRIVATE
       ${app}.cpp
       boost_redis_lib_${flavor}.cpp
   )

   target_link_libraries(${app}
     PRIVATE
        Boost::redis_${flavor}
   )

   install(TARGETS ${app})
endfunction()

add_app(asio_co asio)
add_app(asio_cb asio)
add_app(corosio corosio)

