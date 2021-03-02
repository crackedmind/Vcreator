#pragma once

#if defined(VCREATOR_LIBRARY)
#  define VCREATOR_EXPORT Q_DECL_EXPORT
#else
#  define VCREATOR_EXPORT Q_DECL_IMPORT
#endif
