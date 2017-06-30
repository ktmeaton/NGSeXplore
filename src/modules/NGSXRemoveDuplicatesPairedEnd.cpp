/*! \file NGSXRemoveDuplicatesPairedEnd.cpp
 *  NGSXRemoveDuplicatesPairedEnd Module: Remove exact sequence duplicates.
 *  \verbinclude NGSXRemoveDuplicatesPairedEnd.cpp
 */

/*
 * NGSXRemoveDuplicatesPairedEnd: Implementation
 * Date: 2017-0630
 * Author : Katherine Eaton ktmeaton [at sign here ] gmail.com
 *
 */

//----------------------------System Include----------------------------------//
#include <iostream>									// Input and output to screen
#include <string>										// String
#include <map>											// Maps
#include <iomanip>									// Set Precision
#include <fstream>									// File input and output
#include <algorithm>								// To use the count function

//----------------------------Custom Include----------------------------------//
#include "FastQ.h"									// FastQ object
#include "TextColor.h"							// Unix shell colored output
#include "ProgressLog.h"						// ProgressLog Class

//---------------------------------Main---------------------------------------//
int main(int argc, char* argv[])
{
	//-------------------------------Usage--------------------------------------//
	const std::string usage=std::string(argv[0])+

			      " [options] "+"\n"+
			      "\nThis program takes a forward and reverse fastq file and removes exact sequence duplicates\n"+

			      "\n\tYou must specify two fastq files :\n"+
			      "\t\t"+"--fq1-in" +"\t\t"+"First fastq"+"\n"+
			      "\t\t"+"--fq2-in" +"\t\t"+"Second  fastq file"+"\n"+
			      "\n\tYou must specify two fastq files :\n"+
			      "\t\t"+"--fq1-out" +"\t\t"+"Output first fastq file "+"\n"+
			      "\t\t"+"--fq2-out" +"\t\t"+"Output second fastq file "+"\n"+
			      "\t\t"+"--stats" +"\t\t"+"Output stats file "+"\n\n";

		//-------------------------------Help Parsing-------------------------------//

    if( (argc== 1) ||
    	(argc== 2 && std::string(argv[1]) == "-h") ||
    	(argc== 2 && std::string(argv[1]) == "-help") ||
    	(argc== 2 && std::string(argv[1]) == "--help") ||
			(argc < 2) ||
			(argc > 50 )){
    	std::cout<< "Usage:" << std::endl;
    	std::cout<< "" << std::endl;
    	std::cout<< usage << std::endl;
    	return 1;
    }

		//-----------------------Implementation Variables-------------------------//

		// File Names
		std::string input_file_name_first_fastq;																		// Name of the first input fastq file
		std::string input_file_name_second_fastq;																		// Name of the second input fastq file
		std::string output_file_name_first_fastq;																		// Name of the first output fastq file
		std::string output_file_name_second_fastq;																	// Name of the second input fastq file
		std::string stats_file_name;																								// Name of the output stats file

		// Input file streams
		std::ifstream input_first_fastq_file;																							// Creates an input file stream for the first input fastq file
		std::ifstream input_second_fastq_file;																						// Creates an input file stream for the second input fastq file
		std::ifstream fastq_file_copy;																							// Copy for counting the number of lines

		// Output file streams
		std::ofstream output_first_fastq_file;																			// Creates an output file stream for the first fastq file
		std::ofstream output_second_fastq_file;																			// Creates an output file stream for the second fastq file
		std::ofstream stats_file;																										// Stats file

		// Fastq lines
		std::string current_line;																										// String to hold the line read in from the fastq file
		std::string temp_id;
		std::string temp_seq;
		std::string temp_qual;

		// Associative arrays
		std::map<std::string, FastQ::FastQ> map_unique_first_fastq;									// Map to hold unique first fastq sequences
		std::map<std::string, FastQ::FastQ> map_unique_second_fastq;								// Map to hold unique second fastq sequences

		// Colored text and progress log
		FastQ::FastQ temp_fastq;
		TextColor::TextColor Palette;																								// TextColor object for coloring text output
		ProgressLog::ProgressLog fastq_progress_log;																// ProgressLog object to store file processing progress.


		int total_num_lines;																												// Number of lines in the copy fastq file
		int total_num_records;																											// Number of sequences in the copy fastq file
		int final_num_seq;																													// Number of unique sequences in the final fastq file
		float percent_unique;																												// Percent of input sequences that are unique

		std::map<std::string, FastQ::FastQ>::iterator it;															// Map iterator

		//------------------------------Arg Parsing------------------------------//

		for(int i = 1;i < (argc - 1); i++){//all but last argument

			if(std::string(argv[i]) == "--fq1-in")
			{
	    	input_file_name_first_fastq = std::string(argv[i+1]);
	    	i++;
	    	continue;
			}

			else if(std::string(argv[i]) == "--fq2-in")
			{
	    	input_file_name_second_fastq = std::string(argv[i+1]);
	    	i++;
	    	continue;
			}

			else if(std::string(argv[i]) == "--fq1-out")
			{
				output_file_name_first_fastq = std::string(argv[i+1]);
				i++;
				continue;
			}

			else if(std::string(argv[i]) == "--fq2-out")
			{
				output_file_name_second_fastq = std::string(argv[i+1]);
				i++;
				continue;
			}

			else if(std::string(argv[i]) == "--stats")
			{
					stats_file_name = std::string(argv[i+1]);
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

		input_first_fastq_file.open(input_file_name_first_fastq.c_str());						// Open first input file (open requires parameter to be a const char*)
		input_second_fastq_file.open(input_file_name_second_fastq.c_str());					// Open second input file
		fastq_file_copy.open(input_file_name_first_fastq.c_str());									// Open copy input file
		output_first_fastq_file.open(output_file_name_first_fastq.c_str());					// Open first output file
		output_second_fastq_file.open(output_file_name_second_fastq.c_str());				// Open second output file
		stats_file.open(stats_file_name.c_str());

		// Check if files can be opened properly
		if (input_first_fastq_file.fail())
		{
			std::cerr << "ERROR: Cannot open input first fastq file: " << input_file_name_first_fastq << std::endl;
			return 1;
		}

		if (input_second_fastq_file.fail())
		{
			std::cerr << "ERROR: Cannot open input second fastq file: " << input_file_name_second_fastq << std::endl;
			return 1;
		}

		if (output_first_fastq_file.fail())
		{
			std::cerr << "ERROR: Cannot open output first fastq file: " << output_file_name_first_fastq << std::endl;
			return 1;
		}

		if (output_second_fastq_file.fail())
		{
			std::cerr << "ERROR: Cannot open output second fastq file: " << output_file_name_second_fastq << std::endl;
			return 1;
		}

		if (stats_file.fail())
		{
			std::cerr << "ERROR: Cannot open stats file." << stats_file_name << std::endl;
			return 1;
		}

		//----------------------------Begin Processing----------------------------//
		std::cout << Palette.GREEN << "\nBeginning the NGSX RemoveDuplicatesPairedEnd Module.\n" <<  Palette.RESET << std::endl;

		// Count the number of sequences in the input file (using the copy)
		std::cout << "Initializing files and counting the number of sequences (This may take a while)." << std::endl;
		total_num_lines = std::count(std::istreambuf_iterator<char>(fastq_file_copy), std::istreambuf_iterator<char>(), '\n') + 1;
		total_num_records = total_num_lines / 4;																// 4 lines for each sequence record
		fastq_progress_log.initLog(total_num_records);								  				// Initialize the progres log with the total number of records
		std::cout << "Input fastq file contains " << total_num_records << " sequences." << std::endl;

}
