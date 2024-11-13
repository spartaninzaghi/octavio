#include "Slave.h"
/**
 *
 */


/**
 * @brief Constructor
 * @param keyCount The number of keys monitored by this slave
 * @param notes The notes (picthes) bound to the keys monitored by this slave
 * @param bufferSize The size of the SPI reception buffer for this slave (should be in multiples of 4)
 */
Slave::Slave(const int id, const int keyCount, uint8_t *notes, uint8_t* receiveBuffer) :
    mId(id), mKeyCount(keyCount), mNotes(notes), mReceiveBuffer(receiveBuffer) 
{

}

/**
 * @brief Destructor
 */
Slave::~Slave()
{
    delete[] mNotes;
    delete[] mReceiveBuffer;
}

/**
 * @brief Set the ID for this slave
 * @param ID the ID to set to
 */
void Slave::SetId(const int id)
{
    mId = id;
}

/**
 * @brief Set the key count for this slave
 * @param keyCount The number of keys to set
 */
void Slave::SetKeyCount(int keyCount)
{
    mKeyCount = keyCount;
}

/**
 * @brief Set the notes (pitches) bound to the keys monitored by this slave
 * @param notes The collection of notes to set
 */
void Slave::SetNotes(uint8_t *notes)
{
    mNotes = notes;
}

/**
 * @brief Set the spi transmission object for this slave
 * @param spi The spi transmission object to set (VSPI/HSPI/FSPI)
 */
void Slave::SetSpi(SPIClass *spi)
{
    mSpi = spi;
}

/**
 * @brief Set the reception buffer for this slave
 * @param receiveBuffer The reception buffer to set
 */
void Slave::SetReceiveBuffer(uint8_t *receiveBuffer)
{
    mReceiveBuffer = receiveBuffer;
}

/**
 * @brief Get the ID of this slave
 */
int Slave::GetId() const
{
    return mId;
}

/**
 * @brief Get the number of keys monitored by this slave
 */
int Slave::GetKeyCount() const
{
    return mKeyCount;
}

/**
 * @brief Get the notes (pitches) bound to the keys monitored by this slave
 */
uint8_t* Slave::GetNotes() const
{
    return mNotes;
}

/**
 * @brief Get the SPI transmission object for this slave
 */
SPIClass *Slave::GetSpi() const
{
    return mSpi;
}

/**
 * @brief Get the receive buffer for this slave
 */
uint8_t* Slave::GetReceiveBuffer() const
{
    return mReceiveBuffer;
}

/**
 * @brief Query the slave side for MIDI updates, if any
 */
void Slave::querySPIPeerOnOtherSide()
{

}
