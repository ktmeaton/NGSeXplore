/*! \file ProgressLog.cpp
    ProgressLog Class Implementation.
    \verbinclude ProgressLog.cpp
*/

#include <iostream>
#include <map>                                        // Associative array
#include "ProgressLog.h"                              // Declaration File
#include "TextColor.h"                                // Unix shell colored output
#include "Utilities.h"                                // to_string compiler fix

namespace ProgressLog
{
    //------------------------------Constructor---------------------------------//
    ProgressLog::ProgressLog()
    {
        _total_num_records = 0;
        _processed_num_records = 0;
        ProgressLog::initPercentBoolMap();
    }

    //------------------------------Destructor----------------------------------//
    ProgressLog::~ProgressLog()
    {

    }

    void ProgressLog::printPercentBoolMap()
    {
        for ( int i = 0; i < 101; i += 10 )
        {
            std::cout << i << ": " << _percent_bool_map[i] << std::endl;
        }
    }

    //-------------------Initialize and Increment Progress Log------------------//
    void ProgressLog::initLog( int total_records )
    {
        _total_num_records = total_records;
    }

    void ProgressLog::initPercentBoolMap()
    {
        _percent_bool_map[0] = false;
        _percent_bool_map[10] = false;
        _percent_bool_map[20] = false;
        _percent_bool_map[30] = false;
        _percent_bool_map[40] = false;
        _percent_bool_map[50] = false;
        _percent_bool_map[60] = false;
        _percent_bool_map[70] = false;
        _percent_bool_map[80] = false;
        _percent_bool_map[90] = false;
        _percent_bool_map[100] = false;
    }

    void ProgressLog::incrementLog( int processed_records )
    {
        _processed_num_records += processed_records;
        _percent_processed = ( _processed_num_records / float( _total_num_records ) ) *
                        100;
        ProgressLog::updatePercentBoolMap();
    }

    void ProgressLog::updatePercentBoolMap()
    {
        for ( int i = 0; i < 101; i += 10 )
        {
            if ( !_percent_bool_map[i] && ( _percent_processed > i ||
                                            _percent_processed == i ) )
            {
                _percent_bool_map[i] = true;
                std::cout << _palette.RED << "[" << Utilities::to_string(i) << "% Completed]" << _palette.RESET << std::endl;
            }
        }
    }

} // namespace ProgressLog
