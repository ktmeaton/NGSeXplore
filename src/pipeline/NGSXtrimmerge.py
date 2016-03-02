#!/usr/bin/python
# -*- coding: utf-8 -*-
## @package NGSXtrimmerge
#  Documentation for module NGSXtrimmerge
#
#  Automated leeHom trimming and merging.

"""
NGSXtrimmerge:  Generate makefile NGSXtrimmerge.mk for automated leeHom trimming and merging.
Author:         Katherine Eaton     ktmeaton [at sign here] gmail.com
Date Created:   2016-0218
Date Edited:	2016-0302 - 	Uploaded to github
				Change input to gzipped fastq
Notes:		Requires sample directories to begin with Sample_*
"""

import os                                                                       # OS directory and file navigation
import argparse                                                                 # Command-line argument parsing

# I do not know how coloring output will work on windows platforms
from TextColor import TextColor                                                 # TextColor Class

#-------------------------------------------------------------------------------#
#                             Argument Parsing                                  #
#-------------------------------------------------------------------------------#

parser = argparse.ArgumentParser(description='Generate makefile NGSXtrimmerge.mk for automated leeHom trimming and merging.')
mandatoryArgs = parser.add_argument_group('Mandatory')

mandatoryArgs.add_argument('-d', '--dir',
                    dest='samples_directory',
                    help='Full path to directory of sample folders.',
                    required=True)

# Output directory
mandatoryArgs.add_argument('-o', '--output_dir',
                    dest='output_directory',
                    help='Output directory',
                    required=True)

args = parser.parse_args()                                                      # Store command line arguments


#-------------------------------------------------------------------------------#
#                                  Variables                                    #
#-------------------------------------------------------------------------------#


# An attempt at cross-platform support, TO BE CHANGED IF USING WINDOWS
OS_SEP = "/"

# leeHom parameters
FORWARD_ADAPTER = 'AGATCGGAAGAGC'
REVERSE_ADAPTER = 'AGATCGGAAGAGC'
TRIM_CUTOFF = 3

samples_dir = args.samples_directory                                            # Directory of sample directories
output_dir = args.output_directory                                              # Directory for output files

# Safety directory
safety_dir = output_dir + OS_SEP + 'safety'
if not os.path.exists(safety_dir):
    os.makedirs(safety_dir)

makefile = open('NGSXtrimmerge.mk', 'w')                                        #Create the makefile
all_target_list = []                                                            # list of targets for makefile
clean_target_list = []                                                          # This module needs a separate for cleaning targets
samples_list = []                                                               # List of individual sample directories



#-------------------------------------------------------------------------------#
#                                  Processing                                   #
#-------------------------------------------------------------------------------#
print(TextColor.GREEN + 'Beginning Module: NGSXtrimmerge\n' + TextColor.RESET)  # Processing Begins

# Write the mandatory beginning of the makefile
makefile.write('SHELL := /bin/bash' + '\n\n')
makefile.write('Default:' + '\t' + 'all' + '\n\n')


# Collect path information to individual sample directories
for item in os.listdir(samples_dir):
    item_path = samples_dir + OS_SEP + item
    if os.path.isdir(item_path) and item.startswith("Sample_"):
        samples_list.append(item)

print(TextColor.BLUE + 'Automating leeHom for sample directories: ' + str(samples_list) + TextColor.RESET + '\n\n')

print(TextColor.BLUE + 'Creating commands to run leeHom on the following files:' + TextColor.RESET + '\n')

#-------------------------------------------------------------------------------#
#                                    leeHom                                     #
#-------------------------------------------------------------------------------#

# Search for fastq files, either uncompressed (fastq) or compressed (gzip) form
for ind_sample_dir in samples_list:
    ind_sample_path = samples_dir + OS_SEP + ind_sample_dir
    for file in os.listdir(ind_sample_path):

        # Make directories for individual sample output
        output_dir_leeHom = output_dir + OS_SEP + file.split('_')[0] + "_leeHom"
        if not os.path.exists(output_dir_leeHom):
            os.makedirs(output_dir_leeHom)
            clean_target_list.append(output_dir_leeHom)


        file_path = ind_sample_path + OS_SEP + file

        # Get forward read fastq path
        # I am choosing to only update targets for Read 1 (for paired-end data)
        if os.path.isfile(file_path) and file_path.endswith('R1_001.fastq.gz'):
            fastq_1_file_path = file_path
            print (TextColor.RED + fastq_1_file_path)
            leeHom_prefix = file.split('_')[0] + '.leeHom'
            leeHom_base_target = output_dir_leeHom + OS_SEP + leeHom_prefix
            leeHom_target = output_dir_leeHom + OS_SEP + leeHom_prefix + '.fq.gz'
            all_target_list.append(leeHom_target)

            # leeHom directory to remove
            clean_target_list.append(output_dir_leeHom)

        # Get reverse read fastq path
        elif os.path.isfile(file_path) and file_path.endswith('R2_001.fastq.gz'):
            fastq_2_file_path = file_path
            print (TextColor.RED + fastq_2_file_path)




    # At this point, fastq paths have been acquired for forward and reverse reads
    # Write leeHom commands to makefile for execution
    log_file = output_dir_leeHom + OS_SEP + leeHom_prefix + ".log"

    makefile.write(leeHom_target + ': ' + fastq_1_file_path +
                    ' ' + fastq_2_file_path + '\n')

    # Write the echo sample statement for shell output
    makefile.write('\t' + "@echo -e '\e[31m" +
                'Running leeHom for sample: ' + file.split('_')[0] +
                "'" + '\n')


    # Write the echo execution statement for shell output
    makefile.write('\t' + "@echo -e '\e[32m" +
                'leeHom --ancientdna ' +
                '--log ' + log_file +
                ' -f ' + FORWARD_ADAPTER +
                ' -s ' + REVERSE_ADAPTER +
                ' -t ' + str(TRIM_CUTOFF) +
                ' -fq1 ' + fastq_1_file_path +
                ' -fq2 ' + fastq_2_file_path +
                ' -fqo ' + output_dir_leeHom + OS_SEP + leeHom_prefix + "'"
                '\n')

    # Actual leeHom execution command
    makefile.write('\t' + '@leeHom --ancientdna ' +
                    '--log ' + log_file +
                    ' -f ' + FORWARD_ADAPTER +
                    ' -s ' + REVERSE_ADAPTER +
                    ' -t ' + str(TRIM_CUTOFF) +
                    ' -fq1 ' + fastq_1_file_path +
                    ' -fq2 ' + fastq_2_file_path +
                    ' -fqo ' + output_dir_leeHom + OS_SEP + leeHom_prefix +
                    '\n')

    # Reformat the leeHom log file to have proper tab delimiting
    makefile.write('\t' +
                    "@awk '{print " "$$1\" \"$$2\"\\t\"$$3\"\\t\"$$4}' " +
                    log_file + ' > ' + log_file + '.reformat' + '\n')

    # Write the echo unzip statement for shell output
    makefile.write('\t' + "@echo -e '\e[96m" +
                    'Unzipping all gzip fastq files.' + "'" +
                    '\n')

    # Unzip fastq files for downstream processing
    makefile.write('\t' +
                    '@gunzip ' + output_dir_leeHom + OS_SEP + '*.fq.gz' +
                    '\n')

    makefile.write('\t ' + '@cp ' + log_file + ' ' +
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

print( '\n' + TextColor.GREEN + 'Completed Module: NGSXtrimmerge' + TextColor.RESET)
