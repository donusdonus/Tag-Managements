#include <Tag.h>

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
    return _setName(_option._memType,&_name,name);
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
        _free(_name.value);
    
    }

    if((_data.value != nullptr) && (_option._external_alloc == 0))
    {
        //memset(_data.value,MARK_FREE_MEMORY,_data.size);
        //memset((uint8_t*)&_data.size,MARK_FREE_MEMORY,sizeof(_data.size));
        _free(_data.value);
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