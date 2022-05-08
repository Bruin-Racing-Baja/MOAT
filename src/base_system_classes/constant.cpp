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

bool Constant::save_constants(float* in)
{
    if (sizeof(in) > payload_size) return false;

    int address = find_free_address() + header_size;
    if (address == -1 + header_size) return false;

    EEPROM.put(address + header_size, in);
    int old_lifecycle;
    EEPROM.get(address, old_lifecycle);
    EEPROM.put(address, old_lifecycle + 1);
    return true;
}

float* Constant::read_memory()
{
    int address = find_free_address() + header_size;
    
    float out[5];
    EEPROM.get(address, out);
    return out;
}