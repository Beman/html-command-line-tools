//  html_include_files.cpp  ------------------------------------------------------------//

//  © Copyright Beman Dawes  2007, 2015

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt


//--------------------------------------------------------------------------------------// 

#include <fstream>
#include <iostream>
#include <string>
#include <cstring>   // for strchr
#include <boost/algorithm/string/predicate.hpp>
#include <boost/detail/lightweight_main.hpp>

using namespace std;

namespace
{
  const string start_str("<!-- include file \"");
  const string start_end_str("\" -->");
  const string end_str("<!-- end include file -->");

  const string::size_type min_sz =
    start_str.size() + start_end_str.size() + end_str.size();

  bool start_at_number_sign = false;

  //  replace '"', '&', '<', '>' with html names "&quot;", "&amp;", "&lt;", "&gt;"
  void replace_ascii_chars_with_html_names(string& s)
  {
    static const string chars("\"&<>");
    static const char* name[] = { "&quot;", "&amp;", "&lt;", "&gt;" };

    for (string::size_type pos = 0u;
    (pos = s.find_first_of(chars, pos)) != string::npos;
      ++pos)
    {
      size_t which = std::strchr(chars.c_str(), s[pos]) - chars.c_str();
      s.replace(pos, 1, name[which]);
    }
  }

}  // unnamed namespace

//--------------------------------------------------------------------------------------// 

int cpp_main( int argc, char* argv[] )
{

  if (argc > 1 && *argv[1] == '+' && *(argv[1]+1) == '#')
  {
    start_at_number_sign = true;
    --argc; ++argv;
  }

  if (argc < 3)
  {
    cout << "Usage: html_include_files [+#] input-file output-file\n";
    cout << 
      "Scans input-file for text in the form:\n"
      "   <!-- include file \"include-file\" -->...<!-- end include file --> and\n"
      "   replace ... with the contents of include-file.\n"
      "Option:  +#   Start with first '#' in included file. Used to bypass boilerplate\n"
      ;
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

  //cout << "buf.size() is " << buf.size() << endl;

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
      cout << "include file \"" << include_file << "\"\n";
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
        string::size_type pos_num_sign;
        getline(finc, rep, '\0'); // read entire file

        if (start_at_number_sign && (pos_num_sign = rep.find("#")) != string::npos)
          rep.erase(0, pos_num_sign);
        replace_ascii_chars_with_html_names(rep);
        if (!rep.empty() && rep[rep.size() - 1] == '\n')
          rep.erase(rep.size() - 1);  // erase unwanted trailing newline
        buf.erase(pos, end-pos);
        buf.insert(pos, rep);
        // do not further adjust pos since included file may itself contains includes
        ++file_count;
      }
    }
  }
  
  fo << buf;

  cout << file_count << " files included\n";
  cout << error_count << " errors detected\n";
  return 0;
}