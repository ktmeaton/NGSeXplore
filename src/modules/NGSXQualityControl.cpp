/*! \file NGSXQualityControl.cpp
 *  QualityControl Module: Read filtering based on quality scores and length.
 *  \verbinclude NGSXQualityControl.cpp
 * NGSXQualityControl: Implementation
 *\date $Date: 2016-0222
 *
 *Dated Edited: 2016-0302 - Added proper command-line argument parsing
 *
 *\author Author : Katherine Eaton ktmeaton [at sign here ] gmail.com
 *
 */

//----------------------------System Include----------------------------------//
#include <iostream>								// Input and output to screen
#include <string>								// String
#include <iomanip>								// Set Precision
#include <fstream>								// File input and output
#include <sstream>								// Argument to int
#include <algorithm>								// Count funtion

//----------------------------Custom Include----------------------------------//
#include "TextColor.h"								// Unix shell colored output
#include "ProgressLog.h"							// ProgressLog Class
#include "Patch.cpp"                						// Patch to fix the int to string compiler issue

//---------------------------------Main---------------------------------------//
int main(int argc, char* argv[])
{
	//---------------------------Help Variables---------------------------------//
	std::string usage = std::string("NGSX Quality Control Module. Filters by quality threshold, removes reads with N's and short reads. \n") +
									"Usage:\n" +
										"\t" +
										std::string(argv[0]) +
										" [--phred] [-q] [-p] [-l] [input fastq file] [output fastq file] [stats file]\n\n" +
									"Options:\n" +
										"\t" +
										"[--phred BASE]\t\t= 33 or 64\n" +
										"\t" +
										"[-q INT]\t\t= Minimum quality threshold\n" +
										"\t[-p FLOAT (0-1)]\t= Percentage of read that must meet minimum quality threshold (DEFAULT: 0.5)\n" +
										"\t[-l INT]\t\t= Minimum length to keep";

	//-----------------------------Help Message---------------------------------//
	if ((argc == 1) ||
		(argc == 2 && std::string(argv[1]) == "-h") ||
		(argc == 2 && std::string(argv[1]) == "-help") ||
		(argc == 2 && std::string(argv[1]) == "--help") ||
		(argc < 8))
	{
		std::cerr << usage << std::endl;
		return 1;
	}


	//----------------------------Variables: FILE-------------------------------//
	std::string fastq_file_name;
	fastq_file_name = argv[5];													// Argument 5: Name of the input fastq file


	std::string filter_fastq_file_name;
	filter_fastq_file_name = argv[6];										// Argument 6: Name of the output unique fastq file

	std::string stats_file_name;
	stats_file_name = argv[7];													// Argument 8: Name of the stats file

	std::ifstream fastq_file;														// Creates an input file stream for the input fastq file
	std::ifstream fastq_file_copy;											// Copy for counting the number of lines

	std::ofstream filter_fastq_file;										// Creates an output file stream for the filtered output fastq file
	std::ofstream stats_file;													  // Creates an output file stream for the stats file
	
	std::istringstream parameter;
	
	for(int i=1; i<(argc-1); i++)
	{ 
	    parameter(argv[i]);
	    std::cout << argv[i] << std::endl;
	}

	//----------------------------Variables: CONSTANT---------------------------//
	std::istringstream ss_phred(argv[1]);
  	int i_phred;
  	if (!(ss_phred >> i_phred))  std::cerr << "Invalid phred base. " << argv[1] << '\n';
	const int PHRED_BASE = i_phred;						// Phred base quality

  	std::istringstream ss_min_qual(argv[2]);
  	int i_min_qual;
  	if (!(ss_min_qual >> i_min_qual))  std::cerr << "Invalid minimum quality " << argv[2] << '\n';
  	const int MIN_QUAL = i_min_qual;

  	std::istringstream ss_prop_thresh(argv[3]);
  	float f_prop_thresh;
  	if (!(ss_prop_thresh >> f_prop_thresh))  std::cerr << "Invalid proportion threshold " << argv[3] << '\n';
	const float PROP_THRESHOLD = f_prop_thresh;				// Proportion of reads that must meet minimum quality threshold

  	std::istringstream ss_min_len(argv[4]);
  	int i_min_len;
  	if (!(ss_min_len >> i_min_len))  std::cerr << "Invalid minimum length. " << argv[4] << '\n';
  	const int MIN_LENGTH = i_min_len;

	//---------------------------Variables: STAT--------------------------------//
	int total_num_lines;																// Number of lines in the copy fastq file
	int total_num_records;															// Number of sequences in the copy fastq file
	int final_num_seq;																	// Number of sequences in filtered fastq file
	float percent_filtered;															// Percent of sequences filtered

	//-----------------------Variables: PROCESSSING-----------------------------//
	std::string current_line;														// String to hold the line read in from the fastq file

	TextColor::TextColor Palette;												// TextColor object for coloring text output
	ProgressLog::ProgressLog fastq_progress_log;				// ProgressLog object to store file processing progress.
	std::string temp_id;
	std::string temp_seq;
	std::string temp_qual;


	int read_length;
	int bases_above_threshold;													// Number of bases in current read that are above quality threshold
	int base_quality;																		// Quality of the current base
	bool found_N = false;						   									// boolean to indicate whether an N was found.
	bool reject_read = true;


	//----------------------------------Open Files---------------------------------------//
	fastq_file.open(fastq_file_name.c_str());													// Open input file (open requires parameter to be a const char*)
	fastq_file_copy.open(fastq_file_name.c_str());										// Open copy input file (open requires parameter to be a const char*)
	filter_fastq_file.open(filter_fastq_file_name.c_str());						// Open output file(open requires parameter to be a const char*)
	stats_file.open(stats_file_name.c_str());													// Open stats file(open requires parameter to be a const char*)

	// Check if files can be opened properly
	if (fastq_file.fail())
	{
		std::cerr << "ERROR: Cannot open input fastq file: " << fastq_file_name << std::endl;
		return 1;
	}
	if (filter_fastq_file.fail())
	{
		std::cerr << "ERROR: Cannot open filter fastq file." << filter_fastq_file_name << std::endl;
		return 1;
	}
	if (stats_file.fail())
	{
		std::cerr << "ERROR: Cannot open stats file." << stats_file_name << std::endl;
		return 1;
	}

	//----------------------------Begin Processing------------------------------//
        std::cout << Palette.GREEN << "\nBeginning the NGSX Quality Control Module.\n" <<  Palette.RESET << std::endl;

	// Count the number of sequences in the input file (using the copy)
	std::cout << "Initializing files and counting the number of sequences (This may take a while)." << std::endl;
	total_num_lines = std::count(std::istreambuf_iterator<char>(fastq_file_copy), std::istreambuf_iterator<char>(), '\n') + 1;
	total_num_records = total_num_lines / 4;												// 4 lines for each sequence record
	fastq_progress_log.initLog(total_num_records);								  // Initialize the progres log with the total number of records
	std::cout << "Input fastq file contains " << total_num_records << " sequences." << std::endl;







	//-------------------------Find Unique Sequences----------------------------//

	while (std::getline(fastq_file, current_line))
	{
		// Read in sequence entry
		temp_id = current_line;                                                     // First line is the unique sequence ID
		std::getline(fastq_file, temp_seq);						                        // Second line is the sequence bases
		std::getline(fastq_file, current_line);						                    // Read in Third Line, "+", skip
		std::getline(fastq_file, temp_qual);						                      // Fourth line is the sequence quality, RECORD FINISHED

		read_length = temp_seq.length();

		bases_above_threshold = 0;
		found_N = false;
		reject_read = true;

		// Check read length
		// This should intuitively be read_length > MIN_LENGTH  - 1
		// But c++ on linux thinks the sequence is 1 longer than the actual character count
		// Something to investigate in the future (byte storage for std::string
		if(read_length > (MIN_LENGTH))
		{
			for(int i = 0; i < read_length; i++)
			{
				// Check base quality
				base_quality = int(temp_qual[i]);
				if((base_quality - PHRED_BASE) > MIN_QUAL - 1) {bases_above_threshold++;}

				// Check for "N"
				if(temp_seq[i] == 'N' or temp_seq[i] == 'n')
				{
					found_N = true;
				}
			}


			// Check quality conditions
			if((bases_above_threshold > (read_length * PROP_THRESHOLD)) and not found_N)
			{
				reject_read = false;
			}
		}

		// Decide which output file to write read to
		if (reject_read == false)
		{
	  	// Write to filtered output file
      filter_fastq_file << temp_id << std::endl;
      filter_fastq_file << temp_seq << std::endl;
      filter_fastq_file << "+" << std::endl;
      filter_fastq_file << temp_qual << std::endl;

			// Completed writing 1 filtered sequence record
			final_num_seq++;
		}

		// Completed reading 1 sequence record
		fastq_progress_log.incrementLog(1);
	} // end while loop

	percent_filtered = final_num_seq / (float)total_num_records * 100;

	stats_file << "Total_Sequences\tSequences_Passing_Filter\tPercent_Passing_Filter" << std::endl;
	stats_file << total_num_records << "\t" << final_num_seq << "\t" << std::setprecision(4) << percent_filtered << std::endl;
	stats_file << argv[0] << " " << argv[1] << " " << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << " " << argv[6] << " " << argv[7] << " " << argv[8] << " " << std::endl;


	std::cout << "Out of: " << total_num_records << " sequences, NGSXQualityControl removed: " << total_num_records - final_num_seq << "." << std::endl;
	std::cout << "Percent Filtered Sequences: " << std::setprecision(4) << final_num_seq / (float)total_num_records * 100 << "%" << std::endl;

	std::cout << "\nOutput quality control statistics were written to: " << stats_file_name << "\n" << std::endl;

	std::cout << Palette.GREEN << "Completed the NGSX Quality Control Module.\n" <<  Palette.RESET << std::endl;
	return 0;
}
