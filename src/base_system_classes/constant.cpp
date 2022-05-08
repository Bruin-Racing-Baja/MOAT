#include <Constant.h>
#include <EEPROM.h>

int Constant::find_free_address()
{
    int address = 0;
    for (address; address < eeprom_size; address += payload_size + header_size)
    {
        if (EEPROM.read(address) < eeprom_lifecycle) return address;
    }
    return -1;
}

bool Constant::save_constants(int address, float* in)
{
    if (sizeof(in) > payload_size) return false;
    EEPROM.put(address + header_size, in);
    int old_lifecycle;
    EEPROM.get(address, old_lifecycle);
    EEPROM.put(address, old_lifecycle + 1);
    return true;
}

float* Constant::read_memory(int address)
{
    float out[4];
    EEPROM.get(address, out);
}