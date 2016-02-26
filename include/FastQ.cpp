/*! \file FastQ.cpp
 *  FastQ Class Implementation.
 *  \verbinclude FastQ.cpp
 */


#include <iostream>
#include <string>
#include <algorithm>                                 // Counting char occurences
#include "FastQ.h"

namespace FastQ
{
  //------------------------------Constructor---------------------------------//
  FastQ::FastQ()
  {
    _id = "";
    _sequence = "";
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
  void FastQ::setRecord(std::string id, std::string sequence, std::string quality)
  {
    _id = id;
    _sequence = sequence;
    _quality = quality;
    FastQ::setLength();
    FastQ::setGC();
    FastQ::setAvQual();
  }

  void FastQ::delRecord()
  {
    _id = "";
    _sequence = "";
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
    _GC = ((std::count(_sequence.begin(), _sequence.end(), 'G') +
              std::count(_sequence.begin(), _sequence.end(), 'C')) /
              double(_length) * 100);
  }
  // Average Quality
  void FastQ::setAvQual()
  {
    int total_quality = 0;
    // Iterate through sequence, adding up quality score of each nucleotide.
    for(int i = 0; i < _length; i++) {total_quality += int(_sequence[i]) - 33;}
    _av_qual = total_quality / double(_length);

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
} // namespace FastQ
