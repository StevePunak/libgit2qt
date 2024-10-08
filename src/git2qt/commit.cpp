#include "commit.h"

#include <repository.h>
#include <objectid.h>
#include <tree.h>

using namespace GIT;

Commit::Commit() :
    GitObject(CommitEntity, nullptr, ObjectId())
{
}

Commit::Commit(Repository* repo) :
    GitObject(CommitEntity, repo, ObjectId())
{
}

Commit::Commit(Repository* repo, const ObjectId& objectId) :
    GitObject(CommitEntity, repo, objectId)
{
    resolve();
}

Commit::Commit(GitEntityType entityType, Repository* repo, const ObjectId& objectId) :
    GitObject(entityType, repo, objectId)
{
    resolve();
}

void Commit::resolve()
{
    CommitHandle handle = createHandle();
    if(handle.isNull() == false) {
        _author = Signature(git_commit_author(handle.value()));
        _committer = Signature(git_commit_committer(handle.value()));
        _message = git_commit_message(handle.value());
        _shortMessage = git_commit_summary(handle.value());
        _encoding = git_commit_message_encoding(handle.value());
        _timestamp = QDateTime::fromSecsSinceEpoch((int64_t)git_commit_time(handle.value()), Qt::UTC);
    }
}

Commit::~Commit()
{
}

bool Commit::operator ==(const Commit& other) const
{
    return objectId().isNull() == false &&
           objectId() == other.objectId();
}

Commit Commit::lookup(Repository* repo, const ObjectId& objectId)
{
    Commit result;
    git_commit *commit;
    if(git_commit_lookup(&commit, repo->handle().value(), objectId.toNative()) == 0) {
        result = createFromNative(repo, commit);
        git_commit_free(commit);
    }
    return result;
}

ObjectId Commit::treeId() const
{
    ObjectId result;
    git_commit *thisCommit = nullptr;
    if(git_commit_lookup(&thisCommit, repository()->handle().value(), objectId().toNative()) == 0) {
        const git_oid* oid = git_commit_tree_id(thisCommit);
        result = ObjectId(oid);
    }
    return result;
}

Tree Commit::tree() const
{
    return repository()->lookupTree(treeId());
}

bool Commit::isReachableFrom(const Commit& other) const
{
    Commit::List commits;
    commits.append(other);
    return isReachableFromAny(commits);
}

bool Commit::isReachableFromAny(const List& other) const
{
    git_oid oids[other.count()];
    for(int i = 0;i < other.count();i++) {
        oids[i] = *other.at(i).objectId().toNative();
    }
    int result = git_graph_reachable_from_any(repository()->handle().value(), objectId().toNative(), oids, other.count());
    return result == 1;
}

Commit::List Commit::parents() const
{
    Commit::List result;

    git_commit *thisCommit;
    if(git_commit_lookup(&thisCommit, repository()->handle().value(), objectId().toNative()) == 0) {
        int count = git_commit_parentcount(thisCommit);
        for (int i = 0;i < count;i++) {
            git_commit *parent;
            git_commit_parent(&parent, thisCommit, i);
            Commit commit = createFromNative(repository(), parent);
            result.append(commit);
            git_commit_free(parent);
        }
        git_commit_free(thisCommit);
    }
    return result;
}

CommitHandle Commit::createHandle() const
{
    CommitHandle handle;
    git_commit* commit = nullptr;
    if(git_commit_lookup(&commit, repository()->handle().value(), objectId().toNative()) == 0) {
        handle = CommitHandle(commit);
    }
    return handle;
}

Commit Commit::createFromNative(Repository* repo, git_commit* commit)
{
    ObjectId objectId = git_commit_id(commit);
    Commit result(repo, objectId);
    return result;
}
