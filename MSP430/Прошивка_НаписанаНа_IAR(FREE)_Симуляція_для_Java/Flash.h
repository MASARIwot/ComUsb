
const unsigned char * INFOMEM = (unsigned char *) 0x1000;

// Erase entire information memory
int erase(void)
{
    unsigned char * segment;
    segment = (unsigned char *) INFOMEM;
   
    FCTL2 = FWKEY + FSSEL_1 + FN0 + FN1; // Select MCLK/3 (1.04MHz / 3)

    FCTL3 = FWKEY; // Unlock flash memory
    FCTL1 = FWKEY + ERASE; // Set erase segment mode   
    *segment = 0; // Dummy write to segment we are erasing
    while((FCTL3 & WAIT) != WAIT);
 
    segment += 128; // Point to next segment
    FCTL3 = FWKEY; // Unlock flash memory
    FCTL1 = FWKEY + ERASE; // Set erase segment mode   
    *segment = 0; // Dummy write to segment we are erasing
    while((FCTL3 & WAIT) != WAIT); // Wait for erase to complete

    FCTL3 = FWKEY + LOCK; // Lock flash memory
    return 1;
}

// Save a byte to the information memory
int save(char data, unsigned char address)
{
    char * segment;
    segment = (char *) INFOMEM;
   
    FCTL2 = FWKEY + FSSEL_1 + FN0 + FN1; // Select MCLK/3 (1.04MHz / 3)

    FCTL3 = FWKEY; // Unlock flash memory
    FCTL1 = FWKEY + WRT; // Set write byte mode   
    segment[address] = data; // Write byte
    while((FCTL3 & WAIT) != WAIT); // Wait for write to complete
    FCTL3 = FWKEY + LOCK; // Lock flash memory
    return address;
}
int savedouble(double data, int offset)
{// Save a double into infomem
    char * charptr;
    int i;
    charptr = (char *) &data;
   
    for(i=0;i<4;i++)
    {
        save(charptr[i], i + (offset * 4));
    }           
    return 0;   
}

char read(unsigned char address)
{
    char * segment;
    segment = (char *) INFOMEM;
    return segment[address];
}
double readdouble(int offset)
{// Read a double from infomem
    double data;
    int i;
    char * charptr;
    charptr = (char *) &data;
   
    for(i=0;i<4;i++)
    {
        charptr[i] = read(i + (offset * 4));
    }           
    return data;   
}


