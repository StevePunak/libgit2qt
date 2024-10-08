/**
 * Copyright (c) 2024 Stephen Punak
 *
 * This class represents a git submodule
 *
 * Stephen Punak, August 1, 2024
*/
#ifndef SUBMODULE_H
#define SUBMODULE_H
#include <git2qt/gitentity.h>
#include <git2qt/remote.h>
#include <git2qt/objectid.h>
#include <QMap>

namespace GIT {

class ProgressCallback;

class AbstractCredentialResolver;

class Repository;
class GIT2QT_EXPORT Submodule : public GitEntity
{
public:
    Submodule() :
        GitEntity(SubmoduleEntity) {}

    Submodule(Repository* repo, const QString& name, const QString& path, const QString& url);

    enum SubmoduleRecurse
    {
        /// <summary>
        /// Reset to the value in the config.
        /// </summary>
        RecurseReset = -1,
        /// <summary>
        /// Do not recurse into submodules.
        /// </summary>
        RecurseNo = 0,
        /// <summary>
        /// Recurse into submodules.
        /// </summary>
        RecurseYes = 1,
        /// <summary>
        /// Recurse into submodules only when commit not already in local clone.
        /// </summary>
        RecurseOnDemand = 2,
    };

    enum SubmoduleIgnore
    {
        /// <summary>
        /// Reset to the last saved ignore rule.
        /// </summary>
        IgnoreReset = -1,

        /// <summary>
        /// Any change or untracked == dirty
        /// </summary>
        IgnoreNone = 1,

        /// <summary>
        /// Dirty if tracked files change
        /// </summary>
        IgnoreUntracked = 2,

        /// <summary>
        /// Only dirty if HEAD moved
        /// </summary>
        IgnoreDirty = 3,

        /// <summary>
        /// Never dirty
        /// </summary>
        IgnoreAll = 4,
    };

    enum SubmoduleUpdate
    {
        /// <summary>
        /// Reset to the last saved update rule.
        /// </summary>
        UpdateReset = -1,
        /// <summary>
        /// Only used when you don't want to specify any particular update
        /// rule.
        /// </summary>
        UpdateUnspecified = 0,
        /// <summary>
        /// This is the default - checkout the commit recorded in the superproject.
        /// </summary>
        UpdateCheckout = 1,
        /// <summary>
        /// Rebase the current branch of the submodule onto the commit recorded in the superproject.
        /// </summary>
        UpdateRebase = 2,
        /// <summary>
        /// Merge the commit recorded in the superproject into the current branch of the submodule.
        /// </summary>
        UpdateMerge = 3,
        /// <summary>
        /// Do not update the submodule.
        /// </summary>
        UpdateNone = 4,
    };

    enum SubmoduleStatus
    {
        /// <summary>
        /// No submodule changes detected.
        /// </summary>
        Unmodified = 0,

        /// <summary>
        /// Superproject head contains submodule.
        /// </summary>
        /// <remarks>Can be returned even if ignore is set to "ALL".</remarks>
        InHead = (1 << 0),
        /// <summary>
        /// Superproject index contains submodule.
        /// </summary>
        /// <remarks>Can be returned even if ignore is set to "ALL".</remarks>
        InIndex = (1 << 1),
        /// <summary>
        /// Superproject gitmodules has submodule.
        /// </summary>
        /// <remarks>Can be returned even if ignore is set to "ALL".</remarks>
        InConfig = (1 << 2),
        /// <summary>
        /// Superproject working directory has submodule.
        /// </summary>
        /// <remarks>Can be returned even if ignore is set to "ALL".</remarks>
        InWorkDir = (1 << 3),

        /// <summary>
        /// Submodule is in index, but not in head.
        /// </summary>
        /// <remarks>Can be returned unless ignore is set to "ALL".</remarks>
        IndexAdded = (1 << 4),
        /// <summary>
        /// Submodule is in head, but not in index.
        /// </summary>
        /// <remarks>Can be returned unless ignore is set to "ALL".</remarks>
        IndexDeleted = (1 << 5),
        /// <summary>
        /// Submodule in index and head don't match.
        /// </summary>
        /// <remarks>Can be returned unless ignore is set to "ALL".</remarks>
        IndexModified = (1 << 6),
        /// <summary>
        /// Submodule in working directory is not initialized.
        /// </summary>
        /// <remarks>Can be returned unless ignore is set to "ALL".</remarks>
        WorkDirUninitialized = (1 << 7),
        /// <summary>
        /// Submodule is in working directory, but not index.
        /// </summary>
        /// <remarks>Can be returned unless ignore is set to "ALL".</remarks>
        WorkDirAdded = (1 << 8),
        /// <summary>
        /// Submodule is in index, but not working directory.
        /// </summary>
        /// <remarks>Can be returned unless ignore is set to "ALL".</remarks>
        WorkDirDeleted = (1 << 9),
        /// <summary>
        /// Submodule in index and working directory head don't match.
        /// </summary>
        /// <remarks>Can be returned unless ignore is set to "ALL".</remarks>
        WorkDirModified = (1 << 10),

        /// <summary>
        /// Submodule working directory index is dirty.
        /// </summary>
        /// <remarks>Can only be returned if ignore is "NONE" or "UNTRACKED".</remarks>
        WorkDirFilesIndexDirty = (1 << 11),
        /// <summary>
        /// Submodule working directory has modified files.
        /// </summary>
        /// <remarks>Can only be returned if ignore is "NONE" or "UNTRACKED".</remarks>
        WorkDirFilesModified = (1 << 12),

        /// <summary>
        /// Working directory contains untracked files.
        /// </summary>
        /// <remarks>Can only be returned if ignore is "NONE".</remarks>
        WorkDirFilesUntracked = (1 << 13),
    };
    Q_DECLARE_FLAGS(SubmoduleStatuses, SubmoduleStatus)

    QString name() const { return _name; }
    QString path() const { return _path; }
    QString url() const { return _url; }
    ObjectId headCommitId() const { return _headCommitId; }
    ObjectId indexCommitId() const { return _indexCommitId; }
    ObjectId workdirCommitId() const { return _workdirCommitId; }
    SubmoduleRecurse fetchRecurseSumbmodulesRule() const { return _fetchRecurseSumbmodulesRule; }
    SubmoduleIgnore ignoreRule() const { return _ignoreRule; }
    SubmoduleUpdate updateRule() const { return _updateRule; }

    bool isWorkdirInitialized() const;

    static bool isWorkdirInitialized(SubmoduleStatus status);
    static bool isInIndexButNotInHead(SubmoduleStatus status);
    static bool isWorkDirDirty(SubmoduleStatus status);

    bool initialize(bool overwrite = false);
    Repository* open();
    Repository* clone(AbstractCredentialResolver* credentialResolver = nullptr, ProgressCallback* progressCallback = nullptr);
    bool update(bool initialize = false, AbstractCredentialResolver* credentialResolver = nullptr, ProgressCallback* progressCallback = nullptr);

    SubmoduleStatus status(SubmoduleIgnore ignore = IgnoreNone) const;

    QVariant toVariant() const { return QVariant::fromValue<Submodule>(*this); }
    static Submodule fromVariant(const QVariant& value) { return value.value<Submodule>(); }

    virtual bool isNull() const override { return _name.isEmpty(); }

    static QString statusDebugString(SubmoduleStatus status);
    static QString getSubmoduleStatusString(SubmoduleStatus value) { return _SubmoduleStatusToStringMap.getString(value); }
    static SubmoduleStatus getSubmoduleStatus(const QString& value) { return _SubmoduleStatusToStringMap.getType(value); }
    static QList<SubmoduleStatus> getSubmoduleStatusValues() { return _SubmoduleStatusToStringMap.getTypes(); }

    class Map : public QMap<QString, Submodule> {};
    class List : public QList<Submodule>
    {
    public:
        List() {}
        List(const QList<Submodule>& other)
        {
            for(const Submodule& s : other) {
                append(s);
            }
        }

        Submodule findByName(const QString& name) const
        {
            Submodule result;
            auto it = std::find_if(constBegin(), constEnd(), [name](const Submodule& submodule) { return submodule.name() == name; });
            if(it != constEnd()) {
                result = *it;
            }
            return result;
        }

        int countWithStatus(SubmoduleStatuses statuses) const
        {
            int result = std::count_if(constBegin(), constEnd(), [statuses] (const Submodule& submodule) { return (submodule.status() & statuses) != 0; } );
            return result;
        }
    };

    SubmoduleHandle createHandle() const;

private:
    QString _name;
    QString _path;
    QString _url;
    ObjectId _headCommitId;
    ObjectId _indexCommitId;
    ObjectId _workdirCommitId;
    SubmoduleRecurse _fetchRecurseSumbmodulesRule = RecurseReset;
    SubmoduleIgnore _ignoreRule = IgnoreReset;
    SubmoduleUpdate _updateRule = UpdateReset;

    class SubmoduleStatusToStringMap : public EnumToStringMap<SubmoduleStatus>
    {
    public:
        SubmoduleStatusToStringMap()
        {
            insert(Unmodified,                  "Unmodified");
            insert(InHead,                      "InHead");
            insert(InIndex,                     "InIndex");
            insert(InConfig,                    "InConfig");
            insert(InWorkDir,                   "InWorkDir");
            insert(IndexAdded,                  "IndexAdded");
            insert(IndexDeleted,                "IndexDeleted");
            insert(IndexModified,               "IndexModified");
            insert(WorkDirUninitialized,        "WorkDirUninitialized");
            insert(WorkDirAdded,                "WorkDirAdded");
            insert(WorkDirDeleted,              "WorkDirDeleted");
            insert(WorkDirModified,             "WorkDirModified");
            insert(WorkDirFilesIndexDirty,      "WorkDirFilesIndexDirty");
            insert(WorkDirFilesModified,        "WorkDirFilesModified");
            insert(WorkDirFilesUntracked,       "WorkDirFilesUntracked");
        }
    };

    static const SubmoduleStatusToStringMap _SubmoduleStatusToStringMap;
};

GIT2QT_EXPORT QString getSubmoduleStatusString(Submodule::SubmoduleStatus value);
GIT2QT_EXPORT Submodule::SubmoduleStatus getSubmoduleStatus(const QString& value);
GIT2QT_EXPORT QList<Submodule::SubmoduleStatus> getSubmoduleStatusValues();


} // namespace GIT

Q_DECLARE_METATYPE(GIT::Submodule)

#endif // SUBMODULE_H
