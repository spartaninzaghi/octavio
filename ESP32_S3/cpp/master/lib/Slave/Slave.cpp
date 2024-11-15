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
Slave::Slave(const int id, const int keyCount, const uint8_t *notes) : mId(id), mKeyCount(keyCount), mNotes(notes) {}

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
void Slave::SetNotes(const uint8_t *notes)
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
const uint8_t* Slave::GetNotes() const
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
 * @brief Initialize SPI for this slave
 * @param spi The SPI object that this slave uses to poll updates from its peer
 * @param spiClock The SPI clock used by the master to synchronize transactions with this slave
 * @param bitOrder The bit order for this SPI communication
 * @param dataMode The data mode for this SPI communication
 * @param bufferSize The size of the reception buffer for this SPI communication
 * @param receiveBuffer The receive buffer used to collect data in this SPI communication
 */
void Slave::SetSpiParameters(SPIClass* spi, uint8_t spiClock, uint8_t bitOrder, uint8_t dataMode, const size_t bufferSize, uint8_t* receiveBuffer)
{
    mSpi = spi;
    mSpiClock = spiClock;
    mSpiBitOrder = bitOrder;
    mSpiDateMode = dataMode;
    mBufferSize = bufferSize;
    mReceiveBuffer =  receiveBuffer;
}

/**
 * @brief Query the slave side for MIDI updates, if any
 */
void Slave::querySPIPeerOnOtherSide()
{
    //
    // This slave receives data from its peer with the following partition
    // -----------------------------------------------------------
    // |   1 READINESS   |    2 VELOCITIES    |    3 STATUSES    |
    // -----------------------------------------------------------
    //
    mSpi->beginTransaction(SPISettings(mSpiClock, MSBFIRST, SPI_MODE0));
    digitalWrite(static_cast<uint8_t>(mSpi->pinSS()), LOW);
    mSpi->transferBytes(NULL, mReceiveBuffer, mBufferSize);
    digitalWrite(mSpi->pinSS(), HIGH);
    mSpi->endTransaction();
}
