#include "branchcollection.h"

#include <gitexception.h>
#include <repository.h>

using namespace GIT;

BranchCollection::BranchCollection(Repository* repo) :
    GitEntity(BranchCollectionEntity, repo)
{
    reloadBranches();
}

Branch::List BranchCollection::reloadBranches()
{
    git_branch_iterator* it = nullptr;
    _cachedBranches.clear();

    try
    {
        throwOnError(git_branch_iterator_new(&it, repository()->handle().value(), GIT_BRANCH_ALL));
        git_reference* reference;
        git_branch_t type;
        while(!git_branch_next(&reference, &type, it)) {
            Branch branch(repository(), Reference::create(repository(), reference));
            _cachedBranches.append(branch);
            git_reference_free(reference);
        }
    }
    catch(const GitException&)
    {
    }

    if(it != nullptr) {
        git_branch_iterator_free(it);
    }
    return _cachedBranches;
}

Branch::Map BranchCollection::localBranches() const
{
    Branch::Map branches;
    for(const Branch& branch : _cachedBranches) {
        if(branch.isRemote() == false) {
            branches.insert(branch.friendlyName(), branch);
        }
    }
    return branches;
}

Branch::Map BranchCollection::remoteBranches() const
{
    Branch::Map branches;
    for(const Branch& branch : _cachedBranches) {
        if(branch.isRemote() == true) {
            branches.insert(branch.friendlyName(), branch);
        }
    }
    return branches;
}

bool BranchCollection::isNull() const
{
    return repository()->isNull();
}
