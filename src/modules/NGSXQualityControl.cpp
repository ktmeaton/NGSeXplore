/*! \file NGSXQualityControl.cpp
 *  QualityControl Module: Read filtering based on quality scores and length.
 *  \verbinclude NGSXQualityControl.cpp
 * NGSXQualityControl: Implementation
 *\date $Date: 2016-0222
 * Author : Katherine Eaton ktmeaton [at sign here ] gmail.com
 *
 */

//----------------------------System Include----------------------------------//
#include <iostream>									// Input and output to screen
#include <string>										// String
#include <iomanip>									// Set Precision
#include <fstream>									// File input and output
#include <sstream>									// Argument to int
#include <algorithm>								// Count funtion

//----------------------------Custom Include----------------------------------//
#include "FastQ.h"                  // FastQ object
#include "TextColor.h"							// Unix shell colored output
#include "ProgressLog.h"						// ProgressLog Class

//---------------------------------Main---------------------------------------//
int main(int argc, char* argv[])
{
	//---------------------------Help Variables---------------------------------//
	std::string usage = std::string("NGSX Quality Control Module. Filters by quality threshold and length for single-end short reads. \n") +
									"Options:\n" +
										"\n\tYou must specify one input fastq file :\n" +
                    "\t\t" + "--fq-in" + "\t\t" + "Input fastq" + "\n" +
                    "\n\tYou must specify one output fastq file :\n" +
                    "\t\t" + "--fq-out" + "\t\t" + "Output fastq file " + "\n" +
		    						"\n\tYou must specify one text file for stats output:\n" +
                    "\t\t" + "--stats" + "\t\t\t" + "Output stats file " + "\n" +
										"\n\tParameters to control filtering: \n" +
										"\t\t" + "--phred" + "\t\t" + "Phred encoding (33 or 64) [INT]" + "\n" +
										"\t\t" + "-q" + "\t\t" + "Minimum quality threshold [INT]" + "\n" +
										"\t\t" + "-p" + "\t\t" + "Proportion of read that must meet minimum quality threshold [FLOAT]" + "\n" +
										"\t\t" + "-l" + "\t\t" + "Minimum read length to keep [INT]" + "\n\n";

	//-----------------------------Help Message---------------------------------//
	if ((argc == 1) ||
		(argc == 2 && std::string(argv[1]) == "-h") ||
		(argc == 2 && std::string(argv[1]) == "-help") ||
		(argc == 2 && std::string(argv[1]) == "--help") ||
		(argc !=14)
	{
		std::cerr << usage << std::endl;
		return 1;
	}


	//-----------------------Implementation Variables-------------------------//

	// File Names
	std::string input_file_name_fastq;       // Input fastq
	std::string output_file_name_fastq;      // Output fastq
	std::string stats_file_name;             // Stats file

	// Input file streams
	std::ifstream input_fastq_file;          // Input file stream
	std::ifstream fastq_file_copy;           // Copy for counting lines

	// Output file streams
	std::ofstream output_fastq_file;         // Output file stream
	std::ofstream stats_file;                // Stats file

	// Fastq lines
	std::string current_line;
	std::string temp_id;
	std::string temp_seq;
	std::string temp_line3;
	std::string temp_qual;

	// Number of bases in current read that are above quality threshold
	int bases_above_threshold;

	// Quality of the current base
	int base_quality;

	bool keep_read = false;

	// Associative arrays and iterators
	std::map<std::string, FastQ::FastQ> map_filtered;  // Map filtered
	std::map<std::string, FastQ::FastQ>::iterator it;      // Map iterator

	// FastQ Objects, Colored text and progress log
	FastQ::FastQ temp_fastq;
	TextColor::TextColor Palette;                   // Colored text output
	ProgressLog::ProgressLog fastq_progress_log;    // Progress log

	// Stats variables
	int total_num_lines;                            // Num lines in copy
	int total_num_records;                          // Num fastq records
	int final_num_seq;                              // Num kept through filtering
	float percent_filtered;                           // Percent of input

	// Integer command-line arguments arguments
	int i_phred;
	int PHRED_BASE;

	int i_min_qual;
	int MIN_QUAL;

	float f_prop_thresh;
	float PROP_THRESHOLD;


	int i_min_len;
	int MIN_LENGTH;

	//------------------------------Arg Parsing------------------------------//

	for ( int i = 1; i < ( argc - 1 ); i++ ) //all but last argument (file)
	{

			if ( std::string( argv[i] ) == "--fq-in" )
			{
					input_file_name_fastq = std::string( argv[i + 1] );
					i++;
					continue;
			}


			else if ( std::string( argv[i] ) == "--fq-out" )
			{
					output_file_name_fastq = std::string( argv[i + 1] );
					i++;
					continue;
			}

			else if ( std::string( argv[i] ) == "--stats" )
			{
					stats_file_name = std::string( argv[i + 1] );
					i++;
					continue;
			}

			else if ( std::string( argv[i] ) == "--phred" )
			{
					std::istringstream ss_phred(argv[i + 1]);
					if (!(ss_phred >> i_phred))  std::cerr << "Invalid phred base. " << ss_phred << '\n';
					PHRED_BASE = i_phred;						// Phred base quality
					i++;
					continue;
			}

			else if ( std::string( argv[i] ) == "-q" )
			{
					std::istringstream ss_min_qual( argv[i + 1] );
					if (!(ss_min_qual >> i_min_qual))  std::cerr << "Invalid minimum quality. " << ss_min_qual << '\n';
					MIN_QUAL = i_min_qual;						// Phred base quality
					i++;
					continue;
			}

			else if ( std::string( argv[i] ) == "-p" )
			{
					std::istringstream ss_prop_thresh(argv[i + 1]);
					if (!(ss_prop_thresh >> f_prop_thresh))  std::cerr << "Invalid quality proportion threshold. " << ss_prop_thresh << '\n';
					PROP_THRESHOLD = f_prop_thresh;						// Phred base quality
					i++;
					continue;
			}

			else if ( std::string( argv[i] ) == "-l" )
			{
					std::istringstream ss_min_len(argv[i + 1]);
					if (!(ss_min_len >> i_min_len))  std::cerr << "Invalid minimum length. " << ss_min_len << '\n';
					MIN_LENGTH = i_min_len;						// Phred base quality
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

	input_fastq_file.open(input_file_name_fastq.c_str() );
	fastq_file_copy.open(input_file_name_fastq.c_str() );
	output_fastq_file.open(output_file_name_fastq.c_str() );
	stats_file.open( stats_file_name.c_str() );

	// Check if files can be opened properly
	if ( input_fastq_file.fail() )
	{
			std::cerr << "ERROR: Cannot open input fastq file: " <<
											input_file_name_fastq << std::endl;
			return 1;
	}


	if ( output_fastq_file.fail() )
	{
			std::cerr << "ERROR: Cannot open output fastq file: " <<
											output_file_name_fastq << std::endl;
			return 1;
	}

	if ( stats_file.fail() )
	{
			std::cerr << "ERROR: Cannot open stats file." << stats_file_name << std::endl;
			return 1;
	}

	//----------------------------Begin Processing------------------------------//
	std::cout << Palette.GREEN << "\nBeginning the NGSXQualityControl Module.\n" <<  Palette.RESET << std::endl;

	// Count the number of sequences in the input file (using the copy)
	std::cout << "Initializing files and counting the number of sequences (This may take a while)." << std::endl;
	total_num_lines = std::count( std::istreambuf_iterator<char>( fastq_file_copy ),
																	std::istreambuf_iterator<char>(), '\n' ) + 1;

	total_num_records = total_num_lines /  4;           // 4 lines per record
	fastq_progress_log.initLog(total_num_records );     // Init log
	std::cout << "Input fastq file contains " << total_num_records << " sequences." << std::endl;

	//-------------------------Filter By Quality----------------------------//
  while ( std::getline( input_fastq_file, current_line ) )
	{
		// Default is to reject a read
		keep_read = false;

		// First fastq
		temp_id = current_line;                               // ID
		std::getline( input_fastq_file, temp_seq);      // Sequence
		std::getline( input_fastq_file, temp_line3);    // Ambiguous
		std::getline( input_fastq_file, temp_qual);    // Quality

		// Store fastq record as FastQ Object
		temp_fastq.setRecord( temp_id, temp_seq, temp_line3, temp_qual);


		// Check if read is long enough to pass minimum length filter
		if (temp_fastq.getLength() >= MIN_LENGTH)
		{
			// initalize count of high-quality bases to 0
			bases_above_threshold = 0;
			// Analyze read base qualities
			for(int i = 0; i < temp_fastq.getLength(); i++)
			{
				// Check base quality
				base_quality = int(temp_fastq.getQual()[i]);
				if((base_quality - PHRED_BASE) >= MIN_QUAL) {bases_above_threshold++;}
			}

			// Check quality conditions
			if(bases_above_threshold >= (temp_fastq.getLength() * PROP_THRESHOLD)
				)
				{ keep_read = true;}
		}

		// Write to output filtered files if the read passes quality control
		if (keep_read == true)
		{
			// Add record map/dict/hash table of filtered reads
			map_filtered[temp_id] = temp_fastq;
		}

		 // Completed reading 1 sequence record
		fastq_progress_log.incrementLog( 1 );
		} // end while loop

		//---------------------------Write Filtered Sequences-----------------------//
		std::cout << "Writing filtered sequences to file." << std::endl;
		final_num_seq = 0;

		for ( it = map_filtered.begin(); it != map_filtered.end(); ++it )
		{
				// First output file
				output_fastq_file << it->second.getID() << std::endl;
				output_fastq_file << it->second.getSeq() << std::endl;
				output_fastq_file << it->second.getLine3() << std::endl;
				output_fastq_file << it->second.getQual() << std::endl;

				// Completed writing 1 sequence record
				final_num_seq++;
		}

		percent_filtered = final_num_seq / ( float )total_num_records * 100;

		stats_file << "Total_Sequences\tFiltered_Sequences\tPercent_Filtered" << std::endl;
		stats_file << total_num_records << "\t" << final_num_seq << "\t" <<
										std::setprecision( 4 ) << percent_filtered << "%" << std::endl;

		std::cout << "Out of: " << total_num_records <<
										" sequences, NGSXQualityControlPairedEnd removed: " << total_num_records -
										final_num_seq << "." << std::endl;
		std::cout << "Percent Filtered Sequences: " << percent_filtered << "%" << std::endl;
		return 0;

		}
