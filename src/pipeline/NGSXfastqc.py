#!/usr/bin/python
# -*- coding: utf-8 -*-
## @package NGSXfastqc
#  Module NGSXfastqc
#
#  Automated profile with fastqc

"""
NGSXfastqc:     Generate makefile NGSXfastqc.mk for automated fastqc profiling.
Author:         Katherine Eaton     ktmeaton [at sign here] gmail.com
Date Created:   2016-0218
Edited:         2016-0226 - Uploaded to github, added usage comments.
"""

# An attempt at cross-platform support, TO BE CHANGED IF USING WINDOWS
OS_SEP = "/"

import os                                                                       # OS directory and file navigation
import argparse                                                                 # Command-line argument parsing

# I do not know how coloring output will work on windows platforms
# Meaning I do know: it will not work.
from TextColor import TextColor                                                 # TextColor Class

#-------------------------------------------------------------------------------#
#                             Argument Parsing                                  #
#-------------------------------------------------------------------------------#

parser = argparse.ArgumentParser(description='Generate makefile NGSXfastqc.mk for automated fastqc profiling.')
mandatoryArgs = parser.add_argument_group('Mandatory')

# Input directory holding sample directories
mandatoryArgs.add_argument('-d', '--dir',
                    dest='samples_directory',
                    help='Full path to directory of sample folders.',
                    required=True)

# Number of CPU threads
mandatoryArgs.add_argument('-t', '--threads',
                    type = int,
                    dest='num_threads',
                    help="Number of CPU threads to use.",
                    required=True)

# Output directory
# Directories will be created in this directory for each sample.
# Individual sample directories will take everything before the first underscore in the filename
# Ex. L156_2016-0222
mandatoryArgs.add_argument('-o', '--output_dir',
                    dest='output_directory',
                    help='Output directory',
                    required=True)
                   

args = parser.parse_args()                                                      # Store command line arguments



#-------------------------------------------------------------------------------#
#                                  Variables                                    #
#-------------------------------------------------------------------------------#
samples_dir = args.samples_directory                                            # Directory of sample directories
threads = args.num_threads                                                      # Number of CPU threads to use when running fastqc
output_dir = args.output_directory                                              # Directory for output files


# Safety Folder - Not affected by "make clean"
safety_dir = output_dir + OS_SEP + 'safety'					# Output fastqc.zip file will be copied here
if not os.path.exists(safety_dir):
    os.makedirs(safety_dir)



makefile = open('NGSXfastqc.mk', 'w')                                           # Create the makefile
all_target_list = []                                                            # List of targets for makefile
clean_target_list = []                                                          # This module needs a separate for cleaning targets
samples_list = []                                                               # List of individual sample directories


#-------------------------------------------------------------------------------#
#                                  Processing                                   #
#-------------------------------------------------------------------------------#
print(TextColor.GREEN + 'Beginning Module: NGSXfastqc\n' + TextColor.RESET)                       

# Write the mandatory beginning of the makefile (note: requires bash shell in /bin/bash)
makefile.write('SHELL := /bin/bash' + '\n\n')
makefile.write('Default:' + '\t' + 'all' + '\n\n')


# Collect path information to individual sample directories
# Directories must start with "Sample_XXX" where XXX is user specific
for item in os.listdir(samples_dir):
    item_path = samples_dir + OS_SEP + item
    if os.path.isdir(item_path) and item.startswith("Sample_"):
        samples_list.append(item)

print(TextColor.BLUE + 'Automating fastqc for sample directories: ' + str(samples_list) + '\n\n')

print(TextColor.BLUE + 'Creating commands to run fastqc on the following files:\n')

# Search for fastq files, either uncompressed (fastq) or compressed (gzip) form
for ind_sample_dir in samples_list:
    ind_sample_path = samples_dir + OS_SEP + ind_sample_dir
    for file in os.listdir(ind_sample_path):
        file_path = ind_sample_path + OS_SEP + file
        if os.path.isfile(file_path) and (file_path.endswith('.fastq') or file_path.endswith('.fastq.gz')):

            # Make directories for individual sample output
            output_dir_fastqc = (output_dir + OS_SEP +
                                    file.split('_')[0] + "_fastqc")
            if not os.path.exists(output_dir_fastqc):
                os.makedirs(output_dir_fastqc)
                clean_target_list.append(output_dir_fastqc)

            # Find fastq files

            fastq_file_path = file_path

            if file.endswith('.fastq'):
                print (TextColor.RED + fastq_file_path)
                fastqc_target = (output_dir_fastqc + OS_SEP +
                            file.strip('.fastq') + '_fastqc.zip')

                # Write fastqc commands to makefile for execution
                makefile.write(fastqc_target + ':' + ' ' + fastq_file_path + '\n')
                makefile.write('\t' + "@echo -e '\e[32m" + "fastqc -t " +
                            str(threads) + ' -o ' + output_dir_fastqc + ' ' +
                            fastq_file_path + "\e[39m'" +
                            '\n')

                makefile.write('\t' + '@fastqc -t ' +
                            str(threads) + ' -o ' + output_dir_fastqc + ' ' +
                            fastq_file_path + '\n')
                all_target_list.append(fastqc_target)

	        makefile.write('\t' + '@cp ' + fastqc_target + ' ' +
			             safety_dir + OS_SEP + '\n\n')

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

print( '\n' + TextColor.GREEN + 'Completed Module: NGSXfastqc' + TextColor.RESET + '\n')
# Reset text color, just in case
print(TextColor.RESET)
