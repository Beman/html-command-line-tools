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
  string::size_type pos;
}

int main(int argc, char* argv[])
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

  getline(in, ln);
  while (in.good())
  {
    pos = ln.find_first_not_of(" ");

    // empty lines
    if (ln.empty() || pos == string::npos)
    {
      cout << '\n';
      getline(in, ln);
    }

    // comment only lines
    if (ln[pos] == '/' && ln[pos+1] == '/')  // comment only
    {
      getline(in, ln);
    }

    // signature lines
    bool is_void = false;
    cout << "\n<pre>";
    do
    {
      if (ln.find("void") != string::npos)
        is_void = true;
      cout << ln.c_str() + pos;
      if (ln.find(';') != string::npos)
        break;
      getline(in, ln);
    } while (in.good());

    if (!in.eof())
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