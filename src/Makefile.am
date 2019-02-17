bin_PROGRAMS = mstrie
AM_CXXFLAGS = -std=c++14 -Ilib
mstrie_SOURCES = \
	utils/file_utils.cpp \
    utils/file_utils.hpp \
	core/mstrie.cpp \
    core/mstrie.hpp \
	core/index_manager.cpp \
    core/index_manager.hpp \
	cli/cli.cpp \
    cli/cli.hpp \
	benchmark/benchmark.cpp \
    benchmark/benchmark.hpp \
	main.cpp
mstrie_LDADD=lib/libconfigurator.a
