//  html_para_num.cpp  --------------------------------------------------------//

//  © Copyright Beman Dawes, 2014

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt)

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                      Generate html paragraph numbers                                 //
//                                                                                      //
//    Examples: <p para_num="12">, <pre para_num="12">                                  //
//    Paragraph numbering is reset by <h* elements                                      //
//                                                                                      //
//    Sample style to print the paragraph numbers in the left margin:                   //
//                                                                                      //
//    <style type = "text/css">                                                         //
//                                                                                      //
//    body { max-width : 7in;                                                           //
//           margin-left: 2em; } /* Make room for paragraph numbers. */                 //
//                                                                                      //
//    [para_num]::before {content: attr(para_num); float: left;                         //
//                        font-size: 70%; margin-left: -2.5em; width: 1.5em;            //
//                        text-align: right; }                                          //
//    ...                                                                               //
//    </style>                                                                          //
//                                                                                      //
//--------------------------------------------------------------------------------------// 

#include <iostream>
#include <string>
#include <cstring>  // for strchr
#include <boost/detail/lightweight_main.hpp>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::getline;

namespace
{
  bool header(const string& line)
  {
    string::size_type pos = line.find("<h");
    return pos != string::npos && (pos + 2) < line.size() && isdigit(line[pos+2]);
  }

  string::size_type find_start(const string& line, string::size_type start_pos)
    // returns pos for insertion start
  {
    string::size_type pos = line.find("<p>", start_pos);
    if (pos != string::npos)
      pos += 2;

    string::size_type pos2 = line.find("<p ", start_pos);
    if (pos2 < pos)
      pos = pos2 + 2;

    pos2 = line.find("<pre>", start_pos);
    if (pos2 < pos)
      pos = pos2 + 4;

    pos2 = line.find("<pre ", start_pos);
    if (pos2 < pos)
      pos = pos2 + 4;

    return pos;
  }

}  // unnamed namespace

//--------------------------------------------------------------------------------------// 

int cpp_main(int argc, char* argv[])
{
  if (argc > 1)
  {
    cout << "Usage: html_para_num <stdin >stdout\n"
      "  Input lines copied to output, adding paragraph numbers to <p> and\n"
      "  <pre> elements.\n"
      "  Input lines containing \"generate-paragraph-numbers=false\" or\n"
      "  \"generate-paragraph-numbers=true\" cause indicated action. Default true.\n"
      "  Warning: assumes heading, <p>, and <pre> markup is lower case.\n"
      "  Warning: assumes heading elements (<h1>, etc) never appear on same line\n"
      "  as <p> or <pre> elements."
      ;
    return 1;
  }

  string line;
  string::size_type pos;
  bool generate = true;
  int para_num = 0;

  while (getline(cin, line))  // input each line
  {
    if (line.find("generate-paragraph-numbers=false") != string::npos)
    {
      generate = false;
    }
    else if (line.find("generate-paragraph-numbers=true") != string::npos)
    {
      generate = true;
    }
    else if (header(line))
    {
      para_num = 0;
    }
    else if (generate)
    {
      // if there are already para_num's on line, erase them
      while ((pos = line.find(" para_num=\"")) != string::npos)
      {
        line.erase(pos, 11);
        while (std::strchr("0123456789\"", line[pos]))
          line.erase(pos, 1);
      }

      for (pos = 0; (pos = find_start(line, pos)) != string::npos; pos += 13)
      {
        line.insert(pos, string(" para_num=\"") + std::to_string(++para_num) + '"' );
      }
    }

    // output the line
    std::cout << line << '\n';
  }


  return 0;
}
