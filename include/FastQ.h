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
    void setRecord(std::string id, std::string sequence, std::string quality);

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
} // namespace FastQ

#endif // FASTQ_H
