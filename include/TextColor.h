/*! \file TextColor.h
 *  TextColor Class Declaration
 *  \verbinclude TextColor.h
 */
#ifndef TEXTCOLOR_H
#define TEXTCOLOR_H

#include <string>

namespace TextColor
{
  class TextColor
  {
    public:
      std::string RESET;
      std::string GREY;
      std::string RED;;
      std::string GREEN;
      std::string YELLOW;
      std::string BLUE;
      std::string PINK;
      std::string CYAN;
      std::string BOLD;

      TextColor();
      ~TextColor();
  }; // class TextColor
} // namespace TextColor

#endif // TEXTCOLOR_H
