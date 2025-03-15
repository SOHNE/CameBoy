# CPM Package Lock
# This file should be committed to version control

# PackageProject.cmake
CPMDeclarePackage(PackageProject.cmake
  VERSION 1.13.0
  GITHUB_REPOSITORY TheLartians/PackageProject.cmake
  SYSTEM YES
  EXCLUDE_FROM_ALL YES
)
# GroupSourcesByFolder.cmake
CPMDeclarePackage(GroupSourcesByFolder.cmake
  VERSION 1.0
  GITHUB_REPOSITORY TheLartians/GroupSourcesByFolder.cmake
  SYSTEM YES
  EXCLUDE_FROM_ALL YES
)
# CPMLicenses.cmake
CPMDeclarePackage(CPMLicenses.cmake
  VERSION 0.0.7
  GITHUB_REPOSITORY cpm-cmake/CPMLicenses.cmake
  SYSTEM YES
  EXCLUDE_FROM_ALL YES
)
