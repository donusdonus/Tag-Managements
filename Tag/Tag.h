#ifndef  ___TAG_H___
#define  ___TAG_H___

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "TagUtil.h"

using namespace TagSys;

class Tag
{

     friend void * TagSys::_calloc(isMemory memType, size_t elements, size_t elementSize);
     friend void TagSys::_free(void *src);
     friend uint8_t *TagSys::_memcpy(uint8_t *des ,uint8_t *src,size_t size);

private:
    /* data */
    isOption _option;
    RawMemory _name;       /**< Name of the component */
    RawMemory _data;       /**< Data buffer for the component */

public:
    Tag();
    ~Tag();

        size_t GetObjectByteSize()
        {
            return  1 +  // _option
                    1 +  // _len_of_name
                    _name.Size() + // byte name data
                    4 + // byte size of data
                    _data.Size() ; // byte data
        }

        template<typename T>
        bool Set(T value,size_t index = 0)
        {
            bool monitor;
            size_t a , b ;

            a = sizeof(T);
            b = SchematicPoint[_option._type].element_size;

            /* Check Elements Per Size is equal */
            monitor = (a == b);
            if(!monitor)
                return monitor;

            /* Check is not over index */
            b = GetArraySize();
            monitor = ((b-1) >= index);
            if(!monitor)
                return false;
            
            T *tmp = (T*)_data.value; 

            tmp[index] = value;

            return true ;
        }

        template<typename T>
        T Get(size_t index = 0)
        {
            bool monitor;

            monitor = (sizeof(T) == SchematicPoint[_option._type].element_size);
            if(!monitor)
                return T{};

            monitor = (GetArraySize()-1 >= index);
            if(!monitor)
                return T{};
            
            T *tmp = (T*)_data.value; 

            return tmp[index];
        }

        size_t GetArraySize();

        const char * MonitorInfo(bool include_bytesize = false);

        const char * MonitorValue(size_t index);

        bool SetName(const char *name);

        const char * GetName();

        void Free();

        RawMemory * GetRawBuffer();
};


#endif