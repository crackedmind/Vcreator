set(resource_directories
  templates
)


# copy resource directories during build
qtc_copy_to_builddir(copy_share_to_builddir
  DIRECTORIES ${resource_directories}
  DESTINATION "${IDE_DATA_PATH}"
  CREATE_SUBDIRS
)

# create install rule for resource directories
install(
  DIRECTORY ${resource_directories}
  DESTINATION "${IDE_DATA_PATH}"
  USE_SOURCE_PERMISSIONS
)

