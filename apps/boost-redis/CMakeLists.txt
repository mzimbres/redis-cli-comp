cmake_minimum_required(VERSION 3.8...4.0.2)

project(boost-redis-app VERSION "1.0.0" LANGUAGES CXX)

add_subdirectory($ENV{HOME}/Public/boost tmp/build-boost-redis EXCLUDE_FROM_ALL)

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

add_app(app_boost_redis_asio_co)
add_app(app_boost_redis_asio_cb)

#------------------------------------------------------------------------------

#add_executable(app_boost_redis_corosio)
#
#target_compile_features(app_boost_redis_corosio
#  PRIVATE
#     cxx_std_20
#)
#
#target_sources(app_boost_redis_corosio
# PRIVATE
#   app_boost_redis_corosio.cpp
#   boost_redis_corosio.cpp
#)
#
#target_link_libraries(app_boost_redis_corosio
# PRIVATE
#    Boost::redis_corosio
#)
#
#install(TARGETS app_boost_redis_corosio)
