#ifndef INCLUDED_FILE_PATH_H
#define INCLUDED_FILE_PATH_H

#pragma once

#include "StcExport.h"

#include <string>
#include <fstream>


namespace stc {
namespace framework {

class CBinaryReader;
class CBinaryWriter;

/*
	Represents a file path property
 */
class STC_PUBLIC FilePath 
{
public:

	FilePath();
	virtual ~FilePath();

	FilePath(const std::string&, bool isRemote);

	
	bool operator != (const FilePath& fp) const 
		{ return !(*this == fp); }

	bool operator == (const FilePath& fp) const 
		{ return (m_isRemote == fp.m_isRemote && m_clientPath == fp.m_clientPath); }

    bool operator < (const FilePath & other) const
        { return m_isRemote == other.m_isRemote ? m_clientPath < other.m_clientPath : m_isRemote; }
	

	/// \brief Returns if the file path is a remote path.
	bool IsRemote() const { return m_isRemote; }
           
	/// \brief Returns the client side file path.
	const std::string& GetClientSideFilePath() const { return m_clientPath; }


	/// \brief Copy data value from the input string.
	bool CopyFromString(const std::string& str, std::string * errorStr = 0);
	/// \brief Convert data value to a string.
	std::string ToString() const;




protected:

	std::string m_clientPath;
	bool m_isRemote;

	friend STC_PUBLIC CBinaryReader& operator >>( CBinaryReader& ms, FilePath& value );
	friend STC_PUBLIC CBinaryWriter& operator <<( CBinaryWriter& ms, const FilePath& value );

};




/*
	Represents an input file path for the BLL (to open).
 */
class STC_PUBLIC InputFilePath : public FilePath
{
public:

	InputFilePath();
	virtual ~InputFilePath();

	// TODO: make these explicit after content finishes updating
	InputFilePath(const std::string& path, bool isRemote = false);
	InputFilePath(const char* path, bool isRemote = false);

	operator std::string () const;

private:
	
	friend STC_PUBLIC CBinaryReader& operator >>( CBinaryReader& ms, InputFilePath& value );
	friend STC_PUBLIC CBinaryWriter& operator <<( CBinaryWriter& ms, const InputFilePath& value );
};


/*
	Represents an output filep ath for the BLL (to save).
 */
class STC_PUBLIC OutputFilePath : public FilePath
{
public:

	OutputFilePath();
	virtual ~OutputFilePath();

	// TODO: make these explicit after content finishes updating
	OutputFilePath(const std::string& path, bool isRemote = false);
	OutputFilePath(const char* path, bool isRemote = false);
	// TODO: take this out after content finishes updating
	const char* c_str() const;

	operator std::string () const;

    /// \brief Returns if the file path is a temporary file.   
    bool IsTemporary() const { return m_isTemporary; }

    /// \brief Sets the temporary flag. 
    ///        Setting to true means the caller's intention is to delete the file when finished.
    ///        The caller is responsible for the deletion.
    void SetTemporary(bool temporary) { m_isTemporary = temporary; }

private:

	friend STC_PUBLIC CBinaryReader& operator >>( CBinaryReader& ms, OutputFilePath& value );
	friend STC_PUBLIC CBinaryWriter& operator <<( CBinaryWriter& ms, const OutputFilePath& value );

	friend STC_PUBLIC std::ostream& operator <<( std::ostream& os, const OutputFilePath& value );
    
    bool m_isTemporary;
};


STC_PUBLIC CBinaryReader& operator >>( CBinaryReader& ms, FilePath& value );
STC_PUBLIC CBinaryWriter& operator <<( CBinaryWriter& ms, const FilePath& value );

STC_PUBLIC CBinaryReader& operator >>( CBinaryReader& ms, InputFilePath& value );
STC_PUBLIC CBinaryWriter& operator <<( CBinaryWriter& ms, const InputFilePath& value );

STC_PUBLIC CBinaryReader& operator >>( CBinaryReader& ms, OutputFilePath& value );
STC_PUBLIC CBinaryWriter& operator <<( CBinaryWriter& ms, const OutputFilePath& value );

STC_PUBLIC std::ostream& operator <<( std::ostream& os, const stc::framework::OutputFilePath& value );

}
}


#endif  

