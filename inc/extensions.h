#ifndef __AWRY_EXTENSIONS_H__
#define __AWRY_EXTENSIONS_H__ 1

#include "awry/common.h"

#define AWRY_splat_args(...) __VA_ARGS__

#define AWRY_extension_forwards(...) AWRY_splat_args ( __VA_ARGS__ )
#define AWRY_setup_extensions(...) AWRY_splat_args ( __VA_ARGS__ )
#define AWRY_extension(symbol, datatype)  datatype: __expect_##symbol,

#endif