#!/usr/bin/python
# -*- coding: utf-8 -*-
## @package taxid2name.py
#  Documentation for script taxid2name.py
#
#  Convert krona file of NCBI taxid to Taxonomic names
"""
taxid2name	Convert krona file of NCBI taxid to Taxonomic names.
Author:         Katherine Eaton     ktmeaton [at sign here] gmail.com
Date Created:   2016-0223
Date Edited:	2016-0302 - Uploaded to github, branch 1.0.3
"""



import sys                                                                     # Command line arguments
from collections import Counter                                                # Counting occurrences

#-------------------------------------------------------------------------------#
#                             Argument Parsing                                  #
#-------------------------------------------------------------------------------#

parser = argparse.ArgumentParser(description='Convert krona file of NCBI taxid to Taxonomic names.')
mandatoryArgs = parser.add_argument_group('Mandatory')

# Taxid file (names.dmp)
mandatoryArgs.add_argument('-n', '--names',
                    dest='names_file',
                    help='Path to NCBI names file (names.dmp)',
                    required=True)

# Krona classified file
mandatoryArgs.add_argument('-k', '--krona',
                    dest='krona_file',
                    help='Path to Krona classified file',
                    required=True)

# Output file (translated krona)
mandatoryArgs.add_argument('-o', '--output-krona-file',
                    dest='output_krona_file',
                    help='File to output translated Krona results',
                    required=True)
                    
# Input stats file
mandatoryArgs.add_argument('-i', '--input-stats',
                    dest='input_stats_file',
                    help='Input fasta stats file.',
                    required=True)
                    
# Output binned stats file
mandatoryArgs.add_argument('-b', '--output-binned',
                    dest='output_stats_file',
                    help='File to output annotated stats file with taxonomic bin.',
                    required=True)
                    
args = parser.parse_args()                                                      # Store command line arguments


#------------------------------------------------------------------------#
#                                  Files                                 #
#------------------------------------------------------------------------#
taxid_file_name = args.names_file
krona_file_name = args.krona_file
output_file_name = args.output_krona_file
input_stats_file_name = args.input_stats_file
output_stats_file_name = args.output_stats_file

taxid_file = open(taxid_file_name)
krona_file = open(krona_file_name, "r") 
output_file = open(output_file_name, "w")
input_stats_file = open(input_stats_file_name, 'r')
output_stats_file = open(output_stats_file_name, 'w')

#------------------------------------------------------------------------#
#                         Progress Meter Variables                       #
#------------------------------------------------------------------------#
bool_zero = False
bool_ten = False
bool_twenty = False
bool_thirty = False
bool_forty = False
bool_fifty = False
bool_sixty = False
bool_seventy = False
bool_eighty = False
bool_ninety = False

#------------------------------------------------------------------------#
#                            Processing Variables                        #
#------------------------------------------------------------------------#
taxid_name_dict = {}

total_taxid = 2115701 
num_taxid = 0


taxid_name_dict = {}
read_line = taxid_file.readline()


#------------------------------------------------------------------------#
#                         Create the Taxid Dict                          #
#------------------------------------------------------------------------#

print("Reading in the NCBI Taxonomy names file.")

while(read_line):
    split_line = read_line.split("|")
    taxid = split_line[0].strip()
    name = split_line[1].strip()
    name_type = split_line[3].strip()
    
    if name_type == "scientific name":
        taxid_name_dict[taxid] = name
        
    num_taxid += 1
    
    #--------------------------------------------------------------------#
    #                        Progress Meter                              #
    #--------------------------------------------------------------------#
    if (not bool_ninety and ((num_taxid == 0.9 * total_taxid) or (num_taxid > 0.9 * total_taxid))):
        print("90% processed.")
        bool_ninety = True
  
    elif (not bool_eighty and ((num_taxid == 0.8 * total_taxid) or (num_taxid > 0.8 * total_taxid))):
        print("80% processed.")
        bool_eighty = True	
        
    elif (not bool_seventy and ((num_taxid == 0.7 * total_taxid) or (num_taxid > 0.7 * total_taxid))):
        print("70% processed.")
        bool_seventy = True	
        
    elif (not bool_sixty and ((num_taxid == 0.6 * total_taxid) or (num_taxid > 0.6 * total_taxid))):
        print("60% processed.")
        bool_sixty = True	
        
    elif (not bool_fifty and ((num_taxid == 0.5 * total_taxid) or (num_taxid > 0.5 * total_taxid))):
        print("50% processed.")
        bool_fifty = True
        
    elif (not bool_forty and ((num_taxid == 0.4 * total_taxid) or (num_taxid > 0.4 * total_taxid))):
        print("40% processed.")
        bool_forty = True
        
    elif (not bool_thirty and ((num_taxid == 0.3 * total_taxid) or (num_taxid > 0.3 * total_taxid))):
        print("30% processed.")
        bool_thirty = True	
        
    elif (not bool_twenty and ((num_taxid == 0.2 * total_taxid) or (num_taxid > 0.2 * total_taxid))):
        print("20% processed.")
        bool_twenty = True	

    elif (not bool_ten and ((num_taxid == 0.1 * total_taxid) or (num_taxid > 0.1 * total_taxid))):
        print("10% processed.")
        bool_ten = True	

    elif (not bool_zero and ((num_taxid == 0.0 * total_taxid) or (num_taxid > 0.0 * total_taxid))):
        print("0% processed.")
        bool_zero = True   
        
    read_line = taxid_file.readline()

print("100% processed.")


#------------------------------------------------------------------------#
#                            Process Krona File                          #
#------------------------------------------------------------------------#
read_taxid_dict = {}
header = krona_file.readline()
output_file.write(header)
read_line = krona_file.readline()


print("Translating taxonomy IDs in the input Krona file.")

while(read_line):
    split_line = read_line.split()
    read_ID = split_line[0]
    read_taxid = split_line[1]
    read_evalue = split_line[2]
  
    try:
	taxid_name_dict[read_taxid]
	read_taxid_name = taxid_name_dict[read_taxid]
    except KeyError:
	read_taxid_name = 'Unknown taxid'

 
    read_taxid_dict[read_ID] = read_taxid_name        
    output_file.write(read_ID + "\t" + read_taxid_name + "\t" + read_evalue + "\n")
       


    read_line = krona_file.readline()
 
 
 
print("Creating the taxon integer dictionary")

tax_int_list = [value for value in read_taxid_dict.values()]
tax_int_list_count = Counter(tax_int_list).most_common()


tax_int_dict = {}                                       
index = 1
for taxon in tax_int_list_count:
    tax_int_dict[taxon[0]] = index
    index += 1
                                      


 
 
 
#------------------------------------------------------------------------#
#                            Update Stats File                           #
#------------------------------------------------------------------------#   
print("Appending taxonomy names to the contig stats file.")
classified_reads = read_taxid_dict.keys()

header = input_stats_file.readline().strip()
output_stats_file.write(header + "\t" + "BLASTbin" + "\t" + "tax.color" + "\n")
read_line = input_stats_file.readline()

while(read_line):
    split_line = read_line.split()
    read_id = split_line[0] 
    
    # Write all the columns from the original stats file
    for item in split_line:
        output_stats_file.write(item + "\t")
        
    # If the read was classified    
    if read_id in classified_reads:
        taxon = read_taxid_dict[read_id]
        output_stats_file.write(taxon + "\t" + str(tax_int_dict[taxon]) + "\n")
    # If the read was not classified 
    else:
        output_stats_file.write("Unclassified" + "\t" + "NA" + "\n")
    
    read_line = input_stats_file.readline()


#------------------------------------------------------------------------#
#                               Clean Up                                 #
#------------------------------------------------------------------------#
    
taxid_file.close()
krona_file.close()
output_file.close()
input_stats_file.close()
output_stats_file.close()

print("Finished!")
