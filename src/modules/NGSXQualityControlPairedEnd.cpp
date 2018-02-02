/*! \file NGSXQualityControlPairedEnd.cpp
 *  NGSXQualityControlPairedEnd Module: Read filtering for paired-end data based on quality scores and length.
 *  \verbinclude NGSXQualityControlPairedEnd.cpp
 *
 * NGSXQualityControlPairedEnd: Implementation
 * Date: 2016-0222
 * \author $Author : Katherine Eaton
 * Contact: ktmeaton [at sign here ] gmail.com
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
	std::string usage = std::string("NGSX Quality Control Paired End Module. Filters by quality threshold and length for paired-end short reads. \n") +
									"Options:\n" +
										"\n\tYou must specify two input fastq files :\n" +
                    "\t\t" + "--fq1-in" + "\t\t" + "First fastq" + "\n" +
                    "\t\t" + "--fq2-in" + "\t\t" + "Second  fastq file" + "\n" +
                    "\n\tYou must specify two output fastq files :\n" +
                    "\t\t" + "--fq1-out" + "\t\t" + "Output first fastq file " + "\n" +
                    "\t\t" + "--fq2-out" + "\t\t" + "Output second fastq file " + "\n" +
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
		(argc < 19))
	{
		std::cerr << usage << std::endl;
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

	std::string temp_id_paired;

  // Number of bases in current read that are above quality threshold
	int bases_above_threshold_first;
	int bases_above_threshold_second;

  // Quality of the current base
	int base_quality_first;
	int base_quality_second;

	bool keep_read = false;

	// Associative arrays and iterators
	std::map<std::string, FastQ::FastQPaired> map_filtered_paired;  // Map filtered
	std::map<std::string, FastQ::FastQPaired>::iterator it;      // Map iterator

	// FastQ Objects, Colored text and progress log
	FastQ::FastQ temp_fastq_first;
	FastQ::FastQ temp_fastq_second;
	FastQ::FastQPaired temp_fastq_paired;
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
					std::istringstream ss_min_qual(argv[i + 1]);
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

	//----------------------------Begin Processing------------------------------//
  std::cout << Palette.GREEN << "\nBeginning the NGSXQualityControlPairedEnd Module.\n" <<  Palette.RESET << std::endl;

	// Count the number of sequences in the input file (using the copy)
	std::cout << "Initializing files and counting the number of sequences (This may take a while)." << std::endl;
	total_num_lines = std::count( std::istreambuf_iterator<char>( fastq_file_copy ),
																	std::istreambuf_iterator<char>(), '\n' ) + 1;
	total_num_records = total_num_lines /  4;           // 4 lines per record
	fastq_progress_log.initLog(total_num_records );     // Init log
	std::cout << "Input fastq file contains " << total_num_records << " sequences." << std::endl;


	//-------------------------Filter By Quality----------------------------//

	while ( std::getline( input_first_fastq_file, current_line ) )
	{

		  // Default is to reject a read
		  keep_read = false;

			// First fastq
			temp_id_first = current_line;                               // ID
			std::getline( input_first_fastq_file,temp_seq_first );      // Sequence
			std::getline( input_first_fastq_file,temp_line3_first );    // Ambiguous
			std::getline( input_first_fastq_file, temp_qual_first );    // Quality

			// Store fastq record as FastQ Object
			temp_fastq_first.setRecord( temp_id_first, temp_seq_first, temp_line3_first,
											temp_qual_first );

			// Second fastq
			std::getline( input_second_fastq_file, temp_id_second );    // ID
			std::getline( input_second_fastq_file, temp_seq_second );   // Sequence
			std::getline( input_second_fastq_file, temp_line3_second ); // Ambiguous
			std::getline( input_second_fastq_file, temp_qual_second );  // Quality

			// Store fastq record as FastQ Object
			temp_fastq_second.setRecord( temp_id_second, temp_seq_second, temp_line3_second,
											temp_qual_second );

			// Store paired fastq record as FastQPaired Object
      temp_id_paired = temp_id_first + "}{" + temp_id_second;
      temp_fastq_paired.setRecord( temp_fastq_first, temp_fastq_second );

      // Check if read is long enough to pass minimum length filter
      if (temp_fastq_first.getLength() >= MIN_LENGTH && temp_fastq_second.getLength() >= MIN_LENGTH)
			{
				// initalize count of high-quality bases to 0
				bases_above_threshold_first = 0;
				bases_above_threshold_second = 0;
				// Analyze read 1 base qualities
				for(int i = 0; i < temp_fastq_first.getLength(); i++)
				{
					// Check base quality
					base_quality_first = int(temp_fastq_first.getQual()[i]);
					if((base_quality_first - PHRED_BASE) >= MIN_QUAL) {bases_above_threshold_first++;}
				}
				// Analyze read 2 base qualities
				for(int i = 0; i < temp_fastq_second.getLength(); i++)
				{
					// Check base quality
					base_quality_second = int(temp_fastq_second.getQual()[i]);
					if((base_quality_second - PHRED_BASE) >= MIN_QUAL) {bases_above_threshold_second++;}
				}

				// Check quality conditions
				if((bases_above_threshold_first >= (temp_fastq_first.getLength() * PROP_THRESHOLD))
						&& (bases_above_threshold_second >= (temp_fastq_second.getLength() * PROP_THRESHOLD))
					)
					{ keep_read = true;}
			}

		  // Write to output filtered files if the read passes quality control
		  if (keep_read == true)
		  {
				// Add record map/dict/hash table of filtered reads
				map_filtered_paired[temp_id_paired] = temp_fastq_paired;
		  }

		   // Completed reading 1 sequence record
		  fastq_progress_log.incrementLog( 1 );
	} // end while loop


	//---------------------------Write Filtered Sequences-----------------------//
	std::cout << "Writing filtered sequences to file." << std::endl;
	final_num_seq = 0;

	for ( it = map_filtered_paired.begin(); it != map_filtered_paired.end(); ++it )
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

	percent_filtered = final_num_seq / ( float )total_num_records * 100;

	stats_file << "Total_Sequences\tFiltered_Sequences\tPercent_Filtered" << std::endl;
	stats_file << total_num_records << "\t" << final_num_seq << "\t" <<
									std::setprecision( 4 ) << percent_filtered << std::endl;

	std::cout << "Out of: " << total_num_records <<
									" sequences, NGSXQualityControlPairedEnd removed: " << total_num_records -
									final_num_seq << "." << std::endl;
	std::cout << "Percent Filtered Sequences: " << percent_filtered << "%" << std::endl;
	return 0;

	}
