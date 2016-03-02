#!/usr/bin/python
# -*- coding: utf-8 -*-
## @package NGSXclassify.py
#  Documentation for module NGSXclassify.py
#
#  More details.

"""
NGSXclassify:  Generate makefile NGSXclassify.mk for automated BLASTN and LCA binning.
Author:         Katherine Eaton     ktmeaton [at sign here] gmail.com
Date Created:   2016-0218
Date Edited:	2016-0302, enabled user to specify krona directories
Notes:		Requires input directory to end in *_qualtrim.
		Requires input files to end in *.qualtrim.fastq
"""

import os                                                                       # OS directory and file navigation
import argparse                                                                 # Command-line argument parsing

# I do not know how coloring output will work on windows platforms
from TextColor import TextColor                                                 # TextColor Class

#-------------------------------------------------------------------------------#
#                             Argument Parsing                                  #
#-------------------------------------------------------------------------------#

parser = argparse.ArgumentParser(description='Generate makefile NGSXclassify.mk for automated BLASTN and LCA binning.')
mandatoryArgs = parser.add_argument_group('Mandatory')

mandatoryArgs.add_argument('-d', '--dir',
                    dest='samples_directory',
                    help='Full path to directory of quality filtered sample folders.',
                    required=True)

# Output directory
mandatoryArgs.add_argument('-o', '--output_dir',
                    dest='output_directory',
                    help='Output directory.',
                    required=True)


# Number of CPU threads
mandatoryArgs.add_argument('-t', '--threads',
                    type = int,
                    dest='num_threads',
                    help="Number of CPU threads to use.",
                    required=True)

# Max number of blast hits
mandatoryArgs.add_argument('-m', '--max_num_hits',
                    type = int,
                    dest='max_num_hits',
                    help="Maximum of blast hits to keep.",
                    required=True)

# Krona bin directory
mandatoryArgs.add_argument('-k', '--krona-bin-dir',
                    dest='krona_bin_dir',
                    help="Krona binary directory.",
                    required=True)

# NCBI names file path
mandatoryArgs.add_argument('-n', '--names-path',
                    dest='NCBI_names_path',
                    help="Path to NCBI names.dmp.",
                    required=True)

# Path to python 2.7
mandatoryArgs.add_argument('-p', '--python27',
                    dest='python27_path',
                    help="Path to python 2.7.",
                    required=True)                   


args = parser.parse_args()                                                      # Store command line arguments


#-------------------------------------------------------------------------------#
#                                  Variables                                    #
#-------------------------------------------------------------------------------#


# An attempt at cross-platform support, TO BE CHANGED IF USING WINDOWS
OS_SEP = "/"

# Current working directory
CWD = os.getcwd()

# Script path
script_path = os.path.realpath(__file__)
split_path = script_path.split(OS_SEP)
NGSX_root_list = split_path[:len(split_path)-3]
NGSX_root_path = ""
for directory in NGSX_root_list:
    NGSX_root_path += OS_SEP + directory


samples_dir = args.samples_directory                                            # Directory of sample directories
output_dir = args.output_directory                                              # Directory for output files
num_threads = args.num_threads
max_num_hits = args.max_num_hits
KRONABIN = args.krona_bin_dir
NAMES = args.NCBI_names_path
PYTHON27 = python27_path
TAXID2NAME = NGSX_root_path + OS_SEP + "src" + OS_SEP + "pipeline" + OS_SEP + "taxid2name.py"

# Safety Dir
safety_dir = output_dir + OS_SEP + 'safety'
if not os.path.exists(safety_dir):
    os.makedirs(safety_dir)


makefile = open('NGSXclassify.mk', 'w')                                         #Create the makefile
all_target_list = []                                                            # list of targets for makefile
clean_target_list = []                                                          # This module needs a separate for cleaning targets
samples_list = []                                                               # List of individual sample directories



#-------------------------------------------------------------------------------#
#                                  Processing                                   #
#-------------------------------------------------------------------------------#
print(TextColor.GREEN + 'Beginning Module: NGSXclassify' + TextColor.RESET + '\n')                       # Processing Begins

# Write the mandatory beginning of the makefile
makefile.write('SHELL := /bin/bash' + '\n\n')
makefile.write('Default:' + '\t' + 'all' + '\n\n')


# Collect path information to individual sample directories
for item in os.listdir(samples_dir):
    item_path = samples_dir + OS_SEP + item
    if os.path.isdir(item_path) and item.endswith("_qualtrim"):
        samples_list.append(item)

print(TextColor.BLUE + 'Automating Blastn for sample directories: ' + str(samples_list) + TextColor.RESET + '\n\n')

print(TextColor.BLUE + 'Creating commands to run BLASTN on the following files: ' + TextColor.RESET + '\n')


#-------------------------------------------------------------------------------#
#                                   Blastn                                      #
#-------------------------------------------------------------------------------#
# Search for the merged output fastq files from leeHom
for qualtrim_ind_sample_dir in os.listdir(samples_dir):
    qualtrim_ind_sample_path = samples_dir + OS_SEP + qualtrim_ind_sample_dir
    if not qualtrim_ind_sample_path.endswith('_qualtrim'): continue
    for qualtrim_file in os.listdir(qualtrim_ind_sample_path):
        if qualtrim_file.endswith('.qualtrim.fastq'):
            qualtrim_file_path = qualtrim_ind_sample_path + OS_SEP + qualtrim_file
            print (TextColor.RED + qualtrim_file_path)

            # Make directories for individual sample output
            output_dir_blast = output_dir + OS_SEP + qualtrim_file.split('.')[0] + "_blast"
            if not os.path.exists(output_dir_blast):
                os.makedirs(output_dir_blast)
                clean_target_list.append(output_dir_blast)

            # Make blast target
            blast_target = (output_dir_blast + OS_SEP +
                                qualtrim_file.replace('.fastq','.blast.txt'))

            classify_target = blast_target.replace('.txt','.krona.classified')

            stats = (output_dir_blast + OS_SEP +
                        qualtrim_file.split('.')[0] + '.stats.txt')

            stats_binned = stats.replace('.txt', '.blast.binned.txt')

            fasta_target = blast_target.replace('.blast.txt','.fasta')



            #-------------------------------------------------------------------#
            #                               Makefile                            #
            #-------------------------------------------------------------------#

            #----------------------------FastQ to FastA-------------------------#
            makefile.write(fasta_target + ': ' +
			qualtrim_file_path + '\n')

            # Convert to fasta echo statement
            makefile.write('\t' + "@echo -e '\e[31m" +
                    'Converting fastq to fasta for sample: ' + qualtrim_file.split('.')[0] +
                    '\e[0m' + "'" + '\n')

            # Convert to fasta command
            makefile.write('\t' +
                            "@awk 'BEGIN{RS=\"@HWI-1KL153:\"}{if (NR != 1){print \">\"$$1\"\\n\"$$2}}' " +
                            qualtrim_file_path + ' > ' +
                            fasta_target +
                            '\n\n')

            #------------------------------FastA Stats--------------------------#

            # Create stats file echo statement
            makefile.write('\t' + "@echo -e '\e[31m" +
                    'Creating fasta stats file.' +
                    '\e[0m' + "'" + '\n')

            # Create stats file command
            makefile.write('\t' +
                            "@awk 'BEGIN{RS=\">\"}{if (NR == 1) {print \"name\\tGC.\\tlength\"} if (NR != 1){print $$1\"\\t\"((gsub(\"G\",\"X\",$$2) + gsub(\"C\",\"X\",$$2)) / length($$2) * 100)\"\\t\"length($$2)}}' " +
                            fasta_target + ' > ' +
                            stats +'\n\n')

            #--------------------------------BLASTN-----------------------------#
            makefile.write(blast_target + ': ' +
			fasta_target + '\n')

            # Write the echo blast statement for shell output
            makefile.write('\t' + "@echo -e '\e[31m" +
                    'Conducting BLASTN analysis.' +
                    '\e[0m' + "'" + '\n')

            makefile.write('\t' + '@blastn -query ' + fasta_target +
                            ' -db nt' +
                            ' -num_threads ' + str(num_threads) +
                            ' -max_target_seqs ' + str(max_num_hits) +
                            ' -outfmt 6' +
                            ' -out ' + blast_target +
                            '\n\n')

            #-----------------------------KRONA LCA-----------------------------#
            makefile.write(classify_target + ': ' +
				blast_target + '\n')

            # Write the echo cd to KRONA statement for shell output
            makefile.write('\t' + "@echo -e '\e[96m" +
                                'Changing to the KRONA binary directory to classify blast hits.' + "'" +
                                '\n')
            makefile.write('\t' + '@cd ' + KRONABIN + '; perl ../scripts/ClassifyBLAST.pl -o ' +
                            classify_target + ' ' +
                            blast_target +
                            '\n')


            # Write the echo cd to CWD statement for shell output
            makefile.write('\t' + "@echo -e '\e[96m" +
                                'Changing to the original working directory.' + "'" +
                                '\n')

            makefile.write('\t' + '@cd ' + CWD + '\n\n')

            #-----------------------------Translate-----------------------------#
            translate_target = classify_target + '.translate'

	    makefile.write(stats_binned + ': ' +
				classify_target + '\n')

            makefile.write('\t' + "@echo -e '\e[32m" +
                    'Translating taxonomic identifiers to names.' +
                    '\e[0m' + "'" + '\n')

            makefile.write('\t' + "@echo -e '\e[32m" +
                    'Creating stats file: ' + stats_binned +
                    '\e[0m' + "'" + '\n')

            makefile.write('\t' +
                            '@' + PYTHON27 + ' src' + OS_SEP + 'taxid2name.py ' +
                            NAMES + ' ' +
                            classify_target + ' ' +
		            translate_target + ' ' +
                            stats + ' ' +
                            stats_binned +
                            '\n')
	    makefile.write('\t' +
			    '@cp ' + stats_binned + ' ' +
			    safety_dir + '\n\n')

            #------------------------------RPlot--------------------------------#
	    RPlot = (output_dir_blast + OS_SEP +
                        qualtrim_file.split('.')[0] + '.binned.pdf')
            all_target_list.append(RPlot)


            makefile.write(RPlot + ': ' +
			stats_binned + '\n')

            makefile.write('\t' + "@echo -e '\e[35m" +
                    'Creating RPlot: ' +
                    '\e[0m' + "'" + '\n')

            makefile.write('\t' + 'src/plotBinStats.R ' +
                    stats_binned + ' ' +
                    '15 ' + RPlot +
                    '\n')

            makefile.write('\t' +
			'@cp ' + RPlot + ' ' +
			safety_dir + '\n\n')





#-------------------------------------------------------------------------------#
#                               Finishing-Up                                    #
#-------------------------------------------------------------------------------#

# Write all targets to makefile
makefile.write('all:' + '\t')
for target in all_target_list:
    makefile.write(target + ' ')
makefile.write('\n\n')

# Write all targets to clean file
makefile.write('clean:' + '\n' + '\t' + '@rm -rfv' )
for target in clean_target_list:
    makefile.write(' ' + target)

# Close the makefile
makefile.close()

print( '\n' + TextColor.GREEN + 'Completed Module: NGSXclassify' + TextColor.RESET + '\n')
