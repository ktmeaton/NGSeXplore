/*! \file NGSXFastQStats.cpp
 * \brief Calculates length, quality, and GC content per read.
 *
 * \verbinclude FastQStats.cpp
 *
 * \date Date: 2016-0221
 *
 * \author Author : Katherine Eaton
 *
 * \version Version: 1.0.2
 *
 * Contact: ktmeaton [at sign here ] gmail.com
 *
 */

//----------------------------System Include----------------------------------//
#include <iostream>																// Input and output to screen
#include <string>									  							// String
#include <fstream>																// File input and output
#include <algorithm>															// Counting

//----------------------------Custom Include----------------------------------//
#include "FastQ.h"                  // FastQ Class
#include "ProgressLog.h"						// ProgressLog Class
#include "TextColor.h"						// Unix shell colored output


//--------------------------------Main----------------------------------------//
int main(int argc, char* argv[])
{
	//--------------------------Help Variables----------------------------------//
	std::string usage = std::string("NGSX FastQStats Module. Calculates length, quality, and GC content per read. \n\n") +
									"Usage:\n" +
										"\t" +
										std::string(argv[0]) +
										" [input fastq file] [output stats file]\n\n" +
									"Options:\n" +
										"\t" + "TBD" + "\n\n";


	//---------------------------Help Message-----------------------------------//
	if ((argc == 1) ||
		(argc == 2 && std::string(argv[1]) == "-h") ||
		(argc == 2 && std::string(argv[1]) == "-help") ||
		(argc == 2 && std::string(argv[1]) == "--help") ||
		(argc < 3))
	{
		std::cerr << usage << std::endl;
		return 1;
	}

  //-------------------------Variables: FILE----------------------------------//

  std::string input_fastq_file_name;
  input_fastq_file_name = argv[1];                                              /**Command Line Argument 1: Input fastq file. */
  std::ifstream input_fastq_file;
	std::ifstream input_fastq_file_copy;																					// Copy for counting the number of lines

  std::string output_stats_file_name;
  output_stats_file_name = argv[2];                                             /**Command Line Argument 2: Output stats file. */
  std::ofstream output_stats_file;

  int total_num_lines;																											  	// Number of lines in the copy fastq file
  int total_num_records;																												// Number of records in the copy fastq file

  std::string current_line;                                                     // Current line of the file that was read in.

	TextColor::TextColor Palette;																									// TextColor object for coloring text output
  ProgressLog::ProgressLog fastq_progress_log;																  // ProgressLog object to store file processing progress.
  FastQ::FastQ temp_fastq;								                                      // Temporary FastQ object to hold fastq information
  std::string temp_id;
  std::string temp_seq;
	std::string temp_line3;
  std::string temp_qual;





  //----------------------------Open Files------------------------------------//

  input_fastq_file.open(input_fastq_file_name.c_str());													// Open input fastq file (open requires parameter to be a const char*)
	input_fastq_file_copy.open(input_fastq_file_name.c_str());													// Open input fastq file copy
  output_stats_file.open(output_stats_file_name.c_str());                       // Open output stats file

  // Check if files can be opened properly
  if (input_fastq_file.fail())
  {
    std::cerr << "ERROR: Cannot open input fastq file: " << input_fastq_file_name << std::endl;
    return 1;
  }
  if (output_stats_file.fail())
	{
		std::cerr << "ERROR: Cannot open output stats file." << output_stats_file_name << std::endl;
		return 1;
	}

  output_stats_file << "Name\tLength\tGC.Content\tAverage.Quality\n";









  //----------------------------Begin Processing------------------------------//
  std::cout << Palette.GREEN << "\nBeginning the NGSX FastQStats Module.\n" <<  Palette.RESET << std::endl;



	// Count the number of sequences in the input file (using the copy)
	std::cout << "Initializing files and counting the number of sequences (This may take a while).\n" << std::endl;
	total_num_lines = std::count(std::istreambuf_iterator<char>(input_fastq_file_copy), std::istreambuf_iterator<char>(), '\n') + 1;
	total_num_records = total_num_lines / 4;																			// 4 lines for each sequence record
	fastq_progress_log.initLog(total_num_records);																// Initialize the progres log with the total number of records
	std::cout << "Input fastq file contains " << total_num_records << " sequences.\n" << std::endl;











  while (std::getline(input_fastq_file, current_line))
  {
    temp_id = current_line;                                                     // First line is the unique sequence ID
    std::getline(input_fastq_file, temp_seq);						                        // Second line is the sequence bases
    std::getline(input_fastq_file, temp_line3);						                    // Read in Third Line, "+", skip
    std::getline(input_fastq_file, temp_qual);						                      // Fourth line is the sequence quality, RECORD FINISHED

    temp_fastq.setRecord(temp_id, temp_seq, temp_line3, temp_qual);                         // Store record in the temp fastq object

    output_stats_file << temp_fastq.getID() << '\t' << temp_fastq.getLength() << '\t' << temp_fastq.getGC() << '\t' << temp_fastq.getAvQual() << std::endl;
		fastq_progress_log.incrementLog(1);																					// Increment progress log by 1 fastq record
  }

	std::cout << "\nOutput fastq statistics were written to: " << output_stats_file_name << "\n" << std::endl;

	std::cout << Palette.GREEN << "Completed the NGSX FastQStats Module.\n" <<  Palette.RESET << std::endl;
  return 0;
}
