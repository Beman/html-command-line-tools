//  HTML table-of-contents generator  ----------------------------------------//

//  © Copyright Beman Dawes, 2007

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt)


//----------------------------------------------------------------------------// 

#include <fstream>
#include <iostream>
#include <string>

#include <boost/test/included/prg_exec_monitor.hpp>

using namespace std;

//----------------------------------------------------------------------------// 

int cpp_main( int argc, char* argv[] )
{
  bool suppress_sq_brackets = false;

  if (argc > 1 && *argv[1] == '-' && *(argv[1]+1) == 'x')
  {
    suppress_sq_brackets = true;
    --argc; ++argv;
  }

  if ( argc < 3 )
  {
    cout << "Usage: toc [-x] input-file output-file\n";
    cout << "  Option -x suppresses [...] in heading text\n";
    return 1;
  }

  ifstream fi( argv[1] );
  if ( !fi )
  {
    cout << "Failed to open input file: " << argv[1] << "\n";
    return 1;
  }

  ofstream fo( argv[2] );
  if ( !fo )
  {
    cout << "Failed to open output file: " << argv[2] << "\n";
    return 1;
  }

  string buf;
  getline( fi, buf, '\0' ); // read entire file

  cout << "buf.size() is " << buf.size() << endl;

  string::size_type pos;
  string::size_type end;
  int level;

  //  process each heading
  for ( pos = 0; (pos = buf.find( "<h", pos )) < buf.size(); ++pos )
  {
    // set level and set pos to position after <h#>
    if ( buf[pos+2] < '1' || buf[pos+2] > '9' ) continue;
    level = buf[pos+2] - '0';
    pos += 4;

    // find end of heading
    if ( (end = buf.find( "</h", pos )) == string::npos ) break;

    string text(&buf[pos], &buf[end]);  // initially, assume whole heading is text

    // get anchor and erase anchor from text
    string s(&buf[pos], &buf[end]);
    string anchor;
    string::size_type anchor_pos = s.find("<a name=\""), anchor_end;
    if (anchor_pos != string::npos)
    {
      anchor_pos += 9;
      anchor_end = s.find("\"", anchor_pos);
      if (anchor_end != string::npos)
      {
        anchor = s.substr(anchor_pos, anchor_end - anchor_pos);
        text.erase(anchor_pos - 9, anchor.size() + 11);  // erase <a..>
        anchor_end = text.find("</a>", anchor_pos - 9);
        if (anchor_end != string::npos)
          text.erase(anchor_end, 4);   // erase </a>
      }
    }

    // if requested, remove [...]
    string::size_type bracket_pos;
    if (suppress_sq_brackets && (bracket_pos = text.find('[')) != string::npos)
    {
      string::size_type bracket_end(text.find(']', bracket_pos));
      if (bracket_end != string::npos)
        text.erase(bracket_pos, bracket_end - bracket_pos +1);
    }

    // generate the HTML
    if ( !anchor.empty() )
    {
      fo << "      ";
      for ( int i = 2; i < level; ++i )
        fo << "&nbsp;&nbsp;&nbsp;";
      fo << "<a href=\"#" << anchor << "\">" << text << "</a><br>" << endl;
    }
    else
    {
      cout << "no anchor: <h" << level << ">" << text << "\n"; 
    }
  }                                       

  return 0;
}