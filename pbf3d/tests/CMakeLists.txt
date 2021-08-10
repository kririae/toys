macro(add_test test_name)
  add_executable("${test_name}" "${test_name}.cpp")
  target_link_libraries("${test_name}" pbf3d)
endmacro(add_test)

#add_test(random_particles)