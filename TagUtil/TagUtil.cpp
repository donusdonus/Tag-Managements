#include "TagUtil.h"
#include <cstdlib>
#include <string.h>

/* Modify Replace std function */

     void * TagSys::_calloc(isMemory memType, size_t elements, size_t elementSize)
    {
        switch (memType)
        {
        case isMemory::RAM:
            return std::calloc(elements, elementSize);
            break;
        case isMemory::PSRAM:
            return std::calloc(elements, elementSize);
            break;
        case isMemory::SPARE_1:
            return std::calloc(elements, elementSize);
            break;
        case isMemory::SPARE_2:
            return std::calloc(elements, elementSize);
            break;
        default:
            return std::calloc(elements, elementSize);
            break;
        }
        return nullptr;
    }

     void  TagSys::_free(void *src)
    {
        free(src);
    }
 
     uint8_t* TagSys::_memcpy(uint8_t *des , uint8_t *src,size_t size)
     {
        uint8_t *p = src;

        while(size >= 4)
        { 
            *(uint32_t*)des = *(uint32_t*)p ;
            p+=4;
            des+=4;
            size-=4;
        }

        while(size >= 2)
        { 
            *(uint16_t*)des = *(uint16_t*)p ;
            p+=2;
            des+=2;
            size-=2;
        }

        while(size >= 1)
        { 
            *des = *p ;
            p+=1;
            des+=1;
            size-=1;
        }

        return des;
     }

/* Common function */

     bool   TagSys::_setName(isMemory memType,RawMemory *src,const char *name)
     {

        /* Clear Memory First */
        if(src != nullptr)
        {
            _free(src);
            src = nullptr;
        }

        /* Calloc For New Address RawMemory */
        src = (RawMemory*) _calloc(memType,sizeof(RawMemory),1);
        if(src == nullptr)
            return false;

        /* Calloc For New Address RawMemory::size */
        src->size = strlen(name) + 1;
        /* Calloc For New Address RawMemory::Value */
        src->value = (uint8_t*) _calloc(memType,src->size,sizeof(char));
        if(src->value == nullptr)
        {
            _free(src);
            return false;
        }

        /* Copy name */
        TagSys::_memcpy(src->value,(uint8_t*)name,src->size-1);
        /* mark end char to null */
        src->value[src->size-1] = MARK_CHAR_NULL;

        return true;
     }    
