/*! \file NGSXRemoveDuplicates.cpp
 *  NGSXRemoveDuplicates Module: Remove exact sequence duplicates.
 *  \verbinclude NGSXRemoveDuplicates.cpp
 */

/*
 * NGSXRemoveDuplicates: Implementation
 * Date: 2015-01-12
 * Edited: 2016-05-16 - Added functionality for both fasta and fastq files.
 * Author : Katherine Eaton ktmeaton [at sign here ] gmail.com
 *
 */

//----------------------------System Include----------------------------------//
#include <iostream>										// Input and output to screen
#include <string>										// String
#include <cstring>										// For strcmp
#include <map>											// Maps
#include <iomanip>										// Set Precision
#include <fstream>										// File input and output
#include <algorithm>										// To use the count function

//----------------------------Custom Include----------------------------------//
#include "FastQ.h"										// FastQ object
#include "TextColor.h"										// Unix shell colored output
#include "ProgressLog.h"									// ProgressLog Class

//---------------------------------Main---------------------------------------//
int main(int argc, char* argv[])
{
	//---------------------------Help Variables---------------------------------//
	std::string usage = std::string("NGSXRemoveDuplicates.\n") +
									"Usage:\n" +
									"\t" +
									std::string(argv[0]) +
									" --fasta [input fastq file] --fastq [input fastq file] --output [output deduplicated sequence file] --stats [stats file]\n\n" +
									"This program removes exact sequence duplicates.\n\n" +
									"\tYou must provide as input a fasta OR fastq file:\n" +
									"\t\t--fastq\t\tInput fastq file.\n" +
									"\t\t--fasta\t\tInput fasta file.\n\n" +
									"\tThe output file will be the same format as the input:\n" +
									"\t\t--output\t\t Output fasta/fastq file.\n\n" +
									"\tSummary statistics will be saved in a text file:\n" +
									"\t\t--stats\t\t Output stats text file.\n";


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
	
	std::string fastq_file_name;								// Name of the input fastq file
	std::string fasta_file_name;								// Name of the input fasta file
	std::string output_file_name;								// Name of the output unique sequence file
	std::string stats_file_name;								// Name of the stats file

	std::ifstream fastq_file;								// Creates an input file stream for the input fastq file
	std::ifstream fasta_file;								// Creates an input file stream for the input fasta file	
	std::ifstream file_copy;								// Copy for counting the number of lines

	std::ofstream output_file;								// Creates an output file stream for the unique output fastq file
	std::ofstream stats_file;								// Creates an output file stream for the stats file

	std::string current_line;								// String to hold the line read in from the fastq file
	std::string temp_id;
	std::string temp_seq;
	std::string temp_qual;

	std::map<std::string, FastQ::FastQ> map_unique_fastq;					// Map to hold unique fastq sequences
	std::map<std::string, FastQ::FastQ> map_unique_fasta;					// Map to hold unique fasta sequences

	// Colored text and progress log
	FastQ::FastQ temp_fastq;
	FastQ::FastQ temp_fasta;
	TextColor::TextColor Palette;								// TextColor object for coloring text output
	ProgressLog::ProgressLog progress_log;							// ProgressLog object to store file processing progress.


	int total_num_lines;																										// Number of lines in the copy fastq file
	int total_num_records;																									// Number of sequences in the copy fastq file
	int final_num_seq;
	float percent_unique;																										// Percent of input sequences that are unique

	std::map<std::string, FastQ::FastQ>::iterator it;															// Map iterator
	
	bool fastqFormat;
	bool fastaFormat;

	//---------------------------------Argument Parsing-----------------------------------//
	for(int i=1;i<(argc);i++)								// Iterate through each argument (ignore first)
	{
		if(strcmp(argv[i], "--fastq") == 0 )						
		{
	    		fastq_file_name = std::string(argv[i+1]);				// Store the fastq file name
			fastqFormat=true;
	    		i++;
			continue;
		}
		
		if(strcmp(argv[i], "--fasta") == 0 )					
		{
	    		fasta_file_name = std::string(argv[i+1]);				// Store the fasta file name
			fastaFormat=true;
	    		i++;
			continue;
		}
		
		if(strcmp(argv[i], "--output") == 0 )
		{
	    		output_file_name = std::string(argv[i+1]);				// Store the output file name
	    		i++;
			continue;
		}	
		
		if(strcmp(argv[i], "--stats") == 0 )
		{
	    		stats_file_name = std::string(argv[i+1]);				// Store the stats file name
	    		i++;
			continue;
		}	
	
		std::cerr<< "Unknown option " << argv[i] << " exiting." << std::endl;		// Exit if unknown option
		return 1;		
	}
	
	//------------------------------------Input Check------------------------------------//
	
	if (fastqFormat == true && fastaFormat == true)
	{
		std::cerr<< "Cannot specify both an input fasta and fastq file." << std::endl;
		std::cerr<< "User specified both fastq file " << fastq_file_name << " and fasta file " << fasta_file_name << std::endl;
		return 1;		
	}

	//----------------------------------Open Files---------------------------------------//
	
	stats_file.open(stats_file_name.c_str());						// Generic stats file regardless of fasta/fastq input
	output_file.open(output_file_name.c_str());						// Generic output file regardless of fasta/fastq input

	// Check if files can be opened properly
	if (output_file.fail())
	{
		std::cerr << "ERROR: Cannot open output file." << output_file_name << std::endl;
		return 1;
	}
	if (stats_file.fail())
	{
		std::cerr << "ERROR: Cannot open stats file." << stats_file_name << std::endl;
		return 1;
	}


	//----------------------------------Fastq Format------------------------------------//
	if (fastqFormat)
	{
		fastq_file.open(fastq_file_name.c_str());						// Open input file (open requires parameter to be a const char*)
		file_copy.open(fastq_file_name.c_str());						// Open copy input file (open requires parameter to be a const char*)

		// Check if input fastq file can be opened properly
		if (fastq_file.fail())
		{
			std::cerr << "ERROR: Cannot open input fastq file: " << fastq_file_name << std::endl;
			return 1;
		}
	
		//----------------------------Begin Processing------------------------------//
		std::cout << Palette.GREEN << "\nBeginning the NGSX RemoveDuplicates Module.\n" <<  Palette.RESET << std::endl;

		// Count the number of sequences in the input file (using the copy)
		std::cout << "Initializing files and counting the number of sequences (This may take a while)." << std::endl;
		total_num_lines = std::count(std::istreambuf_iterator<char>(file_copy), std::istreambuf_iterator<char>(), '\n') + 1;
		total_num_records = total_num_lines / 4;								// 4 lines for each fastq sequence record
		progress_log.initLog(total_num_records);								// Initialize the progres log with the total number of records
		std::cout << "Input fastq file contains " << total_num_records << " sequences." << std::endl;
		
		//---------------------------Find Unique Sequences-----------------------------------//

		while (std::getline(fastq_file, current_line))
		{
			temp_id = current_line;																										// First line is the unique sequence ID
			std::getline(fastq_file, temp_seq);						// Second line is the sequence bases
			std::getline(fastq_file, current_line);						// Third line is the "+"
			std::getline(fastq_file, temp_qual);						// Fourth line is the sequence quality

			temp_fastq.setRecord(temp_id, temp_seq, temp_qual);
			map_unique_fasta[temp_seq] = temp_fastq;					// Add or replace sequence and FastQ object in map

			// Completed reading 1 sequence record
			progress_log.incrementLog(1);
		}
		
		//---------------------------Write Unique Sequences-----------------------------------//
		std::cout << "Writing unique sequences to file." << std::endl;
		final_num_seq = 0;
		for(it = map_unique_fastq.begin(); it != map_unique_fastq.end(); ++it)
		{
			output_file << it->second.getID() << std::endl;
			output_file << it->second.getSeq() << std::endl;
			output_file << "+" << std::endl;
			output_file << it->second.getQual() << std::endl;

			// Completed writing 1 sequence record
			final_num_seq++;
		}

	}
	
	//----------------------------------Fasta Format------------------------------------//
	if (fastaFormat)
	{
		fasta_file.open(fasta_file_name.c_str());						// Open input fast file (open requires parameter to be a const char*)
		file_copy.open(fasta_file_name.c_str());						// Open copy input fasta file (open requires parameter to be a const char*)

		// Check if files can be opened properly
		if (fasta_file.fail())
		{
			std::cerr << "ERROR: Cannot open input fasta file: " << fasta_file_name << std::endl;
			return 1;
		}
	
		//----------------------------Begin Processing------------------------------//
		std::cout << Palette.GREEN << "\nBeginning the NGSX RemoveDuplicates Module.\n" <<  Palette.RESET << std::endl;

		// Count the number of sequences in the input file (using the copy)
		std::cout << "Initializing files and counting the number of sequences (This may take a while)." << std::endl;
		total_num_lines = std::count(std::istreambuf_iterator<char>(file_copy), std::istreambuf_iterator<char>(), '\n') + 1;
		total_num_records = total_num_lines / 2;									// 2 lines for each fasta sequence record
		progress_log.initLog(total_num_records);								  	// Initialize the progres log with the total number of records
		std::cout << "Input fasta file contains " << total_num_records << " sequences." << std::endl;

		//---------------------------Find Unique Sequences-----------------------------------//

		while (std::getline(fasta_file, current_line))
		{
			temp_id = current_line;								// First line is the unique sequence ID
			std::getline(fasta_file, temp_seq);						// Second line is the sequence bases																	// Third line is the "+"
			temp_qual = "";									// Set quality to null for fasta

			temp_fasta.setRecord(temp_id, temp_seq, temp_qual);
			map_unique_fastq[temp_seq] = temp_fasta;					// Add or replace sequence and FastQ object in map

			// Completed reading 1 sequence record
			progress_log.incrementLog(1);
		}

	}

  	percent_unique = final_num_seq / (float)total_num_records * 100;

	stats_file << "Total_Sequences\tUnique_Sequences\tPercent_Unique" << std::endl;
	stats_file << total_num_records << "\t" << final_num_seq << "\t" << std::setprecision(4) << percent_unique << std::endl;

	std::cout << "Out of: " << total_num_records << " sequences, NGSXRemoveDuplicates removed: " << total_num_records - final_num_seq << "." << std::endl;
	std::cout << "Percent Unique Sequences: " << percent_unique << "%" << std::endl;
	return 0;
}
