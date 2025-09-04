#include "Tag.h"

Tag::Tag(){}

Tag::~Tag(){ Free(); }


bool Tag::init(isType type,const char * name,size_t array_size=1,isMemory memtype = isMemory::RAM)
{
            /* Check array value size */
            if(array_size == 0) 
                return false;

            bool monitor;

            /* clear value buffer size  */
            _name.size = 0;
            _data.size = 0;

            /* assign size of elements = typeofsize * array_size */
            _data.size = SchematicPoint[type].element_size * array_size;
            
            /* assign source memory  */
            _option._memType = memtype;

            /* assign not reference external address */
            _option._external_alloc = 0;

            /* assign type of data  */
            _option._type = type;

            /* Set tag name */
            monitor = SetName(name);
            if(!monitor)
                return false ;

            _data.value = (uint8_t*)_calloc(memtype,array_size,SchematicPoint[type].element_size);

            monitor = (_data.value != nullptr);
            if(!monitor)
                return false ;

    return true;
}

bool Tag::init(isType type,const char * name,void *addr,size_t array_size=1,isMemory memtype = isMemory::RAM)
{
            bool monitor;

            /* Check array value size */
            if(array_size == 0) 
                return false;

            /* addr is nullptr */
            if(addr == nullptr)
                return false;

            /* clear value buffer size  */
            _name.size = 0;
            _data.size = 0;

            /* assign size of elements = typeofsize * array_size */
            _data.size = SchematicPoint[type].element_size * array_size;
            
            /* assign source memory  */
            _option._memType = memtype;

            /* assign reference external address */
            _option._external_alloc = 1;

            /* assign type of data  */
            _option._type = type;

            /* Set tag name */
            monitor = SetName(name);
            if(!monitor)
                return false ;

            _data.value = (uint8_t*)addr;
        
            monitor = (_data.value != nullptr);
            if(!monitor)
                return false ;

    return true;
}

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
        _name.value = nullptr;
    }

    if((_data.value != nullptr) && (_option._external_alloc == 0))
    {
        //memset(_data.value,MARK_FREE_MEMORY,_data.size);
        //memset((uint8_t*)&_data.size,MARK_FREE_MEMORY,sizeof(_data.size));
        _free(_data.value);
        _data.value = nullptr;
    }
    
    // memset((uint8_t*)_memType,MARK_FREE_MEMORY,sizeof(isMemory));
    // memset((uint8_t*)_external_alloc,MARK_FREE_MEMORY,sizeof(isMemory));
    // memset((uint8_t*)_type,MARK_FREE_MEMORY,sizeof(isMemory));
    
}

RawMemory * Tag::GetRawBuffer()
{
    return &_data;
}