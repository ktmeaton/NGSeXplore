/*! \file TextColor.cpp
 *  TextColor Class Implementation.
 *  \verbinclude TextColor.cpp
 */

#include "TextColor.h"

namespace TextColor
{
  //------------------------------Constructor---------------------------------//
  TextColor::TextColor()
  {
    RESET = "\x1b[0m";
    GREY = "\x1b[30;1m";
    RED = "\x1b[31;1m";
    GREEN = "\x1b[32;1m";
    YELLOW = "\x1b[33;1m";
    BLUE = "\x1b[34;1m";
    PINK = "\x1b[35;1m";
    CYAN = "\x1b[36;1m";

    BOLD = "\x1b[37;1m";
  }

  //------------------------------Destructor----------------------------------//
  TextColor::~TextColor()
  {

  }

} // namespace TextColor
