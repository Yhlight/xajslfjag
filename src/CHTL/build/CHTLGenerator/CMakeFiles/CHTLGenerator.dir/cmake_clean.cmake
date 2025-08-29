file(REMOVE_RECURSE
  "libCHTLGenerator.a"
  "libCHTLGenerator.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/CHTLGenerator.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
