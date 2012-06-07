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
  if ( argc < 2 )
  {
    cout << "Usage: " << argv[0] << " input-path\n";
    return 1;
  }

  fstream fi( argv[1] );
  if ( !fi )
  {
    cout << "Failed to open: " << argv[1] << "\n";
    return 1;
  }

  string buf;
  getline( fi, buf, '\0' ); // read entire file

  cout << "buf.size() is " << buf.size() << endl;

  string::size_type pos;
  string::size_type end;

  for ( pos = 0; (pos = buf.find( "<h", pos )) < buf.size(); ++pos )
  {
    // find start
    if ( buf[pos+2] < '2' || buf[pos+2] > '9' ) continue;
    int level = buf[pos+2] - '0';
    pos += 4;

    // find end
    if ( (end = buf.find( "</h", pos )) == string::npos ) break;

    string s( &buf[pos], &buf[end] );
    string text;
    string anchor;
    if ( (pos+10) < buf.size() && s.substr(0, 9) == "<a name=\"" )
    {
      anchor = s.substr( 9, s.find('"', 9 ) - 9 );
    }
    bool in_tag = false;
    bool end_tag = false;

    for ( unsigned i = 0; i < s.size(); ++i )
    {
      if ( s[i] == '<' )
      {
        in_tag = true;
        end_tag = s[i+1] == '/';
        continue;
      }
      if ( s[i] == '>' )
      {
        in_tag = false;
        continue;
      }
      if ( !in_tag ) text += s[i];
    }

    // generate the HTML
    if ( !anchor.empty() )
    {
      cout << "      ";
      for ( int i = 2; i < level; ++i ) cout << "&nbsp;&nbsp;&nbsp;";
      cout << "<a href=\"#" << anchor << "\">" << text << "</a><br>" << endl;
    }
  }                                       

  return 0;
}