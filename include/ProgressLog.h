/*! \file ProgressLog.h
 *  ProgressLog Class Declaration.
 *  \verbinclude ProgressLog.h
 */


#pragma once

#include <string>
#include <map>                                        // Associative array
#include "TextColor.h"                                // Unix shell colored output

namespace ProgressLog
{
  class ProgressLog
  {
    //-------------------------------PRIVATE---------------------------------//
    private:
      int _total_num_records;
      int _processed_num_records;
      int _percent_processed;
      TextColor::TextColor _palette;

      std::map<int, bool> _percent_bool_map;


      void initPercentBoolMap();

      void updatePercentBoolMap();

    //-------------------------------PUBLIC----------------------------------//
    public:
    /**
       * \fn Constructor
       * \brief Constructs the ProgressLog object.
       * Constructs the ProgressLog with empty attributes.
       */
      ProgressLog();

        /** \fn Destructor */
      ~ProgressLog();

      void initLog(int total_records);

      void printPercentBoolMap();

      void incrementLog(int processed_records);

    }; // class ProgressLog


} // namespace ProgressLog

