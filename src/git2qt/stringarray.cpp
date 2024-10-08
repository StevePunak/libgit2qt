#include "stringarray.h"

#include <log.h>

using namespace GIT;

StringArray::StringArray()
{
    commonInit(QStringList());
}

StringArray::StringArray(const QStringList& values)
{
    commonInit(values);
}

StringArray::StringArray(const QString& value)
{
    commonInit(QStringList() << value);
}

StringArray::StringArray(const StringArray& other)
{
    *this = other;
}

StringArray& StringArray::operator=(const StringArray& other)
{
    if(git_strarray_copy(&_native, &other._native) != 0) {
        Log::logText(LVL_ERROR, "Failed to dup strarray");
    }
    return *this;
}

StringArray::~StringArray()
{
    for(int i = 0;i < (int)_native.count;i++) {
        delete _native.strings[i];
    }
    delete _native.strings;
}

void StringArray::commonInit(const QStringList& values)
{
    _native.count = values.count();
    _native.strings = new char*[_native.count];
    for(int i = 0;i < (int)_native.count;i++) {
        QByteArray utf8 = values.at(i).toUtf8();
        char* ptr = new char[utf8.length() + 1];
        memset(ptr, 0, utf8.length() + 1);
        memcpy(ptr, utf8.constData(), utf8.length());
        _native.strings[i] = ptr;
    }
}

char* StringArray::at(int index) const
{
    char* result = nullptr;
    if(index < (int)_native.count) {
        result = _native.strings[index];
    }
    return result;
}

