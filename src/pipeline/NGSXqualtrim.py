"""
NGSXqualtrim:  Generate makefile NGSXremovedup.mk for automated quality and length filtering.
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


args = parser.parse_args()                                                      # Store command line arguments


#-------------------------------------------------------------------------------#
#                                  Variables                                    #
#-------------------------------------------------------------------------------#


# An attempt at cross-platform support, TO BE CHANGED IF USING WINDOWS
OS_SEP = "/"

# Remove duplicate binary
QUALTRIM = 'bin/NGSXqualtrim'



samples_dir = args.samples_directory                                            # Directory of sample directories
output_dir = args.output_directory                                              # Directory for output files
phred_base = args.phred_base
min_qual = args.min_qual
prop = args.proportion
min_length = args.min_length


# Safety Dir
safety_dir = output_dir + OS_SEP + 'safety'
if not os.path.exists(safety_dir):
    os.makedirs(safety_dir)


makefile = open('NGSXqualtrim.mk', 'w')                                        #Create the makefile
all_target_list = []                                                            # list of targets for makefile
clean_target_list = []                                                          # This module needs a separate for cleaning targets
samples_list = []                                                               # List of individual sample directories



#-------------------------------------------------------------------------------#
#                                  Processing                                   #
#-------------------------------------------------------------------------------#
print(TextColor.GREEN + 'Beginning Module: NGSXqualtrim\n')                       # Processing Begins

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
for removedup_ind_sample_dir in os.listdir(samples_dir):
    removedup_ind_sample_path = samples_dir + OS_SEP + removedup_ind_sample_dir
    for removedup_file in os.listdir(removedup_ind_sample_path):
        if removedup_file.endswith('.sort.fastq'):
            removedup_file_path = removedup_ind_sample_path + OS_SEP + removedup_file
            print (TextColor.RED + removedup_file_path)

            # Make directories for individual sample output
            output_dir_qual = output_dir + OS_SEP + removedup_file.split('.')[0] + "_qualtrim"
            if not os.path.exists(output_dir_qual):
                os.makedirs(output_dir_qual)
                clean_target_list.append(output_dir_qual)

            # Make qualtrim target
            qualtrim_target = (output_dir_qual + OS_SEP +
                                removedup_file.replace('.fastq','.qualtrim.fastq'))
            all_target_list.append(qualtrim_target)

            # Reject fastq file
            qualtrim_reject = (output_dir_qual + OS_SEP +
                                removedup_file.replace('.fastq','.qualtrim.reject.fastq'))

            #Stats file for percent unique
            stats_file = qualtrim_target.replace('.fastq','.stats')

            # Write qualtrim commands to makefile for execution
            makefile.write(qualtrim_target + ': ' + removedup_file_path + '\n')

            # Write the echo qualtrim statement for shell output
            makefile.write('\t' + "@echo -e '\e[31m" +
                    'Quality and length filtering for sample: ' + removedup_file.split('.')[0] +
                    '\e[0m' + "'" + '\n')

            makefile.write('\t' + '@' + QUALTRIM + ' ' +
                        str(phred_base) + ' ' +
                        str(min_qual) + ' ' +
                        str(prop) + ' ' +
                        str(min_length) + ' ' +
                        removedup_file_path + ' ' +
                        qualtrim_target + ' ' +
                        qualtrim_reject + ' ' +
                        stats_file + '\n')


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
