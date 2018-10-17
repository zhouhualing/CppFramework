#include "StdAfx.h"
#include "FilePath.h"

#include "MemoryStream.h"
#include "Path.h"
#include "StcFileManager.h"
#include "StcException.h"

#include <sstream>

using namespace stc::framework;



///////////////////////////////////////////////////////////////////////////////
FilePath::FilePath():
	m_isRemote(false)
{

}

FilePath::~FilePath()
{

}

FilePath::FilePath(const std::string& path, bool isRemote):
	m_isRemote(isRemote),
	m_clientPath(path)
{
	// fix forward slash
}

bool FilePath::CopyFromString(const std::string& str, std::string * errorStr)
{
	m_clientPath = str;
	return true;
}

std::string FilePath::ToString() const
{
	return m_clientPath;
}



///////////////////////////////////////////////////////////////////////////////
InputFilePath::InputFilePath()
{
}

InputFilePath::~InputFilePath()
{
}

InputFilePath::InputFilePath(const std::string& path, bool isRemote):FilePath(path, isRemote)
{
}

InputFilePath::InputFilePath(const char* path, bool isRemote):FilePath(path, isRemote)
{
}

InputFilePath::operator std::string () const 
{
	return FileManager::Instance().GetServerSideFilePath(*this);
}


///////////////////////////////////////////////////////////////////////////////
OutputFilePath::OutputFilePath() :
    m_isTemporary(false)
{
}

OutputFilePath::~OutputFilePath()
{
}

OutputFilePath::OutputFilePath(const std::string& path, bool isRemote):
    FilePath(path, isRemote),
    m_isTemporary(false)
{
}

OutputFilePath::OutputFilePath(const char* path, bool isRemote):
    FilePath(path, isRemote),
    m_isTemporary(false)
{
}

// TODO: take this out after content finishes updating
const char* OutputFilePath::c_str() const
{
	if (!m_isRemote)
	{
		return m_clientPath.c_str();
	}
	else
	{
		throw CStcLogicError(
			"This function will be removed before 2.30 is shipped; "
			"You should do this instead if you need to use c_str(); "
			"std::string fileName = someOutputFilePath;"
			"fileName.c_str();");
	}
}

OutputFilePath::operator std::string () const 
{
	return FileManager::Instance().GetServerSideFilePath(*this);
}


///////////////////////////////////////////////////////////////////////////////

CBinaryReader& stc::framework::operator >>( CBinaryReader& ms, FilePath& value )
{
	return (ms >> value.m_clientPath);
}

CBinaryWriter& stc::framework::operator <<( CBinaryWriter& ms, const FilePath& value )
{
	return (ms << value.m_clientPath);
}


STC_PUBLIC CBinaryReader& stc::framework::operator >>( CBinaryReader& ms, InputFilePath& value )
{
	return (ms >> value.m_clientPath);
}

STC_PUBLIC CBinaryWriter& stc::framework::operator <<( CBinaryWriter& ms, const InputFilePath& value )
{
	return (ms << value.m_clientPath);
}


STC_PUBLIC CBinaryReader& stc::framework::operator >>( CBinaryReader& ms, OutputFilePath& value )
{
	return (ms >> value.m_clientPath);
}

STC_PUBLIC CBinaryWriter& stc::framework::operator <<( CBinaryWriter& ms, const OutputFilePath& value )
{
	return (ms << value.m_clientPath);
}

STC_PUBLIC std::ostream& stc::framework::operator <<( std::ostream& os, const OutputFilePath& value )
{
	return (os << value.m_clientPath);
}
