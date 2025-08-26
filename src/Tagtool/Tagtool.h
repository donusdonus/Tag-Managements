#ifndef __TAG_TOOL__H__
#define __TAG_TOOL__H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define MARK_CHAR_NULL 0x00
#define MARK_FREE_MEMORY 0x0FF
#define MAX_BUFFER_PRINT 128
#define _SECTOR_RAM_(x,y) calloc(x,y)
#define _SECTOR_PSRAM_(x,y) calloc(x,y)
#define _SECTOR_SPARE1_(x,y) calloc(x,y)
#define _SECTOR_SPARE2_(x,y) calloc(x,y)
#define RAM_FREE(x) free(x)

#pragma pack(push,1)
struct TypeInfo {
    const char* name;       
    uint8_t element_size;        
};
#pragma pack(pop)

#pragma pack(push,1)
enum isType : uint8_t
{
    VAR_CHAR,     /**< Single character type */
    VAR_INT8,     /**< 8-bit signed integer */
    VAR_UINT8,    /**< 8-bit unsigned integer */
    VAR_INT16,    /**< 16-bit signed integer */
    VAR_UINT16,   /**< 16-bit unsigned integer */
    VAR_INT32,    /**< 32-bit signed integer */
    VAR_UINT32,   /**< 32-bit unsigned integer */
    VAR_FLOAT,    /**< Single precision floating point */
    VAR_DOUBLE   /**< Double precision floating point */
};
#pragma pack(pop)

#pragma pack(push,1)
/** @brief Raw memory structure for Point data
 * 
 * This structure holds a pointer to the raw memory buffer and its size.
 * It is used to manage the data associated with Point objects.
 */
struct RawMemory
{
    public:
    /// @brief Pointer to the raw memory buffer
    uint8_t *value;
    /// @brief Size of the raw memory buffer in bytes
    size_t   size;

    size_t Size()
    {
        return size ; 
    }
    
};
#pragma pack(pop)

#pragma pack(push,1)
/// @brief Memory types for Point data allocation
/// @details This enum defines the memory types used for allocating data buffers in Point objects.
enum isMemory 
{
    RAM,    
    PSRAM,
    SPARE_1,
    SPARE_2
};
#pragma pack(pop)

union isOption
{
    uint8_t data;
    struct 
    {
        isType _type : 4;
        isMemory _memType : 2;
        uint8_t _external_alloc : 1;
        uint8_t _mark : 1;
    };

    size_t Size(){ return data;}
};


static const TypeInfo SchematicPoint[] = 
{
    {"char\0"  , sizeof(char)     },   /**< Character type */
    {"int8_t\0"  , sizeof(int8_t)   },   /**< 8-bit signed integer */
    {"uint8_t\0" , sizeof(uint8_t)  },   /**< 8-bit unsigned integer */
    {"int16_t\0" , sizeof(int16_t)  },   /**< 16-bit signed integer */
    {"uint16_t\0", sizeof(uint16_t) },   /**< 16-bit unsigned integer */
    {"int32_t\0" , sizeof(int32_t)  },   /**< 32-bit signed integer */
    {"uint32_t\0", sizeof(uint32_t) },   /**< 32-bit unsigned integer */
    {"float\0" , sizeof(float)    },   /**< Single precision float */
    {"double\0", sizeof(double)   }    /**< Double precision float */
};



class Tagtool
{
private:
    /* data */
public:
    Tagtool(/* args */);
    ~Tagtool();

    static void * Calloc(isMemory *memType, size_t elements, size_t elementSize)
    {
        switch (*memType)
        {
        case isMemory::RAM:
            return _SECTOR_RAM_(elements, elementSize);
            break;
        case isMemory::PSRAM:
            return _SECTOR_PSRAM_(elements, elementSize);
            break;
        case isMemory::SPARE_1:
            return _SECTOR_SPARE1_(elements, elementSize);
            break;
        case isMemory::SPARE_2:
            return _SECTOR_SPARE2_(elements, elementSize);
            break;
        default:
            return _SECTOR_RAM_(elements, elementSize);
            break;
        }
        return nullptr;
    }

    static void Free(isMemory *memType,void *src)
    {
        free(src);
    }

};

Tagtool::Tagtool(/* args */)
{
}

Tagtool::~Tagtool()
{
}

#endif