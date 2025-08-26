#ifndef  ___TAG_H___
#define  ___TAG_H___

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


/** Base point configuration macro */
/** Maximum buffer size for printing operations */
#define MARK_CHAR_NULL 0x00
#define MARK_FREE_MEMORY 0x0FF
#define MAX_BUFFER_PRINT 128
#define _SECTOR_RAM_(x,y) calloc(x,y)
#define _SECTOR_PSRAM_(x,y) calloc(x,y)
#define _SECTOR_SPARE1_(x,y) calloc(x,y)
#define _SECTOR_SPARE2_(x,y) calloc(x,y)
#define RAM_FREE(x) free(x)
/** Maximum buffer size for printing operations */
static void * Allocator(isMemory memType, size_t elements, size_t elementSize)
{
    switch (memType)
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
enum isMemory : uint8_t
{
    RAM,    
    PSRAM,
    SPARE_1,
    SPARE_2
};
#pragma pack(pop)


class Tag
{
private:
    /* data */
    isOption _option;
    RawMemory _name;       /**< Name of the component */
    RawMemory _data;       /**< Data buffer for the component */
    Tag *_next = nullptr;
    Tag *_first = nullptr;

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