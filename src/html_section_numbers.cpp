//  HTML section numbers generator  ------------------------------------------//

//  � Copyright Beman Dawes, 2007, 2012

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt)


//----------------------------------------------------------------------------// 

#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/test/included/prg_exec_monitor.hpp>

using namespace std;

namespace
{
  int section[] = {0,0,0,0,0,0,0,0,0,0};

  const string spaces("  ");

  void generate_section_number(string& buf, string::size_type pos, int level)
  {
    int i;
    for (i = 1; i <= level; ++i)
    {
      if (i == level)
        ++section[i];
      string number(boost::lexical_cast<string>(section[i]));
      buf.insert(pos, number);
      pos += number.size();
      if (i == level)
        buf.insert(pos, spaces);
      else
      {
        buf.insert(pos, 1, '.');
        ++pos;
      }
    }

    // reset any higher section numbers to 0
    for (; i < sizeof(section)/sizeof(int) && section[i] > 0; ++i)
      section[i] = 0;
  }
}  // unnamed namespace

//----------------------------------------------------------------------------// 

int cpp_main( int argc, char* argv[] )
{
  if (argc > 1)
  {
    cout << "Usage: html_section_numbers\n"
            "  Reads input from stdin\n"
            "  Writes output to stdout\n"
            "  Input lines copied to output, adding section numbers at start of\n"
            "  <h1>-<h9> heading elements.\n"
            "  Input lists containing \"generate-section-numbers=false\" or\n"
            "  \"generate-section-numbers=true\" cause indicated action. Default true.\n"
            ;
     return 1;
  }

  string buf;
  string::size_type pos;
  bool generate = true;
  int prior_level = 0;

  while (getline(cin, buf))  // input each line
  {
    if (buf.find("generate-section-numbers=false") != string::npos)
    {
      generate = false;
      continue;
    }
    else if (buf.find("generate-section-numbers=true") != string::npos)
    {
      generate = true;
      continue;
    }
    else if (generate && (pos = buf.find("<h")) < buf.size() - 4)
    {
      // if heading
      if (buf[pos+2] >= '1' && buf[pos+2] <= '9')
      {
        int level = buf[pos+2] - '0';
        pos += 4;
        generate_section_number(buf, pos, level);
        if (level > prior_level+1)
          cerr << "warning: heading level is more than one greater than prior level\n";
        prior_level = level;
      }
    }

    // output the line
    std::cout << buf << '\n';
  }


  return 0;
}