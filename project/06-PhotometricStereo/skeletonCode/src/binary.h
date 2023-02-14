template <typename T>
void swapBytes(T& value)
{
    T temp = value;
    char* ptrByte_value = reinterpret_cast<char *>(&value);
            // let ptrByte_value point to value
    char* ptrByte_temp = reinterpret_cast<char *>(&temp);
            // let ptrByte_temp point to temp
    int numBytes = sizeof(value);
    for (int i=0; i<numBytes; i++)
    {
	ptrByte_value[i] = ptrByte_temp[numBytes-1-i];
    }
}


template <class T>
bool
readBinary(const char *what, T *where, FILE *fp)
{
    size_t count = fread(where,sizeof(T),1,fp);
    if (count != 1)
    {
	if (feof(fp))
	    cerr << "ERROR: end of file" << endl;
	else
	    perror(0);
	cerr << "ERROR: Couldn't read " << what << " from binary data file.";
	cerr << endl;
	return false;
    }
#ifndef _WIN32
    if (BYTE_ORDER == BIG_ENDIAN) swapBytes(*where);
#endif // _WIN32
    return true;
}

template <class T>
bool
writeBinary(const char *what, const T *where, FILE *fp)
{
    T copy = *where;
#ifndef _WIN32
    if (BYTE_ORDER == BIG_ENDIAN) swapBytes(copy);
#endif // _WIN32
    
    size_t count = fwrite(&copy,sizeof(T),1,fp);
    if (count != 1)
    {
	perror(0);
	cerr << "ERROR: Couldn't write " << what << " to binary data file.";
	cerr << endl;
	return false;
    }
    return true;
}

