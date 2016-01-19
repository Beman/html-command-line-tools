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
  const string start_str("<!-- include \"");
  const string start_snippet_str("snippet=");
  const string label_chars(
    "_1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
  const string start_end_str(" -->");
  const string end_str("<!-- end include -->");

  const string snip_begin_str("<!-- snippet=");
  const string snip_end_str("<!-- end snippet -->");

  const string::size_type min_sz =
    start_str.size() + start_end_str.size() + end_str.size();

  bool htmlize = false;

  string buf;

  string get_snippet_name(string::size_type& pos)
  {
    // if "snippet=name" is found, pos is set to the first non-space after the name
    // otherwise, pos is unchanged.
    string name;
    string::size_type cur = pos;
    while (cur < buf.size() && buf[cur] == ' ') { ++cur; }
    if (cur == buf.size())
    {
      cout << "\nError - end of input file before end of \"<!-- include\" marker";
      exit(1);
    }
    string::size_type end(buf.find(start_snippet_str, cur));
    if (end == cur) // "snippet=" found
    {
      cur += start_snippet_str.size();  // cur now at curition after the equal sign
      if ((end = buf.find_first_not_of(label_chars, cur)) == string::npos)
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
      "  Scans input-file for text in the form:\n"
      "   <!-- include \"include-path\" [snippet=name]-->...<!-- end include -->\n"
      "  and replaces ... with the contents of include-path.\n"
      "  If optional \"snippet=name\" is present, only the named snippet will be\n"
      "  included; otherwise the entire file will be included.\n"
      "  Snippets begin with the line after a line that contains:\n"
      "      <!-- snippet=name -->\n"
      "  and end with the line before a line that contains:\n"
      "      <!-- end snippet -->\n"
      "  Note: For including C/C++ files, the snippet begin and end markers can be\n"
      "  placed in C/C++ comments."
      "  Option --htmlize causes '\"', '&', '<', and '<' in the include file to be\n"
      "  replaced by \"&quot;\", \"&amp;\", \"&lt;\", and \"&gt;\", respectively."
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

      pos = end+1;
      string snippet_name(get_snippet_name(pos));  // updates pos if snippet=name found
      if (buf.find(start_end_str, pos) == pos) // account for start_end_str
        pos += start_end_str.size();
      else
      {
        cout << "Error: " << start_end_str << " after \"" << include_file
          << "\" not found where expected.";
        exit(1);
      }

      if ((end = buf.find(end_str, pos)) != string::npos)
      {
        ifstream finc(include_file);
        if (!finc)
        {
          cout << "include file not found" << endl;
          ++error_count;
          continue;
        }
        string rep;
        getline(finc, rep, '\0'); // read entire file

        if (!snippet_name.empty())
        {
          string::size_type snip_pos;
          string begin_marker(snip_begin_str + snippet_name + " -->");
          if ((snip_pos = rep.find(begin_marker)) == string::npos)
          {
            cout << "Error: could not find " << begin_marker << " in " << include_file
              << endl;
            exit(1);
          }
          snip_pos += begin_marker.size();
          while (snip_pos < rep.size()
            && (buf[snip_pos] == '\r' || buf[snip_pos] == '\n')) {
            ++snip_pos;
          }
          rep.erase(0, snip_pos);
          if ((snip_pos = rep.find(snip_end_str)) != string::npos)
          {
            while (snip_pos != 0
              && (buf[snip_pos] == '\r' || buf[snip_pos] == '\n')) { --snip_pos; }
            rep.erase(snip_pos);
          }
        }

        replace_ascii_chars_with_html_names(rep);
        if (!rep.empty() && rep[rep.size() - 1] == '\n')
          rep.erase(rep.size() - 1);  // erase unwanted trailing newline
        buf.erase(pos, end-pos);
        buf.insert(pos, rep);
        // do not further adjust pos since included file may itself contains includes
        ++file_count;
      }
      else
      {
        cout << "Warning: \"-->\" not found at end of marker for file "
          << include_file << endl;
      }
    }
  }
  
  fo << buf;

  cout << file_count << " files included\n";
  cout << error_count << " errors detected\n";
  return 0;
}