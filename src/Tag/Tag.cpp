#include <Tag.h>

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

PrintPointData static FuncPrintValue[]   = {
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

Tag::Tag(){}

Tag::~Tag(){ Free(); }

size_t Tag::GetArraySize()
{
    return  _data.size / SchematicPoint[_option._type].element_size ;
}

const char * Tag::MonitorInfo(bool include_bytesize)
{
    size_t out = 0;
    out = sprintf(&PrintOut[0],"  %s %s[%d] ",SchematicPoint[_option._type].name,_name.value,GetArraySize());
    if(include_bytesize)
        out += sprintf(&PrintOut[out]," size %d bytes.",GetObjectByteSize());
    out += sprintf(&PrintOut[out],"\n");
    return &PrintOut[0];  
}

const char * Tag::MonitorValue(size_t index)
{
    size_t cur = 0;
    cur = sprintf(PrintOut,"    %s[%d] = ",_name.value,index);
    cur += FuncPrintValue[_option._type](cur,_data.value,index);
    cur += sprintf(&PrintOut[cur],"\n");
    return &PrintOut[0];   
}

bool Tag::SetName(const char *name)
{
    bool monitor;

    monitor = ((_name.size > 0) && (_name.value != nullptr));
    if(monitor)
    {
        free(_name.value);
        _name.value = nullptr;
    }

    _name.size = strlen(name) + 1;
    
    _name.value = (uint8_t*)Allocator(_option._memType,_name.size,sizeof(uint8_t));
    monitor = (_name.value == nullptr);
    if(monitor)
        return !monitor;

    memcpy(_name.value,name,_name.size-1);
    _name.value[_name.size-1] = MARK_CHAR_NULL;

    return true;   
}

const char * Tag::GetName()
{
    return (char*)_name.value;
}

void Tag::Free()
{
    if(_name.value != nullptr)
    {
        //memset(_name.value,MARK_FREE_MEMORY,_name.size);
        //memset(&_name.size,MARK_FREE_MEMORY,sizeof(_name.size));
        free(_name.value);
    }

    if((_data.value != nullptr) && (_option._external_alloc == 0))
    {
        //memset(_data.value,MARK_FREE_MEMORY,_data.size);
        //memset((uint8_t*)&_data.size,MARK_FREE_MEMORY,sizeof(_data.size));
        free(_data.value);
    }
    
    // memset((uint8_t*)_memType,MARK_FREE_MEMORY,sizeof(isMemory));
    // memset((uint8_t*)_external_alloc,MARK_FREE_MEMORY,sizeof(isMemory));
    // memset((uint8_t*)_type,MARK_FREE_MEMORY,sizeof(isMemory));
    
    _first = nullptr;
    _next = nullptr;
}

RawMemory * Tag::GetRawBuffer()
{
    return &_data;
}