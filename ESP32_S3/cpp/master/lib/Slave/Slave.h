/**
 * @file Slave.h
 * @author Mate Narh
 * 
 * Class for the master to control an SPI slave. This is an emulation of the slave
 * as seen from the master sider. However, in actuality, this slave object receives
 * its data from the actual slave MCU over SPI
 */

#ifndef SLAVE_H
#define SLAVE_H

#include <Arduino.h>
#include <SPI.h>

class Slave
{
    private:
        
        int mId = 0;                       ///< ID of this slave
        int mKeyCount = 0;                 ///< Number of keys monitored by this slave
        const uint8_t* mNotes = nullptr;   ///< Notes (pitches) associated with the keys monitored by this slave
 
        // ------------------- SPI Commmunication Objects & Variables ------------------

        SPIClass* mSpi = nullptr;          ///< The SPI object that this slave object uses to communicate with the other side
        uint32_t mSpiClock = 1000000;       ///< The clock speed for this slave's SPI object
        uint8_t mSpiBitOrder = MSBFIRST;   ///< The bit order for this slave's SPI object
        uint8_t mSpiDateMode = SPI_MODE0;  ///< The data mode for this slave's SPI object
        size_t mBufferSize = 0;            ///< The size of the reception buffer for this slave
        uint8_t* mReceiveBuffer = nullptr; ///< The buffer that this slave receives 

    public:

        Slave(const int id, const int keyCount, const uint8_t* notes);
        ~Slave();
    
        // ----------------------------------- Setters ---------------------------------
        void SetId(const int id);
        void SetKeyCount(int keyCount);
        void SetNotes(const uint8_t* notes);
        void SetSpi(SPIClass* spi);
        void SetReceiveBuffer(uint8_t* receiveBuffer);

        // ----------------------------------- Getters ---------------------------------
        int GetId() const;
        int GetKeyCount() const;
        const uint8_t* GetNotes() const;
        SPIClass* GetSpi() const;
        uint8_t* GetReceiveBuffer() const;
    
        // --------------------------------- Core Methods ------------------------------
        void SetSpiParameters(SPIClass* spi, uint32_t spiClock, uint8_t bitOrder, uint8_t dataMode, const size_t bufferSize, uint8_t* receiveBuffer);
        void querySPIPeerOnOtherSide();

        Slave() = delete;                        ///< Default constructor disabled 
        Slave(const Slave &) = delete;           ///< Copy constructor disabled
        void operator=(const Slave &) = delete;  ///< Assignment operator disabled
};


#endif // SLAVE_H