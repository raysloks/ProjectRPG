#include "binistream.h"
#include "binostream.h"

#ifndef STREAMS_H
#define STREAMS_H

#ifndef _USE_STRING_STREAMS

typedef std::binistream instream;
typedef std::binostream outstream;

#else

typedef std::istream instream;
typedef std::ostream outstream;

#endif

#endif