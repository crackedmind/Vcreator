#pragma once

#include <app/app_version.h>

#define QTCREATOR_VERSION QTCREATOR_VERSION_CHECK(IDE_VERSION_MAJOR, IDE_VERSION_MINOR, IDE_VERSION_RELEASE)
#define QTCREATOR_VERSION_CHECK(major, minor, release) ((major<<24)|(minor<<16)|(release))
