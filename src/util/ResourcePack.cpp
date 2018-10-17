#include "StdAfx.h"

#include "ResourcePack.h"
#include "StcSystem.h"
#include "Path.h"
#include "StringUtil.h"

#include <boost/algorithm/string.hpp>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/categories.hpp>

using namespace stc::framework;

struct ZipSource {
    typedef char char_type;
    typedef boost::iostreams::source_tag  category;
    unzFile m_uzf;

    ZipSource(unzFile uzf):m_uzf(uzf) {}
    ~ZipSource() {}

    std::streamsize read(char_type* s, std::streamsize n)
    {
        const std::streamsize ret =
            unzReadCurrentFile(m_uzf, (voidp)s, (unsigned int)n);
        if (ret > 0)  return ret;
        if (ret == 0) return -1;
        throw CStcRunTimeError("zip read error");
    }
};


struct ZipSourceStream : public boost::iostreams::stream<ZipSource>
{
    unzFile m_uzf;

    ZipSourceStream(unzFile uzf):boost::iostreams::stream<ZipSource>(uzf),m_uzf(uzf) {}

    virtual ~ZipSourceStream()
    {
        if (m_uzf)
        {
            unzClose(m_uzf);
        }
    }
};

//---------------------------------------------------------------------------

ResourcePack::PackMap ResourcePack::s_packMap;

ResourcePack::ResourcePack() :
    m_uzf(0),
    m_caseSensitive(true)
{
}

ResourcePack& ResourcePack::Instance(const std::string& zipFile)
{
    PackMap::iterator iter = s_packMap.find(zipFile);
    if (iter != s_packMap.end())
        return *(iter->second); 

    ResourcePack * pack = new ResourcePack();
    s_packMap[zipFile] = pack;

    pack->Load(Path::JoinPath(CStcSystem::GetApplicationInstallPath(), zipFile));
    return *pack;
}

void ResourcePack::Load(const std::string& fname)
{
    Unload();

    m_packFileName = fname;
    m_uzf = unzOpen(m_packFileName.c_str());


    if (m_uzf == NULL)
    {
        // TODO FIXME Linux install misses the zip files
        // throw CStcRunTimeError(std::string("Failed to open ") + fname);
        return;
    }

    if (unzGoToFirstFile(m_uzf) != UNZ_OK)
        throw CStcRunTimeError(std::string("Failed to find first file in ") + fname);

    do
    {
        char nameBuf[2048];
        unzGetCurrentFileInfo(m_uzf, NULL, nameBuf, sizeof(nameBuf), NULL, 0, NULL, 0);

        unz_file_pos pos;
        unzGetFilePos(m_uzf, &pos);

        m_filePosMap[std::string(nameBuf)] = pos;

    } while (unzGoToNextFile(m_uzf) == UNZ_OK);
}

void ResourcePack::Unload()
{
    m_packFileName = "";
    m_filePosMap.clear();
    if (m_uzf)
    {
        unzClose(m_uzf);
        m_uzf = NULL;
        m_caseSensitive = true;
    }
}

void ResourcePack::SetCaseInsensitive()
{
    if (!m_caseSensitive)
        return;

    m_caseSensitive = false;

    // allow future case-insensitive searching by users by tolowering all names
    FilePosMap filePosOrig = m_filePosMap;
    m_filePosMap.clear();
    
    for (FilePosMap::iterator iter = filePosOrig.begin();
         iter != filePosOrig.end(); ++iter)
    {
        std::string name = iter->first;
        boost::algorithm::to_lower(name);
        m_filePosMap[name] = iter->second;
    }
}

std::auto_ptr<std::istream> ResourcePack::GetResourceFile(const std::string& res_in)
{
    std::string res = res_in;
    if (!m_caseSensitive)
    {
        boost::algorithm::to_lower(res);
    }

    FilePosMap::iterator it = m_filePosMap.find(res);
    if (it == m_filePosMap.end())
    {
        std::string r = res;
        if (Path::IsRelativePath(r))
        {
            r = Path::JoinPath(CStcSystem::GetApplicationInstallPath(), res);
        }
        return std::auto_ptr<std::istream>(new std::ifstream(r.c_str()));
    }

    unzFile uzf = unzOpen(m_packFileName.c_str());
    if (uzf == NULL)
        throw CStcRunTimeError(std::string("Failed to open ") + m_packFileName);

    if (unzGoToFilePos(uzf, &(it->second)) != UNZ_OK)
        throw CStcRunTimeError(std::string("Failed to seek within ") + m_packFileName);

    if (unzOpenCurrentFile(uzf) != UNZ_OK)
        throw CStcRunTimeError(std::string("Failed to open ") + res + std::string(" in ") + m_packFileName);

    return std::auto_ptr<std::istream>(
        new ZipSourceStream(uzf));
        //new boost::iostreams::stream<ZipSource>(m_uzf));
}


//---------------------------------------------------------------------------

bool ResourcePack::Iterator::MatchExtPat(const std::string& s) const
{
    return m_extPat.empty() || StringUtil::EndsWith(s, m_extPat);
}

void ResourcePack::Iterator::InitUseFileSys(const std::string& dir)
{
    std::string fullPath = dir;
    if (Path::IsRelativePath(fullPath))
    {
        fullPath = Path::JoinPath(CStcSystem::GetApplicationInstallPath(), dir);
    }
    m_useFileSys = Path::DirectoryExists(fullPath);
}

ResourcePack::Iterator::Iterator(ResourcePack* rp, const std::string& pattern, bool preferZip):
    m_rp(rp),
    m_useFileSys(false)
{
    assert(m_rp);
    const size_t pos = pattern.find_first_of('*');
    std::string dir = pattern.substr(0, pos);

    if (pos != std::string::npos && pos != (pattern.length() - 1))
    {
        m_extPat = pattern.substr(pos+1);
        if (!m_rp->m_caseSensitive)
        {
            boost::algorithm::to_lower(m_extPat);
        }
    }

    if (!preferZip)
    {
        InitUseFileSys(dir);
    }

    if (!UsingFileSys())
    {
        if (!m_rp->m_caseSensitive)
        {
            boost::algorithm::to_lower(dir);
        }

        m_cur = m_rp->m_filePosMap.lower_bound(dir);
        m_upper = m_rp->m_filePosMap.lower_bound(dir + char(255));

        if (m_cur == m_upper)
        {
            m_rp = NULL;
        }
        else if (!MatchExtPat(GetName()))
        {
            // don't return a begin() that doesn't match
            operator++();
        }
    }
    else
    {
        if (Path::IsRelativePath(dir))
        {
            dir = Path::JoinPath(CStcSystem::GetApplicationInstallPath(), dir);
        }

        boost::filesystem::path path(dir);
        m_fcur = boost::filesystem::directory_iterator(path);

        if (m_fcur == m_fend)
        {
            m_rp = NULL;
        }
        else if (boost::filesystem::is_directory(m_fcur->status()) ||
                 !MatchExtPat(GetName()))
        {
            // don't return a begin() that doesn't match
            operator++();
        }
    }
}

bool ResourcePack::Iterator::operator== (const Iterator& o) const
{
    if (m_rp == o.m_rp)
    {
        return (m_rp == NULL ||
            (!UsingFileSys()? m_cur == o.m_cur : m_fcur == o.m_fcur));
    }
    return false;
}

const std::string ResourcePack::Iterator::GetName() const
{
    if (!UsingFileSys())
        return m_cur->first;
    else
        return m_fcur->path().string();
}

ResourcePack::Iterator& ResourcePack::Iterator::operator++ ()
{
    assert(m_rp);

    if (!UsingFileSys())
    {
        do
        {
            ++m_cur;

        } while (m_cur != m_upper && !MatchExtPat(GetName()));

        if (m_cur == m_upper)
        {
            m_rp = NULL;
        }
    }
    else
    {
        do
        {
            ++m_fcur;
        } while (
            m_fcur != m_fend &&
            (boost::filesystem::is_directory(m_fcur->status()) ||
             !MatchExtPat(GetName())));

        if (m_fcur == m_fend)
        {
            m_rp = NULL;
        }
    }

    return *this;
}
