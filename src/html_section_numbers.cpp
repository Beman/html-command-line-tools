//  HTML section numbers generator  ------------------------------------------//

//  © Copyright Beman Dawes, 2007, 2012

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt)


//----------------------------------------------------------------------------// 

#include <fstream>
#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/test/included/prg_exec_monitor.hpp>

using namespace std;

namespace
{
  int section[] = {0,0,1,1,1,1,1,1,1,1,1};

  void generate_section_number(string& buf, string::size_type pos, int level)
  {
    int i;
    for (i = 1; i <= level; ++i)
    {
      string number(boost::lexical_cast<string>(section[i]));
      buf.insert(pos, number);
      pos += number.size();
      if (i == level)
        ++section[i];
      else
        buf.insert(pos, 1, '.');

      while (section[++i] > 1)
        section[i] = 1;
    }
  }
}  // unnamed namespace

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

  for ( pos = 0; (pos = buf.find( "<h", pos )) < buf.size(); ++pos )
  {
    // find start
    if ( buf[pos+2] < '2' || buf[pos+2] > '9' ) continue;
    int level = buf[pos+2] - '0';
    pos += 4;
    generate_section_number(buf, pos, level);
  }

  // generate the output file
  std::cout << buf;

  return 0;
}