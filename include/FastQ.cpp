/*! \file FastQ.cpp
    FastQ Class Implementation.
    \verbinclude FastQ.cpp
*/


#include <iostream>
#include <string>
#include <algorithm>                                 // Counting char occurences
#include "FastQ.h"
#include <math.h>     /* log10 */
#include <cmath>      /* power exponents */

namespace FastQ
{
    //------------------------------Constructor---------------------------------//
    FastQ::FastQ()
    {
        _id = "";
        _sequence = "";
        _line3 = "";
        _quality = "";
        _length = 0;
        _GC = 0;
        _av_qual = 0;
    }

    //------------------------------Destructor---------------------------------//
    FastQ::~FastQ()
    {

    }

    //-------------------------Set and Delete Record---------------------------//
    void FastQ::setRecord( std::string id, std::string sequence, std::string line3,
                    std::string quality )
    {
        _id = id;
        _sequence = sequence;
        _line3 = line3;
        _quality = quality;
        FastQ::setLength();
        FastQ::setGC();
    }

    void FastQ::delRecord()
    {
        _id = "";
        _sequence = "";
        _line3 = "";
        _quality = "";
    }

    //--------------------------Calculate Attributes----------------------------//
    // Length
    void FastQ::setLength()
    {
        _length = _sequence.length();
    }
    // GC Content
    void FastQ::setGC()
    {
        _GC = ( ( std::count( _sequence.begin(), _sequence.end(), 'G' ) +
                std::count( _sequence.begin(), _sequence.end(), 'C' ) ) /
                                        double( _length ) * 100 );
    }
    // Average Quality
    void FastQ::setAvQual(int phred_encode)
    {

        double total_probability = 0;
        double average_probability;
	int char_phred_qual;
	double char_phred_prob;

        // Iterate through sequence, adding up quality probability of each nucleotide.
        for ( int i = 0; i < _length; i++ )
        {
            char_phred_qual = int(_sequence[i]) - phred_encode;
            char_phred_prob = std::pow(10,(-1 * char_phred_qual / 10));
            total_probability += char_phred_prob;
        }

        average_probability = total_probability / double(_length);
        _av_qual = -10*(log10(average_probability));

    }

    //-----------------------------Get Attributes-------------------------------//
    std::string FastQ::getID()
    {
        return _id;
    }

    std::string FastQ::getSeq()
    {
        return _sequence;
    }

    std::string FastQ::getLine3()
    {
        return _line3;
    }

    std::string FastQ::getQual()
    {
        return _quality;
    }

    int FastQ::getLength()
    {
        return _length;
    }

    float FastQ::getGC()
    {
        return _GC;
    }

    float FastQ::getAvQual()
    {
        return _av_qual;
    }



    //------------------------------Constructor---------------------------------//
    FastQPaired::FastQPaired()
    {
        _id_first = "";
        _sequence_first = "";
        _line3_first = "";
        _quality_first = "";

        _id_second = "";
        _sequence_second = "";
        _line3_second = "";
        _quality_second = "";

    }

    //------------------------------Destructor---------------------------------//
    FastQPaired::~FastQPaired()
    {

    }

    //-------------------------Set and Delete Record---------------------------//
    void FastQPaired::setRecord( FastQ fastq_first,
                    FastQ fastq_second )
    {
        _id_first = fastq_first.getID();
        _sequence_first = fastq_first.getSeq();
        _line3_first = fastq_first.getLine3();
        _quality_first = fastq_first.getQual();

        _id_second = fastq_second.getID();
        _sequence_second = fastq_second.getSeq();
        _line3_second = fastq_second.getLine3();
        _quality_second = fastq_second.getQual();

    }

    void FastQPaired::delRecord()
    {
        _id_first = "";
        _sequence_first = "";
        _line3_first = "";
        _quality_first = "";

        _id_second = "";
        _sequence_second = "";
        _line3_second = "";
        _quality_second = "";
    }


    //-----------------------------Get Attributes-------------------------------//
    std::string FastQPaired::getIDFirst()
    {
        return _id_first;
    }

    std::string FastQPaired::getIDSecond()
    {
        return _id_second;
    }

    std::string FastQPaired::getSeqFirst()
    {
        return _sequence_first;
    }

    std::string FastQPaired::getSeqSecond()
    {
        return _sequence_second;
    }

    std::string FastQPaired::getLine3First()
    {
        return _line3_first;
    }

    std::string FastQPaired::getLine3Second()
    {
        return _line3_second;
    }

    std::string FastQPaired::getQualFirst()
    {
        return _quality_first;
    }

    std::string FastQPaired::getQualSecond()
    {
        return _quality_second;
    }


} // namespace FastQ
