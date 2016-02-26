#!/usr/bin/python
# -*- coding: utf-8 -*-
## @package NGSXremovedup
#  Documentation for module NGSXremovedup
#
#  More details.

"""
NGSXremovedup:  Generate makefile NGSXremovedup.mk for duplicate removal.
Author:         Katherine Eaton     ktmeaton [at sign here] gmail.com
Date Created:   2016-0218
"""

import os                                                                       # OS directory and file navigation
import argparse                                                                 # Command-line argument parsing

# I do not know how coloring output will work on windows platforms
from TextColor import TextColor                                                 # TextColor Class

#-------------------------------------------------------------------------------#
#                             Argument Parsing                                  #
#-------------------------------------------------------------------------------#

parser = argparse.ArgumentParser(description='Generate makefile NGSXremovedup.mk for automated duplicate removal.')
mandatoryArgs = parser.add_argument_group('Mandatory')

mandatoryArgs.add_argument('-d', '--dir',
                    dest='samples_directory',
                    help='Full path to directory of leeHom sample folders.',
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

# Remove duplicate binary
REMOVEDUP = 'bin/NGSXremovedup'



samples_dir = args.samples_directory                                            # Directory of sample directories
output_dir = args.output_directory                                              # Directory for output files

# Safety Dir
safety_dir = output_dir + OS_SEP + 'safety'
if not os.path.exists(safety_dir):
    os.makedirs(safety_dir)


makefile = open('NGSXremovedup.mk', 'w')                                        #Create the makefile
all_target_list = []                                                            # list of targets for makefile
clean_target_list = []                                                          # This module needs a separate for cleaning targets
samples_list = []                                                               # List of individual sample directories



#-------------------------------------------------------------------------------#
#                                  Processing                                   #
#-------------------------------------------------------------------------------#
print(TextColor.GREEN + 'Beginning Module: NGSXremovedup\n')                       # Processing Begins

# Write the mandatory beginning of the makefile
makefile.write('SHELL := /bin/bash' + '\n\n')
makefile.write('Default:' + '\t' + 'all' + '\n\n')


# Collect path information to individual sample directories
for item in os.listdir(samples_dir):
    item_path = samples_dir + OS_SEP + item
    if os.path.isdir(item_path) and item.endswith("_leeHom"):
        samples_list.append(item)

print(TextColor.BLUE + 'Automating duplicate removal for sample directories: ' + str(samples_list) + '\n\n')

print(TextColor.BLUE + 'Creating commands to run remove duplicates on the following files:\n')



#-------------------------------------------------------------------------------#
#                             Remove Duplicates                                 #
#-------------------------------------------------------------------------------#
# Search for the merged output fastq files from leeHom
for leeHom_ind_sample_dir in os.listdir(samples_dir):
    leeHom_ind_sample_path = samples_dir + OS_SEP + leeHom_ind_sample_dir
    for leeHom_file in os.listdir(leeHom_ind_sample_path):
        if leeHom_file.endswith('.leeHom.fq'):
            leeHom_file_path = leeHom_ind_sample_path + OS_SEP + leeHom_file
            print (TextColor.RED + leeHom_file_path)

            # Make directories for individual sample output
            output_dir_removedup = output_dir + OS_SEP + leeHom_file.split('.')[0] + "_removedup"
            if not os.path.exists(output_dir_removedup):
                os.makedirs(output_dir_removedup)
                clean_target_list.append(output_dir_removedup)

            # Make removedup target
            removedup_target = (output_dir_removedup + OS_SEP +
                                leeHom_file.strip('.fq') +
                                '.removedup.fastq')
            all_target_list.append(removedup_target)

            #Sorted target
            sorted_target = removedup_target.replace('.fastq','.sort.fastq')

            #Stats file for percent unique
            stats_file = sorted_target.replace('.fastq','.stats')

            # Write Removedup target commands to makefile for execution
            makefile.write(removedup_target + ': ' + leeHom_file_path + '\n')

            # Write the echo removedup statement for shell output
            makefile.write('\t' + "@echo -e '\e[31m" +
                    'Removing duplicate reads for sample: ' + leeHom_file.split('.')[0] +
                    '\e[0m' + "'" + '\n')

            # Write the removedup command for shell output
            makefile.write('\t' + '@' + REMOVEDUP + ' ' +
                        leeHom_file_path + ' ' +
                        removedup_target + ' ' +
                        stats_file + '\n')

            # Write the echo sort statement for shell output
            makefile.write('\t' + "@echo -e '\e[96m" +
                    'Sorting reads by ID for sample: ' + leeHom_file.split('.')[0] +
                    '\e[0m' + "'" + '\n')

            # Write the sort command for shell output
            makefile.write('\t' +
                            '@cat ' + removedup_target + ' | ' +
                            'paste - - - - | sort -k1,1 -t " " | tr "\\t" "\\n" ' +
                            '> ' + sorted_target +
                            '\n')

	    makefile.write('\t' +
			    '@cp ' + stats_file + ' ' +
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

print( '\n' + TextColor.GREEN + 'Completed Module: NGSXremovedup')
# Reset text color, just in case
print(TextColor.NORMAL)
