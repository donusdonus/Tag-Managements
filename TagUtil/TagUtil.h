#ifndef __TAG_TOOL__H__
#define __TAG_TOOL__H__

#include <stdio.h>
#include <stdint.h>

#define MARK_CHAR_NULL 0x00
#define MARK_FREE_MEMORY 0x0FF
#define MAX_BUFFER_PRINT 128
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


namespace TagSys
{

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


    static char PrintOut[MAX_BUFFER_PRINT];
    typedef size_t (*PrintPointData)(size_t ,void *,size_t);
    size_t static DISP_CHAR(size_t offset,void *data,size_t index){ return  snprintf(PrintOut + offset,MAX_BUFFER_PRINT - offset,"%c",((char*)data)[index]);}
    size_t static DISP_INT8(size_t offset,void *data,size_t index){ return  snprintf(PrintOut + offset,MAX_BUFFER_PRINT - offset,"%d ",((int8_t*)data)[index]);}
    size_t static DISP_UINT8(size_t offset,void *data,size_t index){ return  snprintf(PrintOut + offset,MAX_BUFFER_PRINT - offset,"%u ",((uint8_t*)data)[index]);}
    size_t static DISP_INT16(size_t offset,void *data,size_t index){ return  snprintf(PrintOut + offset,MAX_BUFFER_PRINT - offset,"%d ",((int16_t*)data)[index]);}
    size_t static DISP_UINT16(size_t offset,void *data,size_t index){ return  snprintf(PrintOut + offset,MAX_BUFFER_PRINT - offset,"%u ",((uint16_t*)data)[index]);}
    size_t static DISP_INT32(size_t offset,void *data,size_t index){ return  snprintf(PrintOut + offset,MAX_BUFFER_PRINT - offset,"%d ",((int32_t*)data)[index]);}
    size_t static DISP_UINT32(size_t offset,void *data,size_t index){ return  snprintf(PrintOut + offset,MAX_BUFFER_PRINT - offset,"%u ",((uint32_t*)data)[index]);}
    size_t static DISP_FLOAT(size_t offset,void *data,size_t index){ return  snprintf(PrintOut + offset,MAX_BUFFER_PRINT - offset,"%.3f ",((float*)data)[index]);}
    size_t static DISP_DOUBLE(size_t offset,void *data,size_t index){ return  snprintf(PrintOut + offset,MAX_BUFFER_PRINT - offset,"%.3f ",((double*)data)[index]);}


    PrintPointData static const FuncPrintValue[] = {
                                       &DISP_CHAR,
                                       &DISP_INT8,
                                       &DISP_UINT8,
                                       &DISP_INT16,
                                       &DISP_UINT16,
                                       &DISP_INT32,
                                       &DISP_UINT32,
                                       &DISP_FLOAT,
                                       &DISP_DOUBLE
                                   };

     /* Modify Replace std function */
     void *_calloc(isMemory memType, size_t elements, size_t elementSize);
     void _free(void *src);
     uint8_t *_memcpy(uint8_t *des ,uint8_t *src,size_t size);

     /* Common function */
     bool _setName(isMemory memType,RawMemory *src,const char *name);
      
}

#endif
