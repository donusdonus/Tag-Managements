#ifndef __TAGSTATIC_H
#define __TAGSTATIC_H

#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "string.h"


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

#pragma pack(push,1)
struct TypeInfo {
    const char* name;       
    uint8_t element_size;        
};
#pragma pack(pop)

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
};


#pragma pack(push,1)
struct Tag
{
    public:
        
        isOption _option;
        RawMemory _name;       /**< Name of the component */
        RawMemory _data;       /**< Data buffer for the component */

        Tag *_next = nullptr;
        Tag *_first = nullptr;

        size_t GetObjectByteSize()
        {
            return  1 +  // _option
                    1 +  // _len_of_name
                    _name.size + // byte name data
                    4 ; // byte size of data
                    _data.size ; // byte data
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

        size_t GetArraySize()
        {
            return  _data.size / SchematicPoint[_option._type].element_size ;
        }

        const char * MonitorInfo(bool include_bytesize = false)
        {
            size_t out = 0;
            out = sprintf(&PrintOut[0],"  %s %s[%d] ",SchematicPoint[_option._type].name,_name.value,GetArraySize());
            if(include_bytesize)
                out += sprintf(&PrintOut[out]," size %d bytes.",GetObjectByteSize());
            out += sprintf(&PrintOut[out],"\n");
            return &PrintOut[0];
        }

        const char * MonitorValue(size_t index)
        {
            size_t cur = 0;
            cur = sprintf(PrintOut,"    %s[%d] = ",_name.value,index);
            cur += FuncPrintValue[_option._type](cur,_data.value,index);
            cur += sprintf(&PrintOut[cur],"\n");
            return &PrintOut[0];
        }

        bool SetName(const char *name)
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
      
        const char * GetName()
        {
            return (char*)_name.value;
        }

        void Free()
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

        RawMemory * GetRawBuffer()
        {
            return &_data;
        }
        
    };
#pragma pack(pop)

struct TagGroup
{

        Tag *_Item = nullptr ; 

        Tag * Add(isType type,const char * name,size_t array_size=1,isMemory memtype = isMemory::RAM)
        {
            if(array_size == 0) 
                return nullptr;

            bool monitor;
            Tag **cur = &this->_Item;
            Tag **cur_first = &this->_Item;

            /* 1. Find last elements for connect */
            while ((cur != nullptr) && (*cur != nullptr))
            {
                cur = &(*cur)->_next;
            }

            Tag *newItem = (Tag*)Allocator(memtype,array_size,sizeof(Tag));

            monitor = (newItem != nullptr);
            if(!monitor)
            return nullptr;

            newItem->_name.size = 0;
            newItem->_data.size = 0;

            newItem->_data.size = SchematicPoint[type].element_size * array_size;
            newItem->_option._memType = memtype;
            newItem->_option._type = type;

            monitor = newItem->SetName(name);
            if(!monitor)
            return nullptr;

            newItem->_data.value = (uint8_t*)Allocator(memtype,array_size,SchematicPoint[type].element_size);

            monitor = (newItem->_data.value != nullptr);
            if(!monitor)
            return nullptr;

            *cur = newItem;
            if(cur_first != nullptr)
                (*cur)->_first = (*cur_first);

            return *cur;
        }

        Tag * Add(isType type,const char * name,void *addr,size_t array_size=1,isMemory memtype = isMemory::RAM)
        {
            if(array_size == 0)
                return nullptr;

            bool monitor;
            Tag **cur = &_Item;
            Tag **cur_first = &_Item;

            /* 1. Find last elements for connect */
            while ((cur != nullptr) && (*cur != nullptr))
            {
                cur = &(*cur)->_next;
            }

            Tag *newItem = (Tag*)Allocator(memtype,array_size,sizeof(Tag));

            monitor = (newItem != nullptr);
            if(!monitor)
            return nullptr;

            newItem->_name.size = 0;
            newItem->_data.size = 0;

            newItem->_data.size = SchematicPoint[type].element_size * array_size;
            newItem->_option._memType = memtype;
            newItem->_option._type = type;

            monitor = newItem->SetName(name);
            if(!monitor)
            return nullptr;

            newItem->_data.value = (uint8_t*)addr;

            monitor = (newItem->_data.value != nullptr);
            if(!monitor)
            return nullptr;

            *cur = newItem;
            (*cur)->_first = (*cur_first);

            return *cur;
        }

        Tag * Find(const char *name)
        {
            bool monitor;
            Tag *cur = _Item;

            while(cur != nullptr)
            {
                monitor = (strcmp(name,(char*)cur->_name.value) == 0);
                if(monitor)
                    return cur;

                cur = cur->_next;
            }

            return nullptr;
        }

        Tag * FindByIndex(size_t index)
        {
            bool monitor;
            size_t count = 0;
            Tag **cur = &_Item;

            while((cur != nullptr) && ((*cur) != nullptr))
            {
                monitor = (index == count);
                if(monitor)
                    return *cur;
                    
                cur = &(*cur)->_next;
                count+=1;
            }

            return nullptr;
        }

        int FindIndex(const char *name)
        {
            int count = 0;
            bool monitor;
            Tag **cur = &_Item;

            while((cur != nullptr) && ((*cur) != nullptr))
            {
                monitor = (strcmp(name,(char*)(*cur)->_name.value) == 0);
                if(monitor)
                    return count;

                cur = &(*cur)->_next;
                count+=1;
            }

            return -1;
        }

        void Remove(size_t index)
        {
            bool monitor;
            int count = 0;
            Tag **cur_prev = nullptr;
            Tag **cur = &_Item;


            while((cur != nullptr)&&((*cur) != nullptr))
            {
                Tag *tmp = nullptr;

                monitor = (count == index);
                if(monitor)
                {
                    tmp = (*cur)->_next;
                    (*cur)->Free();
                    free(*cur);
                    *cur = tmp;

                    if((cur_prev != nullptr) && ((*cur_prev) != nullptr))
                        (*cur_prev)->_next = *cur;

                    return ;
                }

                cur_prev = cur;
                cur = &(*cur)->_next; 
                count++;
            }
        }

        void Remove(const char * name)
        {
           int index = FindIndex(name);

           if(index == -1) return;

           Remove(index);
        }

        void RemoveAll()
        {
            Tag **cur = &_Item;

            
            while((cur != nullptr) && ((*cur) != nullptr ))
            {
                Tag **tmp = nullptr;
                tmp = cur;
                *cur = (*cur)->_next;
                (*tmp)->Free();
                free(*tmp);
                *tmp = nullptr;
            }
        }

        size_t Count()
        {
            Tag **cur = &_Item;
            size_t count = 0;
            
            while((cur != nullptr) && ((*cur) != nullptr))
            {   
                cur = &(*cur)->_next;
                count+=1;
            }

            return count;
        }

    };

#pragma pack(push,1)
struct Topic
{
    RawMemory _name;       /**< Name of the group */
    isOption _option;
    TagGroup * _group = nullptr;
    Topic *_first = nullptr;
    Topic *_next = nullptr;


    size_t GetObjectByteSize()
    {
            return  1 +  // IsOption 
                    1 +  // _len_of_name
                    _name.size; // byte name data
    }

    const char * MonitorInfo(bool include_bytesize = false)
    {
        size_t out = 0;
        out = sprintf(&PrintOut[0],"TOPIC %s",_name.value);
        if(include_bytesize)
            out += sprintf(&PrintOut[out]," size %d bytes.",GetObjectByteSize());
        out += sprintf(&PrintOut[out],"\n");
        
        return &PrintOut[0];
    }
  
    const char * Getname()
    {
        return (char*)_name.value;
    }
    
    bool SetName(const char *name)
    {
        bool monitor;

        monitor = ((_name.value != nullptr) && (_name.size > 0));
        if(monitor)
        {
            free(_name.value);
            _name.value = nullptr;
        }

        _name.size = strlen(name) + 1;
        
        _name.value = (uint8_t*)Allocator(_option._memType,_name.size,sizeof(uint8_t));
        monitor = (_name.value != nullptr);
        if(!monitor)
            return false;



        memcpy(_name.value,name,_name.size-1);
        _name.value[_name.size-1] = MARK_CHAR_NULL;

        return true;
    }

    void Free()
    {        
        if((_name.value != nullptr) && (_name.size > 0))
        {
            //memset(_name.value,MARK_FREE_MEMORY,_name.size);
            //memset(&_name.size,MARK_FREE_MEMORY,sizeof(_name.size));
            free(_name.value);
            _name.value = nullptr;
        }

        if(_group != nullptr )
        {
            _group->RemoveAll();
            free(_group);
            _group = nullptr;
        }

        _first = nullptr;
        _next = nullptr;
    }
    
    TagGroup * Items()
    {
        return _group;
    }


};
#pragma pack(pop)

#pragma pack(push,1)
struct TopicGroup
{
    Topic * _Topics = nullptr ;
    
        Topic * Add(const char * name,isMemory memtype = isMemory::RAM)
        {
            bool monitor;
            Topic **cur = &_Topics;
            Topic **cur_first = &_Topics;

            /* 1. Find last elements for connect */
            while ((cur != nullptr) && (*cur != nullptr))
            {
                cur = &(*cur)->_next;
            }

            Topic *newTopic = (Topic*)Allocator(memtype,1,sizeof(Topic));

            monitor = (newTopic != nullptr);
            if(!monitor)
            return nullptr;

            newTopic->_option._memType = memtype;
            newTopic->_group = (TagGroup*)calloc(1,sizeof(TagGroup));
         
            monitor = newTopic->SetName(name);
            if(!monitor)
            return nullptr;

            *cur = newTopic;
            (*cur)->_first = (*cur_first);

            return *cur;
        }

        Topic * Find(const char *name)
        {
            bool monitor;
            Topic **cur = &_Topics;

            while((cur != nullptr) && ((*cur) != nullptr))
            {
                monitor = (strcmp(name,(char*)(*cur)->_name.value) == 0);
                if(monitor)
                    return *cur;

                cur = &(*cur)->_next;
            }

            return nullptr;
        }

        Topic * Find(size_t index)
        {
            bool monitor;
            size_t count = 0;
            Topic **cur = &_Topics;

            while((cur != nullptr) && ((*cur) != nullptr))
            {
                monitor = (index == count);
                if(monitor)
                    return *cur;
                    
                cur = &(*cur)->_next;
                count+=1;
            }

            return nullptr;
        }

        int FindIndex(const char *name)
        {
            int count = 0;
            bool monitor;
            Topic **cur = &_Topics;

            while((cur != nullptr) && ((*cur) != nullptr))
            {
                monitor = (strcmp(name,(char*)(*cur)->_name.value) == 0);
                if(monitor)
                    return count;

                cur = &(*cur)->_next;
                count+=1;
            }

            return -1;
        }

        void Remove(size_t index)
        {
            bool monitor;
            int count = 0;
            Topic **cur_prev = nullptr;
            Topic **cur = &_Topics;


            while((cur != nullptr) && ((*cur) != nullptr))
            {
                Topic *tmp = nullptr;
                monitor = (count == index);
                if(monitor)
                {
                     tmp = (*cur)->_next;
                    (*cur)->Free();
                    free(*cur);
                    *cur = tmp;

                    if((cur_prev != nullptr) && ((*cur_prev) != nullptr))
                        (*cur_prev)->_next = *cur;

                    return ;
                }

                cur_prev = cur;
                cur = &(*cur)->_next; 
                count++;
            }
        }

        void Remove(const char * name)
        {
           int index = FindIndex(name);

           if(index == -1) return;

           Remove(index);
        }

        void RemoveAll()
        {
            Topic **cur = &_Topics;
            
            while((*cur != nullptr) && ((*cur) != nullptr))
            {
                Topic **tmp = nullptr;
                tmp = cur;
                *cur = (*cur)->_next;
                if( (*tmp) != nullptr)
                {
                    (*tmp)->Free();
                    free(*tmp);
                    *tmp = nullptr; 
                }
            }
        }

        size_t Count()
        {
            Topic **cur = &_Topics;
            size_t count = 0;
            
            while((cur != nullptr) && ((*cur) != nullptr))
            {   
                cur = &(*cur)->_next;
                count+=1;
            }

            return count;
        }
    
};
#pragma pack(pop)




#endif // TAGSTATIC_H