#!/usr/bin/python
# -*- coding: utf-8 -*-
## @package NGSXqualtrim
#  Documentation for module NGSXqualtrim
#
#  More details.

"""
NGSXqualtrim:  Generate makefile NGSXqualtrim.mk for automated quality and length filtering.
Author:         Katherine Eaton     ktmeaton [at sign here] gmail.com
Date Created:   2016-0218
Date Edited:	2016-0302 - 	Uploaded to Github, Allow for absolute paths
"""

import os                                                                       # OS directory and file navigation
import argparse                                                                 # Command-line argument parsing

# I do not know how coloring output will work on windows platforms
from TextColor import TextColor                                                 # TextColor Class

#-------------------------------------------------------------------------------#
#                             Argument Parsing                                  #
#-------------------------------------------------------------------------------#

parser = argparse.ArgumentParser(description='Generate makefile NGSXqualtrim.mk for automated quality and length filtering.')
mandatoryArgs = parser.add_argument_group('Mandatory')

mandatoryArgs.add_argument('-d', '--dir',
                    dest='samples_directory',
                    help='Full path to directory of duplicate removed sample folders.',
                    required=True)

# Output directory
mandatoryArgs.add_argument('-o', '--output_dir',
                    dest='output_directory',
                    help='Output directory.',
                    required=True)

# Phred score base
mandatoryArgs.add_argument('--phred',
                    type = int,
                    dest='phred_base',
                    choices=[33,64],
                    help='Phred score base.',
                    required=True)

# Minimum quality
mandatoryArgs.add_argument('-q','--qual',
                    type = int,
                    dest='min_qual',
                    help='Minimum quality threshold.',
                    required=True)

# Minimum quality
mandatoryArgs.add_argument('-p','--prop',
                    type = float,
                    dest='proportion',
                    help='Proportion of bases in a read that must meet the minimum quality threshold.',
                    required=True)

# Minimum quality
mandatoryArgs.add_argument('-l','--length',
                    type = int,
                    dest='min_length',
                    help='Minimum length of read to keep.',
                    required=True)

# Read type (pe or se)
mandatoryArgs.add_argument('-r','--read-type',
		    choices=["se","pe"],
                    dest='read_type',
                    help='Read type (single-end or paired-end).',
                    required=True)     

# Input suffix
mandatoryArgs.add_argument('-s','-input-suffix',
                    dest='input_suffix',
                    help='Suffix of input read files.',
                    required=True)                      


args = parser.parse_args()                                                      # Store command line arguments


#-------------------------------------------------------------------------------#
#                                  Variables                                    #
#-------------------------------------------------------------------------------#

# An attempt at cross-platform support, TO BE CHANGED IF USING WINDOWS
OS_SEP = "/"

samples_dir = args.samples_directory                                            # Directory of sample directories
output_dir = args.output_directory                                              # Directory for output files
phred_base = args.phred_base
min_qual = args.min_qual
prop = args.proportion
min_length = args.min_length
read_type = args.read_type
input_suffix = args.input_suffix

# Safety Dir
safety_dir = output_dir + OS_SEP + 'safety'
if not os.path.exists(safety_dir):
    os.makedirs(safety_dir)

# Script path
script_path = os.path.realpath(__file__)					# Retrieve the path of the NGSXclassify.py file
split_path = script_path.split(OS_SEP)						# Split by operating system delimieter
NGSX_root_list = split_path[:len(split_path)-3]					# Jump up 2 levels to root directory
NGSX_root_path = ""								# Create the string of the NGSX root path
for directory in NGSX_root_list:
    NGSX_root_path += OS_SEP + directory
    
# Read type binary choice
if read_type == "se":
    QUALTRIM = NGSX_root_path + OS_SEP + 'bin' + OS_SEP + 'NGSXQualityControl'
elif read_type == "pe":
    QUALTRIM = NGSX_root_path + OS_SEP + 'bin' + OS_SEP + 'NGSXQualityControlPairedEnd'
else:
    raise ValueError("User-specified read type was not 'pe' or 'se'.")	


makefile = open('NGSXqualtrim.mk', 'w')                                         # Create the makefile
all_target_list = []                                                            # list of targets for makefile
clean_target_list = []                                                          # This module needs a separate for cleaning targets
samples_list = []                                                               # List of individual sample directories



#-------------------------------------------------------------------------------#
#                                  Processing                                   #
#-------------------------------------------------------------------------------#
print(TextColor.GREEN + 'Beginning Module: NGSXqualtrim\n'+ TextColor.RESET )   # Processing Begins

print(TextColor.RESET + "Using QualitryControl Binary: " + QUALTRIM + '\n' + TextColor.RESET)    

# Write the mandatory beginning of the makefile
makefile.write('SHELL := /bin/bash' + '\n\n')
makefile.write('Default:' + '\t' + 'all' + '\n\n')


# Collect path information to individual sample directories
for item in os.listdir(samples_dir):
    item_path = samples_dir + OS_SEP + item
    if os.path.isdir(item_path) and item.endswith("_removedup"):
        samples_list.append(item)

print(TextColor.BLUE + 'Automating quality and length filtering for sample directories: ' + str(samples_list) + '\n\n')

print(TextColor.BLUE + 'Creating commands to run quality and length filtering on the following files:\n')



#-------------------------------------------------------------------------------#
#                       Quality and Length Trimming                             #
#-------------------------------------------------------------------------------#
# Search for the merged output fastq files from leeHom
for ind_sample_dir in os.listdir(samples_dir):
    ind_sample_path = samples_dir + OS_SEP + ind_sample_dir
    for file in os.listdir(ind_sample_path):
        if file.endswith(input_suffix):
            file_path = ind_sample_path + OS_SEP + file
            print (TextColor.RED + file_path)

            # Make directories for individual sample output
            output_dir_qual = output_dir + OS_SEP + file.split('.')[0] + "_qualtrim"
            if not os.path.exists(output_dir_qual):
                os.makedirs(output_dir_qual)
                clean_target_list.append(output_dir_qual)

            # Make qualtrim target
            qualtrim_fastq1_target = (output_dir_qual + OS_SEP +
                                file.replace('.fastq','.qualtrim.fastq'))
            all_target_list.append(qualtrim_fastq1_target)

            #Stats file for percent unique
            stats_file = qualtrim_target.replace('.fastq','.stats')

            # Write qualtrim targets
            makefile.write(qualtrim_target + ': ' + file_path + '\n')
            
            if input_suffix.endswith('.gz'):
            	# Write the gunzip echo statement for shell output
            	makefile.write('\t' + "@echo -e '\e[31m" +
                    'Gunzipping file: ' + removedup_file.split('.')[0] +
                    '\e[0m' + "'" + '\n')

            # Write the echo qualtrim statement for shell output
            makefile.write('\t' + "@echo -e '\e[31m" +
                    'Quality and length filtering for sample: ' + removedup_file.split('.')[0] +
                    '\e[0m' + "'" + '\n')
	    
	    # Write qualtrim commands to makefile for execution
            makefile.write('\t' + '@' + QUALTRIM + ' ' +
                        str(phred_base) + ' ' +
                        str(min_qual) + ' ' +
                        str(prop) + ' ' +
                        str(min_length) + ' ' +
                        removedup_file_path + ' ' +
                        qualtrim_target + ' ' +
                        qualtrim_reject + ' ' +
                        stats_file + '\n')

	    # Write command to transfer stats file to the safety directory
   	    makefile.write('\t' + '@cp ' +
			stats_file + ' ' +
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

print( '\n' + TextColor.GREEN + 'Completed Module: NGSXqualtrim')
# Reset text color, just in case
print(TextColor.NORMAL)
