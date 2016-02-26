/*! \file NGSXRemoveDuplicates.cpp
 *  NGSXRemoveDuplicates Module: Remove exact sequence duplicates.
 *  \verbinclude NGSXRemoveDuplicates.cpp
 */

/*
 * NGSXRemoveDuplicates: Implementation
 * Date: 2015-01-12
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
	//---------------------------Help Variables---------------------------------//
	std::string usage = std::string("NGSXRemoveDuplicates.\n") +
									"Usage:\n" +
									"\t" +
									std::string(argv[0]) +
									"[input fastq file] [output fastq file] [stats file]\n";


	//-----------------------------------Help Message-------------------------------------//
	if ((argc == 1) ||
		(argc == 2 && std::string(argv[1]) == "-h") ||
		(argc == 2 && std::string(argv[1]) == "-help") ||
		(argc == 2 && std::string(argv[1]) == "--help") ||
		(argc < 4))
	{
		std::cerr << usage << std::endl;
		return 1;
	}

	//----------------------------Implementation Variables--------------------------------//

	std::string fastq_file_name;
	fastq_file_name = argv[1];											// Argument 1: Name of the input fastq file

	std::string unique_fastq_file_name;
	unique_fastq_file_name = argv[2];								// Argument 2: Name of the output unique fastq file

	std::string stats_file_name;										// Name of the stats file
	stats_file_name = argv[3];											// Argument 3: Name of the output stats file

	std::ifstream fastq_file;												// Creates an input file stream for the input fastq file
	std::ifstream fastq_file_copy;									// Copy for counting the number of lines

	std::ofstream unique_fastq_file;								// Creates an output file stream for the unique output fastq file
	std::ofstream stats_file;												// Stats file

	std::string current_line;													// String to hold the line read in from the fastq file
	std::string temp_id;
	std::string temp_seq;
	std::string temp_qual;

	std::map<std::string, FastQ::FastQ> map_unique_fastq;	// Map to hold unique sequences

	// Colored text and progress log
	FastQ::FastQ temp_fastq;
	TextColor::TextColor Palette;																						// TextColor object for coloring text output
	ProgressLog::ProgressLog fastq_progress_log;														// ProgressLog object to store file processing progress.


	int total_num_lines;																										// Number of lines in the copy fastq file
	int total_num_records;																									// Number of sequences in the copy fastq file
	int final_num_seq;
	float percent_unique;																										// Percent of input sequences that are unique

	std::map<std::string, FastQ::FastQ>::iterator it;															// Map iterator


	//----------------------------------Open Files---------------------------------------//
	fastq_file.open(fastq_file_name.c_str());						// Open input file (open requires parameter to be a const char*)
	fastq_file_copy.open(fastq_file_name.c_str());					// Open copy input file (open requires parameter to be a const char*)
	unique_fastq_file.open(unique_fastq_file_name.c_str());			// Open output file(open requires parameter to be a const char*)
	stats_file.open(stats_file_name.c_str());

	// Check if files can be opened properly
	if (fastq_file.fail())
	{
		std::cerr << "ERROR: Cannot open input fastq file: " << fastq_file_name << std::endl;
		return 1;
	}
	if (unique_fastq_file.fail())
	{
		std::cerr << "ERROR: Cannot open unique fastq file." << unique_fastq_file_name << std::endl;
		return 1;
	}
	if (stats_file.fail())
	{
		std::cerr << "ERROR: Cannot open stats file." << stats_file_name << std::endl;
		return 1;
	}

	//----------------------------Begin Processing------------------------------//
	std::cout << Palette.GREEN << "\nBeginning the NGSX RemoveDuplicates Module.\n" <<  Palette.RESET << std::endl;

	// Count the number of sequences in the input file (using the copy)
	std::cout << "Initializing files and counting the number of sequences (This may take a while)." << std::endl;
	total_num_lines = std::count(std::istreambuf_iterator<char>(fastq_file_copy), std::istreambuf_iterator<char>(), '\n') + 1;
	total_num_records = total_num_lines / 4;																// 4 lines for each sequence record
	fastq_progress_log.initLog(total_num_records);								  				// Initialize the progres log with the total number of records
	std::cout << "Input fastq file contains " << total_num_records << " sequences." << std::endl;


	//---------------------------Find Unique Sequences-----------------------------------//

	while (std::getline(fastq_file, current_line))
	{
		temp_id = current_line;																										// First line is the unique sequence ID
		std::getline(fastq_file, temp_seq);																			// Second line is the sequence bases
		std::getline(fastq_file, current_line);																		// Third line is the "+"
		std::getline(fastq_file, temp_qual);																		// Fourth line is the sequence quality

		temp_fastq.setRecord(temp_id, temp_seq, temp_qual);
		map_unique_fastq[temp_seq] = temp_fastq;	// Add or replace sequence and FastQ object in map

		// Completed reading 1 sequence record
		fastq_progress_log.incrementLog(1);
	}


	//---------------------------Write Unique Sequences-----------------------------------//
	std::cout << "Writing unique sequences to file." << std::endl;
	final_num_seq = 0;
	for(it = map_unique_fastq.begin(); it != map_unique_fastq.end(); ++it)
	{
		unique_fastq_file << it->second.getID() << std::endl;
		unique_fastq_file << it->second.getSeq() << std::endl;
		unique_fastq_file << "+" << std::endl;
		unique_fastq_file << it->second.getQual() << std::endl;

		// Completed writing 1 sequence record
		final_num_seq++;
	}

  percent_unique = final_num_seq / (float)total_num_records * 100;

	stats_file << "Total_Sequences\tUnique_Sequences\tPercent_Unique" << std::endl;
	stats_file << total_num_records << "\t" << final_num_seq << "\t" << std::setprecision(4) << percent_unique << std::endl;

	std::cout << "Out of: " << total_num_records << " sequences, NGSXRemoveDuplicates removed: " << total_num_records - final_num_seq << "." << std::endl;
	std::cout << "Percent Unique Sequences: " << percent_unique << "%" << std::endl;
	return 0;
}
