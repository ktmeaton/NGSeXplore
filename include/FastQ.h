/*! \file FastQ.h
 *  FastQ Class Declaration.
 *  \verbinclude FastQ.h
 */


#ifndef FASTQ_H
#define FASTQ_H

#include <string>


namespace FastQ{
/** \class FastQ
 *  \brief A class to hold a fastq record.
 *
 */
class FastQ
  {
  //-------------------------------PRIVATE---------------------------------//
  private:
    std::string _id;                       /**<Sequence idenfitier. */
    std::string _sequence;                 /**<Nucleotide sequence. */
    std::string _line3;
    std::string _quality;                  /**<Sequence quality. */
    int _length;                           /**<Sequence length. */
    float _GC;                             /**<Sequence GC content. */
    float _av_qual;                        /**<Sequence average quality. */

    void setLength();                        /**<Calcualate and set length. */
    void setGC();                            /**<Calcualate and set GC content. */
    void setAvQual();                        /**<Calcualate and set average quality. */



  //-------------------------------PUBLIC----------------------------------//
  public:
    /**
     * \fn Constructor
     * \brief Constructs the FastQ object.
     * Constructs the FastQ object with empty ID, sequence, and quality.
     */
    FastQ();

      /** \fn Destructor */
    ~FastQ();

    /**
     * \fn setRecord
     * \brief Stores FastQ record information.
     *
     *Reads the fastq record and stores ID, sequence, and quality.
     * @param id sequence identifier
     * @param sequence nucleotide sequence
     * @param quality nucleotide qualities
     */
    void setRecord(std::string id, std::string sequence, std::string line3, std::string quality);

    /**
     * \fn delRecord
     * \brief Deletes FastQ record information.
     *
     * Delete ID, sequence, and quality.
     */
    void delRecord();

    /**
     * \fn getID
     * \brief Returns the associated sequence record ID.
     * @return ID
     */
    std::string getID();

    /**
     * \fn getSeq
     * \brief Returns the associated sequence record nucleotide sequence.
     * @return Sequence
     */
    std::string getSeq();

    /**
     * \fn getLine3
     * \brief Returns the associated sequence record line 3.
     * @return Line3
     */
    std::string getLine3();

    /**
     * \fn getQual
     * \brief Returns the associated sequence record quality.
     * @return Quality
     */
    std::string getQual();

    /**
     * \fn getLength
     * \brief Returns the associated sequence record length.
     * @return Length
     */
    int getLength();

    /**
     * \fn getGC
     * \brief Returns the associated sequence record average GC content.
     * @return GC Content
     */
    float getGC();

    /**
     * \fn getAvQual
     * \brief Returns the associated sequence record average quality.
     * @return Average Quality
     */
    float getAvQual();
};

/** \class FastQPaired
 *  \brief A class to hold a paired fastq record.
 *
 */
class FastQPaired
  {
  //-------------------------------PRIVATE---------------------------------//
  private:
    std::string _id_first;                       /**<Sequence idenfitier. */
    std::string _sequence_first;                 /**<Nucleotide sequence. */
    std::string _line3_first;
    std::string _quality_first;                  /**<Sequence quality. */

    std::string _id_second;                       /**<Sequence idenfitier. */
    std::string _sequence_second;                 /**<Nucleotide sequence. */
    std::string _line3_second;
    std::string _quality_second;                  /**<Sequence quality. */


  //-------------------------------PUBLIC----------------------------------//
  public:
    /**
     * \fn Constructor
     * \brief Constructs the FastQPaired object.
     * Constructs the FastQPaired object with empty ID, sequence, and quality.
     */
    FastQPaired();

      /** \fn Destructor */
    ~FastQPaired();

    /**
     * \fn setRecord
     * \brief Stores FastQPaired record information.
     *
     *Reads the fastq record and stores ID, sequence, and quality.
     * @param id sequence identifier
     * @param sequence nucleotide sequence
     * @param quality nucleotide qualities
     */
    void setRecord(std::string id_first, std::string id_second,
                std::string sequence_first, std::string sequence_second,
                std::string line3_first, std::string line3_second,
                std::string quality_first, std::string quality_second);

    /**
     * \fn delRecord
     * \brief Deletes FastQ record information.
     *
     * Delete ID, sequence, and quality.
     */
    void delRecord();

    /**
     * \fn getIDFirst
     * \brief Returns the associated sequence record ID.
     * @return ID
     */
    std::string getIDFirst();

    /**
     * \fn getIDFirst
     * \brief Returns the associated sequence record ID.
     * @return ID
     */
    std::string getIDSecond();

    /**
     * \fn getSeq
     * \brief Returns the associated first sequence record nucleotide sequence.
     * @return Sequence
     */
    std::string getSeqFirst();

    /**
     * \fn getSeq
     * \brief Returns the associated second sequence record nucleotide sequence.
     * @return Sequence
     */
    std::string getSeqSecond();

    /**
     * \fn getLine3
     * \brief Returns the associated first sequence record Line 3.
     * @return Line3
     */
    std::string getLine3First();

    /**
     * \fn getLine3
     * \brief Returns the associated second sequence record Line 3.
     * @return Line3
     */
    std::string getLine3Second();

    /**
     * \fn getQualFirst
     * \brief Returns the associated sequence record quality.
     * @return Quality
     */
    std::string getQualFirst();

    /**
     * \fn getQualSecond
     * \brief Returns the associated sequence record quality.
     * @return Quality
     */
    std::string getQualSecond();


};
} // namespace FastQ

#endif // FASTQ_H
