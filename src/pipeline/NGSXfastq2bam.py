#!/usr/bin/python
# -*- coding: utf-8 -*-
## @package NGSXfastq2bam
#  Documentation for pipeline script NGSXfastq2bam
#
#  Batch convert fastq files to bam format.

"""
NGSXfastq2bam:  Generate makefile NGSXtrimmerge.mk for automated quality control filtering.
Author:         Katherine Eaton     ktmeaton [at sign here] gmail.com
Date Created:   2016-0223
"""

import os                                                                       # OS directory and file navigation
import argparse                                                                 # Command-line argument parsing

# I do not know how coloring output will work on windows platforms
from TextColor import TextColor                                                 # TextColor Class

#-------------------------------------------------------------------------------#
#                             Argument Parsing                                  #
#-------------------------------------------------------------------------------#

parser = argparse.ArgumentParser(description='Generate makefile NGSXfast2bam.mk for automated fastq to bam conversion.')
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

FASTQ2BAM = '/home/ana/scripts/BCL2BAM2FASTQ/fastq2bam/fastq2bam'

samples_dir = args.samples_directory                                            # Directory of sample directories
output_dir = args.output_directory                                              # Directory for output files

# Safety directory
safety_dir = output_dir + OS_SEP + 'safety'
if not os.path.exists(safety_dir):
    os.makedirs(safety_dir)

makefile = open('NGSXfastq2bam.mk', 'w')                                        #Create the makefile
all_target_list = []                                                            # list of targets for makefile
clean_target_list = []                                                          # This module needs a separate for cleaning targets
samples_list = []                                                               # List of individual sample directories



#-------------------------------------------------------------------------------#
#                                  Processing                                   #
#-------------------------------------------------------------------------------#
print(TextColor.GREEN + 'Beginning Pipeline Script: NGSXfast2bam\n' + TextColor.RESET)  # Processing Begins

# Write the mandatory beginning of the makefile
makefile.write('SHELL := /bin/bash' + '\n\n')
makefile.write('Default:' + '\t' + 'all' + '\n\n')


# Collect path information to individual sample directories
for item in os.listdir(samples_dir):
    item_path = samples_dir + OS_SEP + item
    if os.path.isdir(item_path) and item.startswith("Sample_"):
        samples_list.append(item)

print(TextColor.BLUE + 'Automating fastq to bam conversion for sample directories: ' + str(samples_list) + TextColor.RESET + '\n\n')

print(TextColor.BLUE + 'Creating commands to run fastq2bam on the following files:' + TextColor.RESET + '\n')


#-------------------------------------------------------------------------------#
#                                  FASTQ2BAM                                    #
#-------------------------------------------------------------------------------#

# Search for fastq files, either uncompressed (fastq) or compressed (gzip) form
for ind_sample_dir in samples_list:
    ind_sample_path = samples_dir + OS_SEP + ind_sample_dir
    for file in os.listdir(ind_sample_path):

        # Make directories for individual sample output
        if file.endswith('fastq') or file.endswith('fastq.gz'):
            output_dir_fastq2bam = output_dir + OS_SEP + 'Sample_' + file.split('_')[0] + "_fastq2bam"
            if not os.path.exists(output_dir_fastq2bam):
                os.makedirs(output_dir_fastq2bam)
                clean_target_list.append(output_dir_fastq2bam)


        file_path = ind_sample_path + OS_SEP + file

        # Get forward read fastq path

        if os.path.isfile(file_path) and (file_path.endswith('R1_001.fastq') or file_path.endswith('R1_001.fastq.gz')):
            fastq_1_file_path = file_path
            print (TextColor.RED + fastq_1_file_path)

            fastq2bam_target = output_dir_fastq2bam + OS_SEP + file.split('_')[0] + '.bam'
            ind_sample_prefix = file.split('_')[0]
            all_target_list.append(fastq2bam_target)

            # leeHom directory to remove
            clean_target_list.append(output_dir_fastq2bam)

        # Get reverse read fastq path
        elif os.path.isfile(file_path) and (file_path.endswith('R2_001.fastq')  or file_path.endswith('R2_001.fastq.gz')):
            fastq_2_file_path = file_path
            print (TextColor.RED + fastq_2_file_path)




    # At this point, fastq paths have been acquired for forward and reverse reads
    # Write fastq2bam commands to makefile for execution

    makefile.write(fastq2bam_target + ': ' + fastq_1_file_path +
                    ' ' + fastq_2_file_path + '\n')

    # Write the echo sample statement for shell output
    makefile.write('\t' + "@echo -e '\e[31m" +
                'Running fastq2bam for sample: ' + ind_sample_prefix +
                "'" + '\n')

    # Write the echo execution statement for shell output
    makefile.write('\t' + "@echo -e '\e[32m" +
                    FASTQ2BAM + ' ' +
                    '-o ' + fastq2bam_target + ' ' +
                    '-r ' + ind_sample_prefix + ' ' +
                    fastq_1_file_path + ' ' +
                    fastq_2_file_path + "'" +
                    '\n')

    # Actual execution statement
    makefile.write('\t' + "@" + FASTQ2BAM + ' ' +
                    '-o ' + fastq2bam_target + ' ' +
                    '-r ' + ind_sample_prefix + ' ' +
                    fastq_1_file_path + ' ' +
                    fastq_2_file_path +
                    '\n\n')





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

print( '\n' + TextColor.GREEN + 'Completed Pipeline Script: NGSXfastq2bam' + TextColor.RESET)
