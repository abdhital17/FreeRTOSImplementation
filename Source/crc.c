const unsigned char CRC_POLY = 0x91;

unsigned char getCRC(unsigned char message[], unsigned char length)
{
    unsigned char i, j, crc = 0;

    for (i = 0; i < length; i++)
    {
        crc ^= message[i];

        for (j = 0; j < 8; j++)
        {
            if (crc & 1)
                crc ^= CRC_POLY;
            crc >>= 1;
        }
    }
    return crc;
}