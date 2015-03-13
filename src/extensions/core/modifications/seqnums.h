#ifndef EXTENSIONS_CORE_MODIFICATIONS_SEQNUMS_H
#define EXTENSIONS_CORE_MODIFICATIONS_SEQNUMS_H

#include <vector>

/** Inserts sequence numbers into a stream of raw bytes. */
std::vector<unsigned char> AddSequenceNumbers(const std::vector<unsigned char>& vch, unsigned char nFirstSeqNum = 0x01);
/** Removes sequence numbers from a stream of raw bytes. */
std::vector<unsigned char> RemoveSequenceNumbers(const std::vector<unsigned char>& vch);

/** Inserts sequence numbers into a stream of raw bytes. */
void AddSequenceNumbersIn(std::vector<unsigned char>& vch, unsigned char nFirstSeqNum = 0x01);
/** Removes sequence numbers from a stream of raw bytes. */
void RemoveSequenceNumbersIn(std::vector<unsigned char>& vch);

#endif // EXTENSIONS_CORE_MODIFICATIONS_SEQNUMS_H
