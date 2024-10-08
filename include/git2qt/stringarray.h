/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class wraps the git_strarray structure from libgit2
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef STRINGARRAY_H
#define STRINGARRAY_H

#include <QStringList>
#include <git2.h>
#include <git2qt/declspec.h>

namespace GIT {

class GIT2QT_EXPORT StringArray
{
public:
    StringArray();
    StringArray(const QStringList& values);
    StringArray(const QString& value);
    StringArray(const StringArray& other);
    StringArray& operator=(const StringArray& other);
    virtual ~StringArray();

    char* at(int index) const;
    int count() const { return _native.count; }

    const git_strarray* toNative() const { return &_native; }

private:
    void commonInit(const QStringList& values);

    git_strarray _native;
};

} // namespace

#endif // STRINGARRAY_H
