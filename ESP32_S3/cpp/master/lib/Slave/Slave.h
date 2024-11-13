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
#include <USB-MIDI.h>
#include <SPI.h>

class Slave
{
    private:
        int mId = 0;                       ///< ID of this slave
        int mKeyCount = 0;                 ///< Number of keys monitored by this slave
        uint8_t* mNotes = nullptr;         ///< Notes (pitches) associated with the keys monitored by this slave

        SPIClass* mSpi = nullptr;          ///< The SPI object that this slave object uses to communicate with the other side
        int mBufferSize = 0;               ///< The size of the reception buffer for this slave
        uint8_t* mReceiveBuffer = nullptr; ///< The buffer that this slave receives 

    public:

        Slave(const int id, const int keyCount, uint8_t* notes, uint8_t* receiveBuffer);
        ~Slave();
    
        // ----------------------------------- Setters ---------------------------------
        void SetId(const int id);
        void SetKeyCount(int keyCount);
        void SetNotes(uint8_t* notes);
        void SetSpi(SPIClass* spi);
        void SetReceiveBuffer(uint8_t* receiveBuffer);

        // ----------------------------------- Getters ---------------------------------
        int GetId() const;
        int GetKeyCount() const;
        uint8_t* GetNotes() const;
        SPIClass* GetSpi() const;
        uint8_t* GetReceiveBuffer() const;
    
        // --------------------------------- Core Methods ------------------------------
        void querySPIPeerOnOtherSide();

        Slave() = delete;
        Slave(const Slave &) = delete;
        void operator=(const Slave &) = delete;
};


#endif // SLAVE_H