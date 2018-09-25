#include "ftkcommon/UtilsString.h"
 
namespace daq
{
  namespace ftk
  {
    std::string Format(const std::string& fmt, ...)
    {
      int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
      std::string str;
      va_list ap;
      while (1)
	{     // Maximum two passes on a POSIX system...
	  str.resize(size);
	  va_start(ap, fmt);
	  int n = vsnprintf((char *)str.data(), size, fmt.c_str(), ap);
	  va_end(ap);
	  if (n > -1 && n < size)
	    {  // Everything worked
	      str.resize(n);
	      return str;
	    }
	  if (n > -1)  // Needed size returned
	    size = n + 1;   // For null char
	  else
	    size *= 2;      // Guess at a larger size (OS specific)
	}
      return str;
    }
 
    void ReplaceAll( std::string &str , const std::string& from , const std::string& to )
    {
      size_t start_pos = 0;
      while( ( start_pos = str.find(from,start_pos) ) != std::string::npos )
	{
	  str.replace(start_pos, from.length(), to);
	  start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
    }
   
    std::vector<std::string> Tokenize(const std::string& str, char token)
    {
      std::vector<std::string> result;
      std::stringstream ss( str );
      std::string tmp_s;
      while( std::getline( ss , tmp_s , token ) )
	{
	  result.push_back( tmp_s );
	}
      return result;
    }
 
    std::vector<std::string> Tokenize(const std::vector<std::string> &a, char token )
    {
      // tokenize each string in the vector, return a vector with size >= a
      std::vector<std::string> result;
      for( std::size_t i = 0 ; i < a.size() ; i++ )
	{
	  std::vector<std::string> tmp_result = Tokenize( a[i] , token );
	  for( std::size_t j = 0 ; j < tmp_result.size() ; j++ )
	    {
	      result.push_back( tmp_result[j] );
	    }
	}
      return result;
    }
 
    Int_tf BaseConvertLL(const std::string& str, unsigned int base)
    { return std::stoll(str,0,base); }
  }
}
