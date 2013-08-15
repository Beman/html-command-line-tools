//  html_ref_from_synopsis.cpp  --------------------------------------------------------//

//  © Copyright Beman Dawes, 2013

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt)

//--------------------------------------------------------------------------------------//
//                                                                                      //
//          Generate reference documentation html boilerplate from a synopsis           //
//                                                                                      //
//--------------------------------------------------------------------------------------// 

#include <iostream>
#include <fstream>
#include <string>
#include <boost/detail/lightweight_main.hpp>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::getline;

namespace
{
  string input_path;
  std::ifstream in;
  string ln;
  string::size_type pos, semi_pos, lt_pos, gt_pos, amp_pos;
}

int cpp_main(int argc, char* argv[])
{
  if (argc < 2)
  {
    cerr << "Invoke: ***** input-path"
         << endl;
    return 1;
  }

  input_path = argv[1];
  in.open(input_path);
  if (!in)
  {
    cerr << "Could not open " << input_path << endl;
    return 1;
  }

  //  read each line

  for (;;)
  {
    getline(in, ln);
    if (!in.good())
      break;

    pos = ln.find_first_not_of(" ");

    // empty lines
    if (ln.empty() || pos == string::npos)
    {
      cout << '\n';
      continue;
    }

    // comment only lines
    if (ln[pos] == '/' && ln[pos+1] == '/')  // comment only
    {
      continue;
    }

    // signature lines
    bool is_void = false;
    cout << "\n<pre>";
    do
    {
      if (ln.find("void") != string::npos)
        is_void = true;
      for (const char* p = ln.c_str() + pos;
           *p != '\0' && !(*p == '/' && *(p+1) == '/'); ++p)  // stop at end or "//"
      {
        while (*p == ' ' && *(p+1) == ' ' && *(p+2) == ' ')  
          ++p;  // reduce multiple spaces to at most two spaces 
        cout << *p;
      }
      semi_pos = ln.rfind(';');
      lt_pos   = ln.rfind("&lt;");
      gt_pos   = ln.rfind("&gt;");
      amp_pos  = ln.rfind("&amp;");
      if (semi_pos != string::npos
          && (lt_pos == string::npos || semi_pos > lt_pos + 3)
          && (gt_pos == string::npos || semi_pos > gt_pos + 3)
          && (amp_pos == string::npos || semi_pos > amp_pos + 4))
        break;
      else
        cout << "\n";
      getline(in, ln);
    } while (in.good());

    if (in.eof())
      break;
    cout << "</pre>\n";
    cout << "<blockquote>\n  <p><i>";

    cout << (is_void ? "Effects" : "Returns")
         << ":</i> </p>\n</blockquote>\n"; 
  }

  if (!in.eof())
  {
    cerr << "Error reading " << input_path << endl;
    return 1;
  }

  return 0;
}