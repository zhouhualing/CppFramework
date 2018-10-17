#ifndef INCLUDED_STC_RESOURCE_PACK_H
#define INCLUDED_STC_RESOURCE_PACK_H

#include "Scriptable.h"

#include <unzip.h>

#define BOOST_FILESYSTEM_NO_LIB
#define BOOST_SYSTEM_NO_LIB
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

class UnitTestResourcePack;

namespace stc {
namespace framework {

class STC_PUBLIC ResourcePack {
private:
    ResourcePack();
    ~ResourcePack() {};

    void Load(const std::string& fname);
    void Unload();

    typedef std::map<const std::string, unz_file_pos> FilePosMap;
    FilePosMap m_filePosMap;
    unzFile m_uzf;
    std::string m_packFileName; 
    bool m_caseSensitive;

    typedef std::map<const std::string, ResourcePack *> PackMap;
    static PackMap s_packMap;

public:
    static ResourcePack& Instance(const std::string& zipFile);

    void SetCaseInsensitive(); // irreversible

    std::auto_ptr<std::istream> GetResourceFile(const std::string& file);

    class Iterator;
    friend class Iterator;
    typedef Iterator iterator;

    iterator begin(const std::string& pattern = "", bool preferZip = false) { return Iterator(this, pattern, preferZip); }
    iterator end() { return Iterator(); }

    class STC_PUBLIC Iterator {
        ResourcePack* m_rp;
        std::string m_extPat;
        bool m_useFileSys;

        FilePosMap::iterator m_upper;
        FilePosMap::iterator m_cur;

        boost::filesystem::directory_iterator m_fcur;
        boost::filesystem::directory_iterator m_fend;

        bool UsingFileSys() const { return m_useFileSys; }
        bool MatchExtPat(const std::string& s) const;

        /// If the directory exists, use that instead of the zip
        void InitUseFileSys(const std::string& dir);

    public:
        Iterator(ResourcePack* rp, const std::string& pattern, bool preferZip);
        Iterator():m_rp(NULL),m_useFileSys(false) {}

        ~Iterator() {}

        bool operator != (const Iterator& o) const
        {
            return !(*this == o);
        }

        std::auto_ptr<std::istream> GetStream()
        {
            return m_rp->GetResourceFile(GetName());
        }

        bool operator == (const Iterator& o) const;

        const std::string GetName() const;

        Iterator& operator++ ();

        friend class ::UnitTestResourcePack;
    };
};

}
}

#endif //INCLUDED_STC_RESOURCE_PACK_H
