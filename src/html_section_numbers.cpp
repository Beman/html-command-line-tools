//  HTML section number generator  -----------------------------------------------------//

//  � Copyright Beman Dawes, 2007, 2012

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt)


//--------------------------------------------------------------------------------------// 

#include <iostream>
#include <string>
#include <cstring>
#include <boost/lexical_cast.hpp>
#include <boost/test/included/prg_exec_monitor.hpp>

using namespace std;

namespace
{
  int section[] = {0,0,0,0,0,0,0,0,0,0};

  const string spaces("  ");

  void generate_section_number(string& buf, string::size_type pos, int level)
  {
    // if there is an existing section number, erase it
    if (std::strchr("0123456789", buf[pos]))
    {
      while (std::strchr("0123456789. ", buf[pos]))
        buf.erase(pos, 1);
    }

    // insert the new section number
    int i;
    for (i = 1; i <= level; ++i)
    {
      if (i == level)
        ++section[i];
      if (!section[i])
        continue;
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

//--------------------------------------------------------------------------------------// 

int cpp_main( int argc, char* argv[] )
{
  if (argc > 1)
  {
    string arg(argv[1]);
    if (arg.find("--initial=") == 0)
    {
      --argc;
      arg.erase(0, 10);
      int level = 0;
      while (!arg.empty())
      {
        string vstr;
        while (!arg.empty() && arg[0] != '.')
        {
          vstr.push_back(arg[0]);
          arg.erase(0, 1);
        }
        if (!arg.empty())
          arg.erase(0, 1); // erase '.'
        section[++level] = atoi(vstr.c_str());
      }
    }
  }

  if (argc > 1)
  {
    cout << "Usage: html_section_numbers [--initial=n...]\n"
     "  Reads input from stdin\n"
     "  Writes output to stdout\n"
     "  Input lines copied to output, adding section numbers at start of\n"
     "  <h1>-<h9> heading elements.\n"
     "  Input lines containing \"generate-section-numbers=false\" or\n"
     "  \"generate-section-numbers=true\" cause indicated action. Default true.\n"
     " Option --initial=n... initializes section numbers,\n"
     "   for example, --initial=24.8.8"
            ;
     return 1;
  }

  string buf;
  string::size_type pos;
  bool generate = true;
  int prior_level = -1;  // -1 suppresses heading level warning for initial header

  while (getline(cin, buf))  // input each line
  {
    if (buf.find("generate-section-numbers=false") != string::npos)
    {
      generate = false;
    }
    else if (buf.find("generate-section-numbers") != string::npos)
    {
      generate = true;
      prior_level = -1;  // suppress heading level warning 
    }
    else if (generate && (pos = buf.find("<h")) < buf.size() - 4)
    {
      // if heading
      if (buf[pos+2] >= '1' && buf[pos+2] <= '9')
      {
        int level = buf[pos+2] - '0';
        if ((pos = buf.find(">", pos)) != string::npos)
        {
          generate_section_number(buf, pos+1, level);
          if (prior_level >= 0 && level > prior_level+1)
          {
            cerr << "Warning: heading level is more than one greater than prior level:\n";
            cerr << buf << '\n';
          }
          prior_level = level;
        }
      }
    }

    // output the line
    std::cout << buf << '\n';
  }

  return 0;
}
