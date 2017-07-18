/*! \file NGSXFastQReconcile.cpp
 *  NGSXFastQReconcile Module: Reconcile reads in paired fastq files
 *  \verbinclude NGSXFastQReconcile.cpp
 */

/*
 * NGSXFastQReconcile: Implementation
 * Date: 2017-0712
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
#include "Utilities.h"						// Requires UPAC function

//---------------------------------Main---------------------------------------//
int main(int argc, char* argv[])
{
	//-------------------------------Usage--------------------------------------//
	const std::string usage=std::string(argv[0])+

			      " [options] "+"\n"+
			      "\nThis program takes a forward and reverse fastq file and only keeps paired reads \n"+

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
			(argc < 11) ||
			(argc > 11 )){
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
		std::ifstream input_first_fastq_file;																				// Creates an input file stream for the first input fastq file
		std::ifstream input_second_fastq_file;																			// Creates an input file stream for the second input fastq file
		std::ifstream fastq_file_copy_first;																							// Copy for counting the number of lines
		std::ifstream fastq_file_copy_second;
		// Output file streams
		std::ofstream output_first_fastq_file;																			// Creates an output file stream for the first fastq file
		std::ofstream output_second_fastq_file;																			// Creates an output file stream for the second fastq file
		std::ofstream stats_file;																										// Stats file

		// Fastq lines
		std::string current_line;																										// String to hold the line read in from the first fastq file
		std::string temp_id;
		std::string temp_seq;
		std::string temp_line3;
		std::string temp_qual;


    		FastQ::FastQ temp_fastq;
    		FastQ::FastQPaired temp_fastq_paired;

		// Associative arrays
		std::map<std::string, FastQ::FastQ> map_reads_forward;							  // Map to hold paired sequences
		std::map<std::string, FastQ::FastQ> map_reads_reverse;							  // Map to hold paired sequences
		std::map<std::string, std::pair<FastQ::FastQ, FastQ::FastQ> > map_properly_paired;							// Map to hold paired sequences

		// Colored text and progress log
		TextColor::TextColor Palette;			 							// TextColor object for coloring text output
		ProgressLog::ProgressLog fastq_progress_log_first;							// ProgressLog object to store file processing progress.
                ProgressLog::ProgressLog fastq_progress_log_second;

		int total_num_lines;											// Number of lines in the copy fastq file
                int total_num_records_first;
                int total_num_records_second;
		int total_num_records;											// Number of sequences in the copy fastq file
		int final_num_seq;											// Number of unique sequences in the final fastq file
		float percent_paired;											// Percent of input sequences that are unique
		std::map<std::string, std::pair<FastQ::FastQ, FastQ::FastQ> >::iterator it;							// Map iterator

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
		fastq_file_copy_first.open(input_file_name_first_fastq.c_str());									// Open copy input file
                fastq_file_copy_second.open(input_file_name_second_fastq.c_str());
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
		std::cout << Palette.GREEN << "\nBeginning the NGSX NGSXFastQIntersect Module.\n" <<  Palette.RESET << std::endl;

		// Count the number of sequences in the input file (using the copy)
		std::cout << "Initializing first fastq file and counting the number of sequences (This may take a while)." << std::endl;
		total_num_lines = std::count(std::istreambuf_iterator<char>(fastq_file_copy_first), std::istreambuf_iterator<char>(), '\n') + 1;
		total_num_records_first = total_num_lines / 4;																// 4 lines for each sequence record
		std::cout << "First input fastq file contains " << total_num_records_first << " sequences." << std::endl;

                std::cout << "Initializing second fastq file and counting the number of sequences (This may take a while)." << std::endl;
                total_num_lines = std::count(std::istreambuf_iterator<char>(fastq_file_copy_second), std::istreambuf_iterator<char>(), '\n') + 1;
                total_num_records_second = total_num_lines / 4;                                                                                                                          // 4 lines for each sequence record
                std::cout << "Second input fastq file contains " << total_num_records_second << " sequences." << std::endl;

                total_num_records = total_num_records_first + total_num_records_second;

		//---------------------------Stores Sequences in Map-----------------------------------//
    std::cout << "Analyzing forward reads." << std::endl;
		fastq_progress_log_first.initLog(total_num_records_first);								  				// Initialize the progres log with the total number of records

		// First fastq
		while (std::getline(input_first_fastq_file, current_line))
		{
			temp_id = current_line;
			std::getline(input_first_fastq_file, temp_seq);											// Second line is the sequence bases
			std::getline(input_first_fastq_file, temp_line3);										// Third line is the "+"
			std::getline(input_first_fastq_file, temp_qual);										// Fourth line is the sequence quality

                        temp_fastq.setRecord(temp_id,
                            temp_seq,
                            temp_line3,
                            temp_qual);


			map_reads_forward[temp_id] = temp_fastq;										// Add or replace sequence and FastQ object in map
                        // Completed reading 1 sequence record
                        fastq_progress_log_first.incrementLog(1);
                }
                std::cout << "Forward read analysis complete." << std::endl;

                // Second fastq
                std::cout << "Analyzing reverse reads." << std::endl;
		fastq_progress_log_second.initLog(total_num_records_second);								  				// Initialize the progres log with the total number of records
                while (std::getline(input_second_fastq_file, current_line))
		{
			temp_id = current_line;																							// First line is the unique sequence ID
			std::getline(input_second_fastq_file, temp_seq);											// Second line is the sequence bases
			std::getline(input_second_fastq_file, temp_line3);										// Third line is the "+"
			std::getline(input_second_fastq_file, temp_qual);										// Fourth line is the sequence quality

                        temp_fastq.setRecord(temp_id,
                            temp_seq,
                            temp_line3,
                            temp_qual);


		        map_reads_reverse[temp_id] = temp_fastq;										// Add or replace sequence and FastQ object in map

                        // Completed reading 1 sequence record
                        fastq_progress_log_second.incrementLog(1);
               }
    std::cout << "Reverse read analysis complete." << std::endl;

		//---------------------------Write Unique Sequences-----------------------//
		std::cout << "Writing paired sequences to file." << std::endl;
		final_num_seq = 0;

                // Use custom map intersection function to find
                // properly paired reads
                map_properly_paired = Utilities::IntersectMaps(map_reads_forward,map_reads_reverse);
                for (it = map_properly_paired.begin(); it != map_properly_paired.end(); it++)
                {
                    // First output file
                    output_first_fastq_file << it->second.first.getID() << std::endl; 
                    output_first_fastq_file << it->second.first.getSeq() << std::endl; 
                    output_first_fastq_file << it->second.first.getLine3() << std::endl;
                    output_first_fastq_file << it->second.first.getQual() << std::endl;                 

                    // Second output file
                    output_second_fastq_file << it->second.second.getID() << std::endl;
                    output_second_fastq_file << it->second.second.getSeq() << std::endl;
                    output_second_fastq_file << it->second.second.getLine3() << std::endl;
                    output_second_fastq_file << it->second.second.getQual() << std::endl;

                    final_num_seq++;
                }

	        percent_paired = final_num_seq / (float)total_num_records * 100;

		stats_file << "Total_Sequences\tPaired_Sequences\tPercent_Paired" << std::endl;
		stats_file << total_num_records << "\t" << final_num_seq << "\t" << std::setprecision(4) << percent_paired << std::endl;

		std::cout << "Out of: " << total_num_records << " sequences, NGSXFastQReconcile removed: " << total_num_records - final_num_seq << "." << std::endl;
		std::cout << "Percent Paired Sequences: " << percent_paired << "%" << std::endl;
		return 0;

}
