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
#include <fstream>
#include <string>
#include <boost/detail/lightweight_main.hpp>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::getline;
