/*! \file NGSXRemoveDuplicatesPairedEnd.cpp
    NGSXRemoveDuplicatesPairedEnd Module: Remove exact sequence duplicates.
    \verbinclude NGSXRemoveDuplicatesPairedEnd.cpp
*/

/*
    NGSXRemoveDuplicatesPairedEnd: Implementation
    Date: 2017-0630
    Author : Katherine Eaton ktmeaton [at sign here ] gmail.com

*/

//----------------------------System Include----------------------------------//
#include <iostream>                    // Input and output to screen
#include <string>                      // String
#include <map>                         // Maps
#include <iomanip>                     // Set Precision
#include <fstream>                     // File input and output
#include <algorithm>                   // To use the count function

//----------------------------Custom Include----------------------------------//
#include "FastQ.h"                     // FastQ object
#include "TextColor.h"                 // Unix shell colored output
#include "ProgressLog.h"               // ProgressLog Class

//---------------------------------Main---------------------------------------//
int main( int argc, char* argv[] )
{
    //-------------------------------Usage--------------------------------------//
    const std::string usage = std::string( argv[0] ) +

                    " [options] " + "\n" +
                    "\nThis program takes a forward and reverse fastq file and removes exact sequence duplicates\n"
                    +

                    "\n\tYou must specify two fastq files :\n" +
                    "\t\t" + "--fq1-in" + "\t\t" + "First fastq" + "\n" +
                    "\t\t" + "--fq2-in" + "\t\t" + "Second  fastq file" + "\n" +
                    "\n\tYou must specify two fastq files :\n" +
                    "\t\t" + "--fq1-out" + "\t\t" + "Output first fastq file " + "\n" +
                    "\t\t" + "--fq2-out" + "\t\t" + "Output second fastq file " + "\n" +
		    "\n\tYou must specify one text file for stats output:\n" +
                    "\t\t" + "--stats" + "\t\t\t" + "Output stats file " + "\n\n";

    //-------------------------------Help Parsing-------------------------------//

    if ( ( argc == 1 ) ||
                    ( argc == 2 && std::string( argv[1] ) == "-h" ) ||
                    ( argc == 2 && std::string( argv[1] ) == "-help" ) ||
                    ( argc == 2 && std::string( argv[1] ) == "--help" ) ||
                    ( argc < 11 ) ||
                    ( argc > 11 ) )
    {
        std::cout << "Usage:" << std::endl;
        std::cout << "" << std::endl;
        std::cout << usage << std::endl;
        return 1;
    }

    //-----------------------Implementation Variables-------------------------//

    // File Names
    std::string input_file_name_first_fastq;       // First input fastq
    std::string input_file_name_second_fastq;      // Second input fastq         
    std::string output_file_name_first_fastq;      // First output fastq                    
    std::string output_file_name_second_fastq;     // Second output fastq
    std::string stats_file_name;                   // Stats file

    // Input file streams
    std::ifstream input_first_fastq_file;          // Input first file stream   
    std::ifstream input_second_fastq_file;         // Input second file stream
    std::ifstream fastq_file_copy;                 // Copy for counting lines

    // Output file streams
    std::ofstream output_first_fastq_file;         // Output first file stream   
    std::ofstream output_second_fastq_file;        // Output second file stream
    std::ofstream stats_file;                      // Stats file

    // Fastq lines
    std::string current_line;                                                              
    std::string temp_id_first;
    std::string temp_seq_first;
    std::string temp_line3_first;
    std::string temp_qual_first;

    std::string temp_id_second;
    std::string temp_seq_second;
    std::string temp_line3_second;
    std::string temp_qual_second;

    std::string temp_seq_paired;

    // Associative arrays
    std::map<std::string, FastQ::FastQPaired> map_unique_paired;  // Map unique

    // Colored text and progress log
    FastQ::FastQ temp_fastq_first;
    FastQ::FastQ temp_fastq_second;
    FastQ::FastQPaired temp_fastq_paired;
    TextColor::TextColor Palette;                   // Colored text output
    ProgressLog::ProgressLog fastq_progress_log;    // Progress log 


    int total_num_lines;                            // Num lines in copy             
    int total_num_records;                          // Num fastq records
    int final_num_seq;                              // Num unique
    float percent_unique;                           // Percent of input

    std::map<std::string, FastQ::FastQPaired>::iterator it;      // Map iterator

    //------------------------------Arg Parsing------------------------------//

    for ( int i = 1; i < ( argc - 1 ); i++ ) //all but last argument (file)
    {

        if ( std::string( argv[i] ) == "--fq1-in" )
        {
            input_file_name_first_fastq = std::string( argv[i + 1] );
            i++;
            continue;
        }

        else if ( std::string( argv[i] ) == "--fq2-in" )
        {
            input_file_name_second_fastq = std::string( argv[i + 1] );
            i++;
            continue;
        }

        else if ( std::string( argv[i] ) == "--fq1-out" )
        {
            output_file_name_first_fastq = std::string( argv[i + 1] );
            i++;
            continue;
        }

        else if ( std::string( argv[i] ) == "--fq2-out" )
        {
            output_file_name_second_fastq = std::string( argv[i + 1] );
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

    input_first_fastq_file.open(input_file_name_first_fastq.c_str() );   
    input_second_fastq_file.open(input_file_name_second_fastq.c_str() );
    fastq_file_copy.open(input_file_name_first_fastq.c_str() );         
    output_first_fastq_file.open(output_file_name_first_fastq.c_str() );
    output_second_fastq_file.open(output_file_name_second_fastq.c_str() );
    stats_file.open( stats_file_name.c_str() );

    // Check if files can be opened properly
    if ( input_first_fastq_file.fail() )
    {
        std::cerr << "ERROR: Cannot open input first fastq file: " <<
                        input_file_name_first_fastq << std::endl;
        return 1;
    }

    if ( input_second_fastq_file.fail() )
    {
        std::cerr << "ERROR: Cannot open input second fastq file: " <<
                        input_file_name_second_fastq << std::endl;
        return 1;
    }

    if ( output_first_fastq_file.fail() )
    {
        std::cerr << "ERROR: Cannot open output first fastq file: " <<
                        output_file_name_first_fastq << std::endl;
        return 1;
    }

    if ( output_second_fastq_file.fail() )
    {
        std::cerr << "ERROR: Cannot open output second fastq file: " <<
                        output_file_name_second_fastq << std::endl;
        return 1;
    }

    if ( stats_file.fail() )
    {
        std::cerr << "ERROR: Cannot open stats file." << stats_file_name << std::endl;
        return 1;
    }

    //----------------------------Begin Processing----------------------------//
    std::cout << Palette.GREEN <<
                    "\nBeginning the NGSX RemoveDuplicatesPairedEnd Module.\n" <<  Palette.RESET <<
                    std::endl;

    // Count the number of sequences in the input file (using the copy)
    std::cout <<
                    "Initializing files and counting the number of sequences (This may take a while)."
                    << std::endl;
    total_num_lines = std::count( std::istreambuf_iterator<char>( fastq_file_copy ),
                                    std::istreambuf_iterator<char>(), '\n' ) + 1;
    total_num_records = total_num_lines /  4;           // 4 lines per record
    fastq_progress_log.initLog(total_num_records );     // Init log
    std::cout << "Input fastq file contains " << total_num_records << " sequences."
                    << std::endl;


    //---------------------------Find Unique Sequences------------------------//

    while ( std::getline( input_first_fastq_file, current_line ) )
    {
        // First fastq
        temp_id_first = current_line;                               // ID    
        std::getline( input_first_fastq_file,temp_seq_first );      // Sequence   
        std::getline( input_first_fastq_file,temp_line3_first );    // Ambiguous  
        std::getline( input_first_fastq_file, temp_qual_first );    // Quality 

        temp_fastq_first.setRecord( temp_id_first, temp_seq_first, temp_line3_first,
                        temp_qual_first );

        // Second fastq
        std::getline( input_second_fastq_file, temp_id_second );    // ID 
        std::getline( input_second_fastq_file, temp_seq_second );   // Sequence
        std::getline( input_second_fastq_file, temp_line3_second ); // Ambiguous
        std::getline( input_second_fastq_file, temp_qual_second );  // Quality

        temp_fastq_second.setRecord( temp_id_second, temp_seq_second, temp_line3_second,
                        temp_qual_second );

        temp_seq_paired = temp_seq_first + "}{" + temp_seq_second;

        temp_fastq_paired.setRecord( temp_fastq_first, temp_fastq_second );

        map_unique_paired[temp_seq_paired] = temp_fastq_paired;   // Add/replace

        // Completed reading 1 sequence record
        fastq_progress_log.incrementLog( 1 );
    }

    //---------------------------Write Unique Sequences-----------------------//
    std::cout << "Writing unique sequences to file." << std::endl;
    final_num_seq = 0;
    final_num_seq = 0;

    for ( it = map_unique_paired.begin(); it != map_unique_paired.end(); ++it )
    {
        // First output file
        output_first_fastq_file << it->second.getIDFirst() << std::endl;
        output_first_fastq_file << it->second.getSeqFirst() << std::endl;
        output_first_fastq_file << it->second.getLine3First() << std::endl;
        output_first_fastq_file << it->second.getQualFirst() << std::endl;

        output_second_fastq_file << it->second.getIDSecond() << std::endl;
        output_second_fastq_file << it->second.getSeqSecond() << std::endl;
        output_second_fastq_file << it->second.getLine3Second() << std::endl;
        output_second_fastq_file << it->second.getQualSecond() << std::endl;


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
