/*  $Id: Trie.cpp,v 1.11 2011/01/08 19:07:36 sarrazip Exp $
    Trie.cpp - Tree structure for string storage

    verbiste - French conjugation system
    Copyright (C) 2003-2005 Pierre Sarrazin <http://sarrazip.com/>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
    02111-1307, USA.
*/

#include "Trie.h"

#include <assert.h>
#include <stdlib.h>
#include <list>
#include <iostream>


namespace verbiste {


template <class T>
Trie<T>::Trie(bool _userDataFromNew)
  : lambda(),
    firstRow(new Row()),
    userDataFromNew(_userDataFromNew)
{
}


template <class T>
Trie<T>::~Trie()
{
    firstRow->recursiveDelete(userDataFromNew);
    delete firstRow;
}


template <class T>
Trie<T>::Descriptor::Descriptor(Row *inferior /*= NULL*/)
  : inferiorRow(inferior),
    userData(NULL)
{
}


template <class T>
Trie<T>::Descriptor::~Descriptor()
{
}


template <class T>
void
Trie<T>::Descriptor::recursiveDelete(bool deleteUserData)
{
    if (deleteUserData)
    {
        delete userData;
        userData = NULL;
    }
    if (inferiorRow != NULL)
    {
        inferiorRow->recursiveDelete(deleteUserData);
        delete inferiorRow;
        inferiorRow = NULL;
    }
}


template <class T>
size_t
Trie<T>::Descriptor::computeMemoryConsumption() const
{
    return sizeof(*this) + (inferiorRow != NULL ? inferiorRow->computeMemoryConsumption() : 0);
}


template <class T>
size_t
Trie<T>::CharDesc::computeMemoryConsumption() const
{
    return (sizeof(*this) - sizeof(desc)) + desc.computeMemoryConsumption();
}


template <class T>
size_t
Trie<T>::Row::computeMemoryConsumption() const
{
    size_t sum = 0;
    for (typename std::vector<CharDesc>::const_iterator it = elements.begin(); it != elements.end(); ++it)
        sum += it->computeMemoryConsumption();
    return sizeof(*this) + sum;
}


template <class T>
void
Trie<T>::Row::recursiveDelete(bool deleteUserData)
{
    for (typename std::vector<CharDesc>::iterator it = elements.begin();
                                        it != elements.end(); it++)
        it->desc.recursiveDelete(deleteUserData);
    elements.clear();
}


template <class T>
typename Trie<T>::Descriptor *
Trie<T>::Row::find(wchar_t unichar)
{
    for (typename std::vector<CharDesc>::iterator it = elements.begin();
                                        it != elements.end(); it++)
        if (it->unichar == unichar)
            return &it->desc;

    return NULL;
}


template <class T>
typename Trie<T>::Descriptor &
Trie<T>::Row::operator [] (wchar_t unichar)
{
    Descriptor *pd = find(unichar);
    if (pd != NULL)
        return *pd;

    elements.push_back(CharDesc(unichar));
    assert(elements.back().unichar == unichar);
    return elements.back().desc;
}


///////////////////////////////////////////////////////////////////////////////


template <class T>
T *
Trie<T>::add(const std::wstring &key, T *userData)
{
    if (key.empty())
    {
        T *old = lambda;
        lambda = userData;
        return old;
    }

    Descriptor *d = getDesc(firstRow, key, 0, true, false);
    assert(d != NULL);
    T *old = d->userData;
    d->userData = userData;
    return old;
}


template <class T>
T *
Trie<T>::get(const std::wstring &key) const
{
    if (lambda != NULL)
        onFoundPrefixWithUserData(key, 0, lambda);

    if (key.empty())
        return lambda;

    Descriptor *d = const_cast<Trie<T> *>(this)->getDesc(firstRow, key, 0, false, true);
    return (d != NULL ? d->userData : NULL);
}


template <class T>
T *
Trie<T>::getWithDefault(const std::wstring &key, T *deFault)
{
    if (key.empty())
    {
        if (lambda == NULL)
            lambda = deFault;
        return lambda;
    }

    Descriptor *d = getDesc(firstRow, key, 0, true, false);
    assert(d != NULL);
    if (d->userData == NULL)
        d->userData = deFault;
    return d->userData;
}


template <class T>
T **
Trie<T>::getUserDataPointer(const std::wstring &key)
{
    if (key.empty())
        return &lambda;

    // Get descriptor associated with 'key' (and create a new entry
    // if the key is not known).
    //
    Descriptor *d = getDesc(firstRow, key, 0, true, false);
    assert(d != NULL);
    return &d->userData;
}


template <class T>
typename Trie<T>::Descriptor *
Trie<T>::getDesc(Row *row,
                const std::wstring &key,
                std::wstring::size_type index,
                bool create,
                bool callFoundPrefixCallback)
{
    assert(row != NULL);
    assert(index < key.length());

    wchar_t unichar = key[index];  // the "expected" character
    assert(unichar != '\0');

    Descriptor *pd = row->find(unichar);

    static bool trieTrace = getenv("TRACE") != NULL;
    if (trieTrace)
        std::wcout << "getDesc(row=" << row
                   << ", key='" << key << "' (len=" << key.length()
                   << "), index=" << index
                   << ", create=" << create
                   << ", call=" << callFoundPrefixCallback
                   << "): unichar=" << unichar << ", pd=" << pd << "\n";

    if (pd == NULL)  // if expected character not found
    {
        if (!create)
            return NULL;

        Descriptor &newDesc = (*row)[unichar];
        assert(row->find(unichar) != NULL);
        assert(row->find(unichar) == &newDesc);

        if (index + 1 == key.length())  // if last char of string
            return &newDesc;

        // Create new descriptor that points to a new inferior row:
        newDesc.inferiorRow = new Row();
        assert(row->find(unichar)->inferiorRow == newDesc.inferiorRow);

        return getDesc(newDesc.inferiorRow,
                        key, index + 1, create, callFoundPrefixCallback);
    }

    if (trieTrace)
        std::wcout << "getDesc: userData=" << pd->userData
                   << ", inferiorRow=" << pd->inferiorRow
                   << "\n";

    if (callFoundPrefixCallback && pd->userData != NULL)
        onFoundPrefixWithUserData(key, index + 1, pd->userData);  // virtual call

    if (index + 1 == key.length())  // if reached end of key
    {
        if (trieTrace)
            std::wcout << "getDesc: reached end of key\n";
        return pd;
    }

    if (pd->inferiorRow == NULL)  // if pd is a leaf:
    {
        if (!create)
            return NULL;  // not found

        pd->inferiorRow = new Row();
    }

    return getDesc(pd->inferiorRow,
                        key, index + 1, create, callFoundPrefixCallback);
}


template <class T>
size_t
Trie<T>::computeMemoryConsumption() const
{
    return sizeof(*this) + (firstRow != NULL ? firstRow->computeMemoryConsumption() : 0);
}


}  // namespace verbiste
