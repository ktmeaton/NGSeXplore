/*! \file NGSXQualityControlPairedEnd.cpp
 *  NGSXQualityControlPairedEnd Module: Read filtering for paired-end data based on quality scores and length fdsafjdsalkfjdl.
 *  \verbinclude NGSXQualityControlPairedEnd.cpp
 *
 * NGSXQualityControlPairedEnd: Implementation
 *\date Date: 2016-0222
 *
 * Date Edited: 2016-0302 -	Added proper command-line parsing
 *
 *\author Author : Katherine Eaton
 *
 * Contact: ktmeaton [at sign here ] gmail.com
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
#include "Patch.cpp"            						// Patch to fix the int to string compiler issue

//---------------------------------Main---------------------------------------//
int main(int argc, char* argv[])
{
	//---------------------------Help Variables---------------------------//
	std::string usage = std::string("NGSX Quality Control Paired End Module. Filters by quality threshold for paired-end reads, removes reads with N's and short reads. \n") +
					"Usage:\n" +
					"\t" +
					std::string(argv[0]) +
					" [--phred] [-q] [-p] [-l] [input fastq 1 file] [input fastq 2 file] [output fastq 1 file] [output fastq 2 file] [stats file]\n\n" +
					"Options:\n" +
					"\t" +
					"[--phred BASE]\t\t= 33 or 64\n" +
					"\t" +
					"[-q INT]\t\t= Minimum quality threshold\n" +
					"\t[-p FLOAT (0-1)]\t= Percentage of read that must meet minimum quality threshold\n" +
					"\t[-l INT]\t\t= Minimum length to keep";

	//-----------------------------Help Message---------------------------------//
	if ((argc == 1) ||
		(argc == 2 && std::string(argv[1]) == "-h") ||
		(argc == 2 && std::string(argv[1]) == "-help") ||
		(argc == 2 && std::string(argv[1]) == "--help") ||
		(argc < 10))
	{
		std::cerr << usage << std::endl;
		return 1;
	}


	//----------------------------Variables: FILE-------------------------------//
	std::string fastq_1_file_name;
	fastq_1_file_name = argv[5];							// Argument 5: Name of the input fastq 1 file

	std::string fastq_2_file_name;
	fastq_2_file_name = argv[6];							// Argument 6: Name of the input fastq 2 file

	std::string filter_fastq_1_file_name;
	filter_fastq_1_file_name = argv[7];						// Argument 7: Name of the output unique fastq 1 file

	std::string filter_fastq_2_file_name;
	filter_fastq_2_file_name = argv[8];						// Argument 8: Name of the output unique fastq 2 file

	std::string stats_file_name;
	stats_file_name = argv[9];							// Argument 9: Name of the stats file


	std::ifstream fastq_1_file;							// Creates an input file stream for the input fastq 1 file
	std::ifstream fastq_1_file_copy;						// Copy for counting the number of lines

	std::ifstream fastq_2_file;							// Creates an input file stream for the input fastq 2 file

	std::ofstream filter_fastq_1_file;						// Creates an output file stream for the filtered output fastq 1 file
	std::ofstream filter_fastq_2_file;						// Creates an output file stream for the filtered output fastq 2 file
	std::ofstream stats_file;							// Creates an output file stream for the stats file

	
	std::string parameter;								// String to store current command-line argument
	std::string str_phred;								// Temp string to store input phred base
	std::string str_min_qual;							// Temp string to store input minimum quality
	std::string str_prop_thresh;							// Temp string to store input proportion threshold
	std::string str_min_len;							// Temp string to store input minimum length
	
	
	//---------------------Variables: ARGUMENT PARSING---------------------------//
	for(int i=1; i<(argc-5); i++) // Look through all command-line arguments except last 5
	{ 
	    parameter = argv[i];
	    std::cout << parameter << std::endl;
	    
	    if(parameter == "--phred") 
	    {
	    	str_phred = argv[i+1]; 
	    	i++; 
	    	continue;
	    }
	    else if(parameter == "-q") 
	    {
	    	str_min_qual = argv[i+1]; 
	    	i++; 
	    	continue;
	    }
	    else if(parameter == "-p") 
	    {
	    	str_prop_thresh = argv[i+1]; 
	    	i++; 
	    	continue;
	    }
	    else if(parameter == "-l") 
	    {
	    	str_min_len = argv[i+1]; 
	    	i++; 
	    	continue;
	    }
	    else std::cerr<<"ERROR: Unknown argument "<< argv[i] <<" exiting"<< std::endl;
	    return 1;	
	}

	//----------------------------Variables: CONSTANT----------------------------//
	
	// Phred Base
	std::istringstream ss_phred(str_phred);
  	int i_phred;
  	if (!(ss_phred >> i_phred)) std::cerr << "Invalid phred base. " << str_phred << std::endl;
	const int PHRED_BASE = i_phred;	
	
	// Minimum Quality
  	std::istringstream ss_min_qual(str_min_qual);
  	int i_min_qual;
  	if (!(ss_min_qual >> i_min_qual))  std::cerr << "Invalid minimum quality " << str_min_qual << '\n';
  	const int MIN_QUAL = i_min_qual;

        // Proportion Threshold
  	std::istringstream ss_prop_thresh(str_prop_thresh);
  	float f_prop_thresh;
  	if (!(ss_prop_thresh >> f_prop_thresh))  std::cerr << "Invalid proportion threshold " << str_prop_thresh << '\n';
	const float PROP_THRESHOLD = f_prop_thresh;					// Proportion of reads that must meet minimum quality threshold

	// Minimum Length
  	std::istringstream ss_min_len(str_min_len);
  	int i_min_len;
  	if (!(ss_min_len >> i_min_len))  std::cerr << "Invalid minimum length. " << str_min_len << '\n';
  	const int MIN_LENGTH = i_min_len;

	//---------------------------Variables: STAT--------------------------------//
	int total_num_lines;								// Number of lines in the copy fastq file
	int total_num_records;								// Number of sequences in the copy fastq file
	int final_num_seq;								// Number of sequences in filtered fastq file
	float percent_filtered;								// Percent of sequences filtered

	//-----------------------Variables: PROCESSSING-----------------------------//
	// Current line of file
	std::string current_line_1;							// String to hold the line read in from the fastq 1 file
	std::string current_line_2;							// String to hold the line read in from the fastq 2 file

	// Colored text and progress log
	TextColor::TextColor Palette;							// TextColor object for coloring text output
	ProgressLog::ProgressLog fastq_progress_log;					// ProgressLog object to store file processing progress.

	// Temporary id holder
	std::string temp_id_1;
	std::string temp_id_2;

	// Temporary sequence holder
	std::string temp_seq_1;
	std::string temp_seq_2;

	// Temporary quality holder
	std::string temp_qual_1;
	std::string temp_qual_2;

	int read_length_1;
	int read_length_2;

	int bases_above_threshold_1;							// Number of bases in current read that are above quality threshold
	int bases_above_threshold_2;							// Number of bases in current read that are above quality threshold

	int base_quality_1;								// Quality of the current base
	int base_quality_2;								// Quality of the current base

	bool found_N = false;						   		// boolean to indicate whether an N was found.
	bool reject_read = true;


	//----------------------------------Open Files-------------------------------//
	// open requires parameter to be a const char*
	fastq_1_file.open(fastq_1_file_name.c_str());					// Open input fastq 1 file
	fastq_2_file.open(fastq_2_file_name.c_str());					// Open input fastq 2 file

	fastq_1_file_copy.open(fastq_1_file_name.c_str());				// Open copy input file

	filter_fastq_1_file.open(filter_fastq_1_file_name.c_str());			// Open output filter file 1
	filter_fastq_2_file.open(filter_fastq_2_file_name.c_str());			// Open output filter file 1

	stats_file.open(stats_file_name.c_str());					// Open stats file

	// Check if files can be opened properly
	if (fastq_1_file.fail())
	{
		std::cerr << "ERROR: Cannot open input fastq 1 file: " << fastq_1_file_name << std::endl;
		return 1;
	}
	if (fastq_2_file.fail())
	{
		std::cerr << "ERROR: Cannot open input fastq 2 file: " << fastq_2_file_name << std::endl;
		return 1;
	}
	if (filter_fastq_1_file.fail())
	{
		std::cerr << "ERROR: Cannot open filter fastq 1 file." << filter_fastq_1_file_name << std::endl;
		return 1;
	}
	if (filter_fastq_2_file.fail())
	{
		std::cerr << "ERROR: Cannot open filter fastq 2 file." << filter_fastq_2_file_name << std::endl;
		return 1;
	}
	if (stats_file.fail())
	{
		std::cerr << "ERROR: Cannot open stats file." << stats_file_name << std::endl;
		return 1;
	}

	//----------------------------Begin Processing------------------------------//
  std::cout << Palette.GREEN << "\nBeginning the NGSX QualityControl Module.\n" <<  Palette.RESET << std::endl;

	// Count the number of sequences in the input file (using the copy)
	std::cout << "Initializing files and counting the number of sequences (This may take a while)." << std::endl;
	total_num_lines = std::count(std::istreambuf_iterator<char>(fastq_1_file_copy), std::istreambuf_iterator<char>(), '\n') + 1;
	total_num_records = total_num_lines / 4;					// 4 lines for each sequence record
	fastq_progress_log.initLog(total_num_records);					// Initialize the progres log with the total number of records
	std::cout << "Input fastq file contains " << total_num_records << " sequences." << std::endl;


	//-------------------------Find Unique Sequences----------------------------//

	while (std::getline(fastq_1_file, current_line_1))
	{
		// Also get fastq 2 line
		std::getline(fastq_2_file, current_line_2);

		// Read in sequence identifiers
		temp_id_1 = current_line_1;                                             // First line is the unique sequence ID read 1
		temp_id_2 = current_line_2;                                             // First line is the unique sequence ID read 2

		// Read in sequence nucleotides
		std::getline(fastq_1_file, temp_seq_1);					// Second line is the sequence bases
		std::getline(fastq_2_file, temp_seq_2);					// Second line is the sequence bases

		// Skip Third line "+"
		std::getline(fastq_1_file, current_line_1);
		std::getline(fastq_2_file, current_line_2);

		// Read in sequence qualities
		std::getline(fastq_1_file, temp_qual_1);				// Fourth line is the sequence quality, read 1, RECORD FINISHED
		std::getline(fastq_2_file, temp_qual_2);				// Fourth line is the sequence quality, read 2, RECORD FINISHED

		read_length_1 = temp_seq_1.length();
		read_length_2 = temp_seq_2.length();
		bases_above_threshold_1 = 0;
		bases_above_threshold_2 = 0;
		found_N = false;
		reject_read = true;

		// Check read length
		// This should intuitively be read_length > MIN_LENGTH  - 1
		// But c++ on linux thinks the sequence is 1 longer than the actual character count
		// Something to investigate in the future (byte storage for std::string)
		if(read_length_1 > (MIN_LENGTH))
		{
			// Analyze read 1 base qualities
			for(int i = 0; i < read_length_1; i++)
			{
				// Check base quality
				base_quality_1 = int(temp_qual_1[i]);
				if((base_quality_1 - PHRED_BASE) > MIN_QUAL - 1) {bases_above_threshold_1++;}
				// Check for "N"
				if(temp_seq_1[i] == 'N' or temp_seq_1[i] == 'n'){found_N = true;}
			}
		}

		if(read_length_2 > (MIN_LENGTH))
		{
			// Analyze read 2 base qualities
			for(int i = 0; i < read_length_2; i++)
			{
				// Check base quality
				base_quality_2 = int(temp_qual_2[i]);
				if((base_quality_2 - PHRED_BASE) > MIN_QUAL - 1) {bases_above_threshold_2++;}
				// Check for "N"
				if(temp_seq_2[i] == 'N' or temp_seq_2[i] == 'n'){found_N = true;}
			}
		}

		// Check quality conditions
		if(!found_N
		    && (bases_above_threshold_1 > (read_length_1 * PROP_THRESHOLD))
       		    && (bases_above_threshold_2 > (read_length_2 * PROP_THRESHOLD)))
		    { reject_read = false;}

		// Write to output filtered files if the read passes quality control
		if (reject_read == false)
		{
	  	    // Write ID
		    filter_fastq_1_file << temp_id_1 << std::endl;
		    filter_fastq_2_file << temp_id_2 << std::endl;

		    // Write sequence
	            filter_fastq_1_file << temp_seq_1 << std::endl;
		    filter_fastq_2_file << temp_seq_2 << std::endl;

	      	    // Write "+"
	    	    filter_fastq_1_file << "+" << std::endl;
	    	    filter_fastq_2_file << "+" << std::endl;

		    // Write quality
          	    filter_fastq_1_file << temp_qual_1 << std::endl;
		    filter_fastq_2_file << temp_qual_1 << std::endl;

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
	std::cout << "Out of: " << total_num_records << " sequences, NGSXQualityControlPairedEnd removed: " << total_num_records - final_num_seq << "." << std::endl;
	std::cout << "Percent Filtered Sequences: " << std::setprecision(4) << final_num_seq / (float)total_num_records * 100 << "%" << std::endl;
	std::cout << "\nOutput quality control statistics were written to: " << stats_file_name << "\n" << std::endl;

	std::cout << Palette.GREEN << "Completed the NGSX Quality Control Paired End Module.\n" <<  Palette.RESET << std::endl;
	return 0;
}
