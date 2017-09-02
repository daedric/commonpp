#
# File: src/commonpp/thread/detail/CMakeLists.txt
# Part of commonp.
#
# Distributed under the 2-clause BSD licence (See LICENCE.TXT file at the
# project root).
#
# Copyright (c) 2016 Thomas Sanchez.  All rights reserved.
#

set(srcs
	logger.cpp
)

if (NOT HWLOC_FOUND)
	message(WARNING "HWLOC has not been found, thread pining is disabled.")
else()
	set(srcs
		${srcs}
		Core.cpp
		Cores.cpp
	)
endif()

add_commonpp_library_source(
    SOURCES
        ${srcs}
)