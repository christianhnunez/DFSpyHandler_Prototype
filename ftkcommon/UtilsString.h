#ifndef UTILSSTRING_H_
#define UTILSSTRING_H_

#include <assert.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdarg.h>
#include <stdlib.h>

#include "ftkcommon/UtilsMath.h"

namespace daq
{
  namespace ftk
  {
    std::string Format(const std::string& fmt, ...);

    void ReplaceAll(std::string& str, const std::string& from, const std::string& to);

    std::vector<std::string> Tokenize(const std::string& str, char token);

    std::vector<std::string> Tokenize(const std::vector<std::string> &a , char token);

    Int_tf BaseConvertLL(const std::string& str,unsigned int base);

  }
}

#endif // UTILSSTRING_H_
