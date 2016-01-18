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
  const string start_snippet_str("snippet=");
  const string label_chars(
    "_1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
  const string start_end_str("-->");
  const string end_str("<!-- end include file -->");

  const string::size_type min_sz =
    start_str.size() + start_end_str.size() + end_str.size();

  bool htmlize = false;

  string buf;

  string get_snippet_name(string::size_type& pos)
  {
    // if a snippet name is found, pos is set to the first non-space after the name
    // otherwise, pos is unchanged.
    string name;
    string::size_type cur = pos;
    while (cur < buf.size() && buf[cur] == ' ') { ++cur; }
    if (cur == buf.size())
    {
      cout << "\nError - end of input file before end of \"<!-- include file\" marker";
      exit(1);
    }
    string::size_type end(buf.find(start_snippet_str, cur));
    if (end == cur) // "snippet=" found
    {
      cur += start_snippet_str.size();  // cur now at curition after the equal sign
      if ((end = buf.find_first_not_of(label_chars, cur)) == string::ncur)
      {
        cout << "\nError - end of input file while processing \"snippet=name\"";
        exit(1);
      }
      name = buf.substr(cur, end-cur);
      pos = end;
    }
    return name;
  }

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

  if (argc > 1 && *argv[1] == '-')
  {
    string s(argv[1]);
    if (s == "--htmlize")
    {
      htmlize = true;
      --argc; ++argv;
    }
    else
      cout << "Invalid option: " << s << endl;
  }

  if (argc < 3)
  {
    cout << "Usage: html_include_files [--htmlize] input-file output-file\n";
    cout <<
      "Scans input-file for text in the form:\n"
      "       <!-- include file \"include-path\" [snippet=name]-->\n"
      "       ...\n"
      "       <!-- end include file -->\n"
      "   and replaces ... with the contents of include-path.\n"
      "   If optional \"snippet=name\" is present, only the named snippet will be\n"
      "   included; otherwise the entire file will be included.\n"
      "   Snippets begin with the line after a line that contains:\n"
      "       <!-- snippet=name -->\n"
      "   and end with the line before a line that contains:\n"
      "       <!-- snippet end -->\n"
      "   Note: For including C/C++ files, the snippet begin and end markers can be\n"
      "   placed in C/C++ comments."
      "   Option --htmlize causes '\"', '&', '<', and '<' in the include file to be\n"
      "   replaced by \"&quot;\", \"&amp;\", \"&lt;\", and \"&gt;\", respectively."
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
  getline(fi, buf, '\0'); // read entire file

  //cout << "buf.size() is " << buf.size() << endl;

  string::size_type pos;
  string::size_type end;
  int file_count = 0;
  int error_count = 0;

  //  process each include file

  for (pos = 0;
    pos + min_sz < buf.size() && (pos = buf.find(start_str, pos)) < buf.size();
    ++pos)
  {
    pos += start_str.size();  // set pos to beginning of include file path
    if ((end = buf.find('"', pos + 1)) != string::npos)
    {
      string include_file(buf.substr(pos, end - pos));
      cout << "include file \"" << include_file << "\"\n";

      pos = end;
      string snippet_name(get_snippet_name(pos));  // updates pos, empty() if not found

      if ((end = buf.find(end_str, pos)) != string::npos)
      {
        ifstream finc(include_file);
        if (!finc)
        {
          cout << "file not found" << endl;
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