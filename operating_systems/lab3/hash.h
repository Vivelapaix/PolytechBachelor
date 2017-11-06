static int get_hash(const char* source)
{    
    if (source == 0) 
    {
        return 0;
    }

    int hash = 0;
    while (*source != '\0') 
    {
        char c = *source++;
        int a = c - '0';
        hash = (hash * 10) + a;     
    } 
    return hash;
}
