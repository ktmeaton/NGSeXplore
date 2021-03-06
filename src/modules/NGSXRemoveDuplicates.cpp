/*! \file NGSXRemoveDuplicates.cpp
    NGSXRemoveDuplicates Module: Remove exact sequence duplicates.
    \verbinclude NGSXRemoveDuplicates.cpp
*/

/*
    NGSXRemoveDuplicates: Implementation
    Date: 2015-01-12
    Author : Katherine Eaton ktmeaton [at sign here ] gmail.com

*/

//----------------------------System Include----------------------------------//
#include <iostream>           // Input and output to screen
#include <string>             // String
#include <map>                // Maps
#include <iomanip>            // Set Precision
#include <fstream>            // File input and output
#include <algorithm>          // To use the count function

//----------------------------Custom Include----------------------------------//
#include "FastQ.h"            // FastQ object
#include "TextColor.h"        // Unix shell colored output
#include "ProgressLog.h"      // ProgressLog Class

//---------------------------------Main---------------------------------------//
int main( int argc, char* argv[] )
{
    //-----------------------------Usage--------------------------------------//
    const std::string usage = std::string( argv[0] ) +

                    " [options] " + "\n" +
                    "\nThis program takes a single fastq file and removes exact sequence duplicates\n"
                    +

                    "\n\tYou must specify one input fastq file :\n" +
                    "\t\t" + "--fq-in" + "\t\t\t" + "Input fastq" + "\n" +
                    "\n\tYou must specify one ouput fastq file :\n" +
                    "\t\t" + "--fq-out" + "\t\t" + "Output fastq file " + "\n" +
                    "\n\tYou must specify one text file for stats output:\n" +
                    "\t\t" + "--stats" + "\t\t\t" + "Output stats file " + "\n\n";

    //---------------------------Help Message---------------------------------//
    if ( ( argc == 1 ) ||
                    ( argc == 2 && std::string( argv[1] ) == "-h" ) ||
                    ( argc == 2 && std::string( argv[1] ) == "-help" ) ||
                    ( argc == 2 && std::string( argv[1] ) == "--help" ) ||
                    ( argc < 7 ) ||
                    ( argc > 7 ) )
    {
        std::cout << "Usage:" << std::endl;
        std::cout << "" << std::endl;
        std::cout << usage << std::endl;
    }

    //---------------------Implementation Variables--------------------------//

    std::string fastq_file_name;                   // Input Fastq
    std::string unique_fastq_file_name;            // Output Fastq
    std::string stats_file_name;                   // Stats file

    std::ifstream fastq_file;                      // Input file stream
    std::ifstream fastq_file_copy;                 // Copy for number lines

    std::ofstream unique_fastq_file;               // Output file stream
    std::ofstream stats_file;                      // Stats file

    std::string current_line;                      // String for line read
    std::string temp_id;
    std::string temp_seq;
    std::string temp_line3;
    std::string temp_qual;

    std::map<std::string, FastQ::FastQ> map_unique_fastq;   // Map of unique seq

    // Colored text and progress log
    FastQ::FastQ temp_fastq;
    TextColor::TextColor Palette;                  // Colored text output
    ProgressLog::ProgressLog fastq_progress_log;   // Progress log


    int total_num_lines;                           // Number of lines in copy
    int total_num_records;                         // Number of sequences
    int final_num_seq;
    float percent_unique;                          // Percent unique of input

    std::map<std::string, FastQ::FastQ>::iterator it;      // Map iterator

    //------------------------------Arg Parsing------------------------------//

    for ( int i = 1; i < ( argc - 1 ); i++ ) //all but last argument (file)
    {

        if ( std::string( argv[i] ) == "--fq-in" )
        {
            fastq_file_name = std::string( argv[i + 1] );
            i++;
            continue;
        }

        else if ( std::string( argv[i] ) == "--fq-out" )
        {
            unique_fastq_file_name = std::string( argv[i + 1] );
            i++;
            continue;
        }

        else if ( std::string( argv[i] ) == "--stats" )
        {
            stats_file_name = std::string( argv[i + 1] );
            i++;
            continue;
        }

        else
        {
            std::cerr << "Unknown option " << argv[i] << " exiting" << std::endl;
            return 1;
        }

    }


    //----------------------------------Open Files----------------------------//
    // Open requires parameter to be a const char*
    fastq_file.open(fastq_file_name.c_str() );                    
    fastq_file_copy.open(fastq_file_name.c_str() );               
    unique_fastq_file.open(unique_fastq_file_name.c_str() );      
    stats_file.open( stats_file_name.c_str() );

    // Check if files can be opened properly
    if ( fastq_file.fail() )
    {
        std::cerr << "ERROR: Cannot open input fastq file: " << fastq_file_name <<
                        std::endl;
        return 1;
    }

    if ( unique_fastq_file.fail() )
    {
        std::cerr << "ERROR: Cannot open unique fastq file." << unique_fastq_file_name
                        << std::endl;
        return 1;
    }

    if ( stats_file.fail() )
    {
        std::cerr << "ERROR: Cannot open stats file." << stats_file_name << std::endl;
        return 1;
    }

    //----------------------------Begin Processing----------------------------//
    std::cout << Palette.GREEN << "\nBeginning the NGSX RemoveDuplicates Module.\n"
                    <<  Palette.RESET << std::endl;

    // Count the number of sequences in the input file (using the copy)
    std::cout <<
                    "Initializing files and counting the number of sequences (This may take a while)."
                    << std::endl;
    total_num_lines = std::count( std::istreambuf_iterator<char>( fastq_file_copy ),
                                    std::istreambuf_iterator<char>(), '\n' ) + 1;
    total_num_records = total_num_lines / 4;         // 4 lines per record
    fastq_progress_log.initLog(total_num_records );  // Init progress log
    std::cout << "Input fastq file contains " << total_num_records << " sequences."
                    << std::endl;


    //---------------------------Find Unique Sequences------------------------//

    while ( std::getline( fastq_file, current_line ) )
    {
        temp_id = current_line;                        // Sequence ID
        std::getline( fastq_file, temp_seq );          // Sequence Bases
        std::getline( fastq_file, temp_line3 );        // Ambiguous
        std::getline( fastq_file, temp_qual );         // Sequence Quality

        temp_fastq.setRecord( temp_id, temp_seq, temp_line3, temp_qual );
        map_unique_fastq[temp_seq] = temp_fastq;       // Add or replace in map 
	
	// Completed reading 1 sequence record
        
	fastq_progress_log.incrementLog( 1 );
    }


    //---------------------------Write Unique Sequences-----------------------------------//
    std::cout << "Writing unique sequences to file." << std::endl;
    final_num_seq = 0;

    for ( it = map_unique_fastq.begin(); it != map_unique_fastq.end(); ++it )
    {
        unique_fastq_file << it->second.getID() << std::endl;
        unique_fastq_file << it->second.getSeq() << std::endl;
        unique_fastq_file << it->second.getLine3() << std::endl;
        unique_fastq_file << it->second.getQual() << std::endl;

        // Completed writing 1 sequence record
        final_num_seq++;
    }

    percent_unique = final_num_seq / ( float )total_num_records * 100;

    stats_file << "Total_Sequences\tUnique_Sequences\tPercent_Unique" << std::endl;
    stats_file << total_num_records << "\t" << final_num_seq << "\t" <<
                    std::setprecision( 4 ) << percent_unique << std::endl;

    std::cout << "Out of: " << total_num_records <<
                    " sequences, NGSXRemoveDuplicates removed: " << total_num_records -
                    final_num_seq << "." << std::endl;
    std::cout << "Percent Unique Sequences: " << percent_unique << "%" << std::endl;
    return 0;
}
