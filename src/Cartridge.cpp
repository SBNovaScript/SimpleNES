#include "Cartridge.h"
#include "Log.h"
#include "Mapper.h"
#include <fstream>
#include <string>
#include <array>

namespace sn
{
    /**
     * Default constructor for Cartridge class, initializes member variables.
     */
    Cartridge::Cartridge() :
        m_nameTableMirroring(0),
        m_mapperNumber(0),
        m_extendedRAM(false),
        m_hasBattery(false)
    {
    }

    /**
     * Returns a reference to the PRG-ROM data.
     */
    const std::vector<Byte>& Cartridge::getROM() const
    {
        return m_PRG_ROM;
    }

    /**
     * Returns a reference to the CHR-ROM data.
     */
    const std::vector<Byte>& Cartridge::getVROM() const
    {
        return m_CHR_ROM;
    }

    /**
     * Returns the mapper number.
     */
    Byte Cartridge::getMapper() const
    {
        return m_mapperNumber;
    }

    /**
     * Returns the name table mirroring mode.
     */
    Byte Cartridge::getNameTableMirroring() const
    {
        return m_nameTableMirroring;
    }

    /**
     * Returns whether the cartridge has battery-backed RAM.
     */
    bool Cartridge::hasBattery() const
    {
        return m_hasBattery;
    }

    /**
     * Read header values and set member variables accordingly.
     * @param header: Array containing header values from the ROM file.
     */
    void Cartridge::readHeaderValues(const std::array<Byte, 0x10>& header)
    {
        // Set name table mirroring mode
        if (header[6] & 0x8)
        {
            m_nameTableMirroring = NameTableMirroring::FourScreen;
            LOG(Info) << "Name Table Mirroring: " << "FourScreen" << std::endl;
        }
        else
        {
            m_nameTableMirroring = header[6] & 0x1;
            LOG(Info) << "Name Table Mirroring: " << (m_nameTableMirroring == 0 ? "Horizontal" : "Vertical") << std::endl;
        }

        // Set mapper number
        m_mapperNumber = ((header[6] >> 4) & 0xf) | (header[7] & 0xf0);
        LOG(Info) << "Mapper #: " << +m_mapperNumber << std::endl;

        // Set extended RAM
        m_extendedRAM = header[6] & 0x2;
        LOG(Info) << "Extended (CPU) RAM: " << std::boolalpha << m_extendedRAM << std::endl;

        // Set battery-backed RAM
        m_hasBattery = header[6] & 0x2;
        LOG(Info) << "Battery-backed RAM: " << std::boolalpha << m_hasBattery << std::endl;
    }

    /**
     * Loads a ROM from a file at the given path.
     * Returns true if successful, otherwise false.
     */
    bool Cartridge::loadFromFile(std::string path)
    {
        std::ifstream romFile (path, std::ios_base::binary | std::ios_base::in);
        if (!romFile)
        {
            LOG(Error) << "Could not open ROM file from path: " << path << std::endl;
            return false;
        }

        std::array<Byte, 0x10> header;
        LOG(Info) << "Reading ROM from path: " << path << std::endl;

        // Read the iNES header
        if (!romFile.read(reinterpret_cast<char*>(header.data()), header.size()))
        {
            LOG(Error) << "Reading iNES header failed." << std::endl;
            return false;
        }

        // Check if the file is a valid iNES image
        if (std::string{header.begin(), header.begin() + 4} != "NES\x1A")
        {
            LOG(Error) << "Not a valid iNES image. Magic number: "
                      << std::hex << header[0] << " "
                      << header[1] << " " << header[2] << " " << int(header[3]) << std::endl
                      << "Valid magic number : N E S 1a" << std::endl;
            return false;
        }

        // Read the header values
        LOG(Info) << "Reading header, it dictates: \n";
        readHeaderValues(header);

        Byte banks = header[4];
        LOG(Info) << "16KB PRG-ROM Banks: " << +banks << std::endl;
        if (!banks)
        {
            LOG(Error) << "ROM has no PRG-ROM banks. Loading ROM failed." << std::endl;
            return false;
        }

        Byte vbanks = header[5];
        LOG(Info) << "8KB CHR-ROM Banks: " << +vbanks << std::endl;

        // Check for unsupported features
        if (header[6] & 0x4)
        {
            LOG(Error) << "Trainer is not supported." << std::endl;
            return false;
        }

        if ((header[0xA] & 0x3) == 0x2 || (header[0xA] & 0x1))
        {
            LOG(Error) << "PAL ROM not supported." << std::endl;
            return false;
        }
        else
            LOG(Info) << "ROM is NTSC compatible.\n";

        // Read PRG-ROM 16KB banks
        m_PRG_ROM.resize(0x4000 * banks);
        if (!romFile.read(reinterpret_cast<char*>(&m_PRG_ROM[0]), 0x4000 * banks))
        {
            LOG(Error) << "Reading PRG-ROM from image file failed." << std::endl;
            return false;
        }

        // Read CHR-ROM 8KB banks
        if (vbanks)
        {
            m_CHR_ROM.resize(0x2000 * vbanks);
            if (!romFile.read(reinterpret_cast<char*>(&m_CHR_ROM[0]), 0x2000 * vbanks))
            {
                LOG(Error) << "Reading CHR-ROM from image file failed." << std::endl;
                return false;
            }
        }
        else
            LOG(Info) << "Cartridge with CHR-RAM." << std::endl;
        return true;
    }
}
