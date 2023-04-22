#include "PictureBus.h"
#include "Log.h"

namespace sn
{
    // Constants for readability
    enum : Address {
        CHR_ROM_BEGIN = 0x0000,
        CHR_ROM_END = 0x1FFF,
        NAME_TABLES_BEGIN = 0x2000,
        NAME_TABLES_END = 0x3EFF,
        PALETTE_BEGIN = 0x3F00,
        PALETTE_END = 0x3FFF,
        NAME_TABLE_MIRROR_OFFSET = 0x1000
    };

    /**
     * Constructor for PictureBus
     */
    PictureBus::PictureBus() :
        m_palette(0x20),
        m_RAM(0x800),
        m_mapper(nullptr)
    {}

    /**
     * Destructor for PictureBus
     */
    PictureBus::~PictureBus() {
        m_mapper = nullptr;
    }

    /**
     * Read data from PictureBus memory
     * @param addr Address to read from
     * @return Byte value at the given address
     */
    Byte PictureBus::read(Address addr) const
    {
        if (addr <= CHR_ROM_END)
        {
            return m_mapper->readCHR(addr);
        }
        else if (addr <= NAME_TABLES_END)
        {
            const auto index = addr & 0x3ff;
            // Name tables up to 0x3000, then mirrored up to 0x3eff
            auto normalizedAddr = (addr >= 0x3000) ? addr - NAME_TABLE_MIRROR_OFFSET : addr;

            if (NameTable0 >= m_RAM.size())
                return m_mapper->readCHR(normalizedAddr);
            else if (normalizedAddr < 0x2400)
                return m_RAM[NameTable0 + index];
            else if (normalizedAddr < 0x2800)
                return m_RAM[NameTable1 + index];
            else if (normalizedAddr < 0x2c00)
                return m_RAM[NameTable2 + index];
            else
                return m_RAM[NameTable3 + index];
        }
        else if (addr <= PALETTE_END)
        {
            auto paletteAddr = addr & 0x1f;
            // Addresses $3F10/$3F14/$3F18/$3F1C are mirrors of $3F00/$3F04/$3F08/$3F0C
            if (paletteAddr >= 0x10 && paletteAddr % 4 == 0) {
                paletteAddr &= 0xf;
            }
            return m_palette[paletteAddr];
        }
        return 0;
    }

    /**
     * Write data to PictureBus memory
     * @param addr Address to write to
     * @param value Byte value to write
     */
    void PictureBus::write(Address addr, Byte value)
    {
        if (addr <= CHR_ROM_END)
        {
            m_mapper->writeCHR(addr, value);
        }
        else if (addr <= NAME_TABLES_END)
        {
            const auto index = addr & 0x3ff;
            // Name tables up to 0x3000, then mirrored up to 0x3eff
            auto normalizedAddr = (addr >= 0x3000) ? addr - NAME_TABLE_MIRROR_OFFSET : addr;

            if (NameTable0 >= m_RAM.size())
                m_mapper->writeCHR(normalizedAddr, value);
            else if (normalizedAddr < 0x2400)
                m_RAM[NameTable0 + index] = value;
            else if (normalizedAddr < 0x2800)
                m_RAM[NameTable1 + index] = value;
            else if (normalizedAddr < 0x2c00)
                m_RAM[NameTable2 + index] = value;
            else
                m_RAM[NameTable3 + index] = value;
        }
        else if (addr <= PALETTE_END)
        {
            auto palette = addr & 0x1f;
            // Addresses $3F10/$3F14/$3F18/$3F1C are mirrors of $3F00/$3F04/$3F08/$3F0C
            if (palette >= 0x10 && addr % 4 == 0) {
                palette &= 0xf;
            }
            m_palette[palette] = value;
        }
    }

    /**
     * Update nametable mirroring, based on the mapper configuration
     */
    void PictureBus::updateMirroring()
    {
        switch (m_mapper->getNameTableMirroring())
        {
            case Horizontal:
                NameTable0 = NameTable1 = 0;
                NameTable2 = NameTable3 = 0x400;
                break;
            case Vertical:
                NameTable0 = NameTable2 = 0;
                NameTable1 = NameTable3 = 0x400;
                break;
            case OneScreenLower:
                NameTable0 = NameTable1 = NameTable2 = NameTable3 = 0;
                break;
            case OneScreenHigher:
                NameTable0 = NameTable1 = NameTable2 = NameTable3 = 0x400;
                break;
            case FourScreen:
                NameTable0 = m_RAM.size();
                break;
            default:
                NameTable0 = NameTable1 = NameTable2 = NameTable3 = 0;
                LOG(Error) << "Unsupported Name Table mirroring : " << m_mapper->getNameTableMirroring() << std::endl;
        }
    }

    /**
     * Set the mapper for the PictureBus
     * @param mapper Pointer to the Mapper object
     * @return True if the mapper is set successfully, False otherwise
     */
    bool PictureBus::setMapper(Mapper *mapper)
    {
        if (!mapper)
        {
            LOG(Error) << "Mapper argument is nullptr" << std::endl;
            return false;
        }

        m_mapper = mapper;
        updateMirroring();
        return true;
    }

    /**
     * Trigger a scanline IRQ event
     */
    void PictureBus::scanlineIRQ(){
        m_mapper->scanlineIRQ();
    }
}
