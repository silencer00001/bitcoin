#ifndef EXTENSIONS_CORE_MODIFICATIONS_PACKETS_H
#define EXTENSIONS_CORE_MODIFICATIONS_PACKETS_H

#include <stddef.h>
#include <vector>

/**
 * Pads the payload to the size of completely filled data packets.
 *
 * To ensure empty space is not filled with zeros during the public key conversion, the
 * payload should be padded before obfuscation, so that the fillers are also obfuscated.
 *
 * @param vchPayload[in,out]  The payload to modify
 * @param nPacketSize[in]     The packet size (defaults to 31 byte)
 */
void PadBeforeObfuscationIn(std::vector<unsigned char>& vchPayload, size_t nPacketSize = 31)
{
    const size_t nSize = vchPayload.size();
    const size_t nPackets = (nSize / nPacketSize) + (nSize % nPacketSize != 0);

    vchPayload.resize(nPackets * nPacketSize);
}

#endif // EXTENSIONS_CORE_MODIFICATIONS_PACKETS_H
