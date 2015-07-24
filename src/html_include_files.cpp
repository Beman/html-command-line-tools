//  html_include_files.cpp  ------------------------------------------------------------//

//  © Copyright Beman Dawes  2007, 2015

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt)


//--------------------------------------------------------------------------------------// 

#include <fstream>
#include <iostream>
#include <string>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/detail/lightweight_main.hpp>

using namespace std;

namespace
{
  const string start_str("<!-- include file \"");
  const string start_end_str("\" -->");
  const string end_str("<!-- end include file -->");
  const string pre_start_str("<pre>");
  const string pre_end_str("</pre>");

  const string::size_type min_sz =
    start_str.size() + start_end_str.size() + end_str.size();

}  // unnamed namespace

//--------------------------------------------------------------------------------------// 

int cpp_main( int argc, char* argv[] )
{

  //if (argc > 1 && *argv[1] == '-' && *(argv[1]+1) == 'x')
  //{
  //  suppress_sq_brackets = true;
  //  --argc; ++argv;
  //}

  if (argc < 3)
  {
    cout << "Usage: html_include_files input-file output-file\n";
    cout << 
      "Scans input-file for text in the form:\n"
      "   <!-- include file \"include-file\" -->...<!-- end include file --> and\n"
      "   replaces ... with the contents of include-file prefixed with \"<pre>\"\n"
      "   and suffixed with \"</pre>\"";
    return 1;
  }

  ifstream fi(argv[1]);
  if (!fi)
  {
    cout << "Failed to open input file: " << argv[1] << "\n";
    return 1;
  }

  ofstream fo(argv[2]);
  if (!fo)
  {
    cout << "Failed to open output file: " << argv[2] << "\n";
    return 1;
  }

  string buf;
  getline(fi, buf, '\0'); // read entire file

  cout << "buf.size() is " << buf.size() << endl;

  string::size_type pos;
  string::size_type end;
  string include_file;
  int file_count = 0;
  int error_count = 0;

  //  process each include file
  for (pos = 0;
    pos + min_sz < buf.size() && (pos = buf.find(start_str, pos)) < buf.size();
    ++pos)
  {
    pos += start_str.size();
    if ((end = buf.find(start_end_str, pos + 1)) != string::npos)
    {
      include_file = buf.substr(pos, end - pos);
      cout << "include file \"" << include_file << "\" ";
      pos = end + start_end_str.size();
      if ((end = buf.find(end_str, pos)) != string::npos)
      {
        ifstream finc(include_file);
        if (!finc)
        {
          cout << "not found" << endl;
          ++error_count;
          continue;
        }
        string rep;
        getline(finc, rep, '\0'); // read entire file
        if (!rep.empty() && rep[rep.size() - 1] == '\n')
          rep.erase(rep.size() - 1);  // erase unwanted trailing newline
        cout << "replacing:\n" << buf.substr(pos, end-pos) << "\nwith:\n" << rep << '\n';
        buf.erase(pos, end-pos);
        buf.insert(pos, pre_start_str);
        pos += pre_start_str.size();
        buf.insert(pos, rep);
        // do not further adjust pos since included file may itself contains includes
        buf.insert(pos + rep.size(), pre_end_str);
        ++file_count;
      }
    }
  }
  
  fo << buf;

  cout << "buf.size() is " << buf.size() << endl;

  cout << file_count << " files included\n";
  cout << error_count << " errors detected\n";
  return 0;
}