//  HTML table-of-contents generator  ----------------------------------------//

//  © Copyright Beman Dawes, 2007

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt)


//----------------------------------------------------------------------------// 

#include <fstream>
#include <iostream>
#include <string>

#include <boost/detail/lightweight_main.hpp>

using namespace std;

namespace
{
  bool suppress_sq_brackets = false;
  bool all_anchors = false;

  string buf;

  string::size_type heading_or_anchor(string::size_type pos)
  {
    string::size_type heading = buf.find( "<h", pos );
    string::size_type anchor = all_anchors ? buf.find( "<a name=", pos ) : string::npos;
    return heading < anchor ? heading : anchor;
  }
}

//----------------------------------------------------------------------------// 

int cpp_main( int argc, char* argv[] )
{
  if (argc > 1 && *argv[1] == '-' && *(argv[1]+1) == 'x')
  {
    suppress_sq_brackets = true;
    --argc; ++argv;
  }
  else if (argc > 1 && *argv[1] == '-' && *(argv[1]+1) == 'a')
  {
    all_anchors = true;
    --argc; ++argv;
  }

  if ( argc < 3 )
  {
    cout << "Usage: toc [-x] [-a] input-file output-file\n";
    cout << "  Option -x suppresses [...] in heading text\n";
    cout << "  Option -a adds all anchors to table-of-contents, not just headings\n";
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

  getline( fi, buf, '\0' ); // read entire file

  cout << "buf.size() is " << buf.size() << endl;

  string::size_type pos;
  string::size_type end;
  int level;

  //  process each heading or anchor
  for ( pos = 0; (pos = heading_or_anchor(pos)) < buf.size(); ++pos )
  {
    string text;
    string anchor;

    if (buf[pos+1] == 'h')
    {
      // set level
      if ( buf[pos+2] < '1' || buf[pos+2] > '9' ) continue;
      level = buf[pos+2] - '0';

      // if id = present, save value as anchor
      if (buf.find(" id=\"", pos+3) == pos + 3)
      {
        if ((end = buf.find('"', pos + 8)) != string::npos)
        {
          anchor = buf.substr(pos + 8, end - (pos + 8));
        }
      }

      if ((end = buf.find('>', pos+2)) == string::npos) break;
      pos = end + 1; // set pos to position after <h#...>

      // find end of heading
      if ( (end = buf.find( "</h", pos )) == string::npos ) break;

      text.append(&buf[pos], &buf[end]);  // initially, assume whole heading is text

      if (anchor.empty())
      {
        // get anchor and erase anchor from text
        string s(&buf[pos], &buf[end]);
        string::size_type anchor_pos = s.find("<a name=\""), anchor_end;
        string::size_type id_pos = s.find("id=\"");
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
//        fo << "&nbsp;&nbsp;&nbsp;";
        for ( int i = 2; i < level; ++i )
          fo << "&nbsp;&nbsp;&nbsp;";
        fo << "<a href=\"#" << anchor << "\">" << text << "</a><br>" << endl;
      }
      else
      {
        cout << "no anchor: <h" << level << ">" << text << "\n"; 
      }

      pos = end;
    }
    else // pos is for "<a name=\"" not in a heading
    {
      // find end of heading
      if ( (end = buf.find( "</a>", pos )) == string::npos ) break;

      pos += 9; // account for the <a name="

      for (; pos < end && buf[pos] != '"'; ++pos)
        anchor += buf[pos];

      text.append(&buf[pos+2], &buf[end]);  // don't include "> at beginning of text

      if ( !anchor.empty() && !text.empty())
      {
        fo << "&nbsp;&nbsp;&nbsp;";
        for ( int i = 2; i < level; ++i )
          fo << "&nbsp;&nbsp;&nbsp;";
        fo << "<a href=\"#" << anchor << "\">" << text << "</a><br>" << endl;
      }
      pos = end;
    }
  }                                       

  return 0;
}