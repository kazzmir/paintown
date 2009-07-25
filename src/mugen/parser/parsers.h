#ifndef _paintown_mugen_parsers_h
#define _paintown_mugen_parsers_h

#include "parse-exception.h"

namespace Mugen{
  void parseAir(const std::string & filename);
  void parseCns(const std::string & filename);
  void parseCmd(const std::string & filename);
  void parseDef(const std::string & filename) throw (ParserException);
}

#endif
